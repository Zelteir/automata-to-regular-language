#include "translator.hpp"
#include <QDebug>


Translator::Translator()
{
    regex = "";
}

//int count(const QRegExp &rx) const
//int count(const QRegularExpression &re) const

QString Translator::star(QString text)
{
    QString s;
    if (text.size() == 0)
        s = "";
    else if(text.size() == 1)
        s = text + "*";
    else if(text[0] == ('(') && text[text.size()-1] == (')')) //Si la chaine de caractères commence et fini par des parenthèses, nous vérifions si elles sont liées
    {
        //Pour cela, nous utilisons un compteur (méthode naive) : en parcourant la chaine de caractères, nous augmentons le compteur à la présence de '(' et diminuons en présence de ')'.
        //Si le compteur tombe à 0 avant la fin, les parenthèses ne sont pas liées!
        int i=1, compteur = 1;
        while(compteur!=0 && i != text.size()-1)
        {
            if(text[i] == '(')
                compteur++;
            else if (text[i] == ')')
                compteur--;
            i++;
        }
        if (compteur == 0)
            s = "(" + text + ")*";
        else
            s = text + "*";
    }
    else
        s = "(" + text + ")*";
    return s;
}

void Translator::brzozowskiMethod(Automaton automaton, bool ignoreUnobservable, bool ignoreUncontrolable)
{
    int automatonStatesNumber = automaton.getIdState();
    //Vecteur : liste selon les états RECEVEURS
    //QMultiMap : int : numéro de l'état qui donnera son expression, QString : l'expression qu'il recevra
    QVector<QMultiMap<int, QString>> expressionList(automatonStatesNumber);
    QMap<int, bool> traitement;
    Transition t;
    int i, j, k, l, m;
    QList<QString> expressionsTempo1, expressionsTempo2;
    QString tmp, tmp2;
    QList<bool> traitementTempo;


    //Etape 1 : Initialisation des expressions et du traitement à faux
    //-1 pour "l'état initial", donnera au final les expressions finies
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i))
        {
            if (automaton.getState(i).getInitial())
                expressionList[i].insert(-1, "");
        }
        traitement.insert(i, false);
        traitementTempo.insert(i, false);
    }
    traitementTempo.insert(automatonStatesNumber, false);

    //Ensuite, ajout de toutes les transitions dans les multiMaps (par défaut, un état transite de plusieurs manières sur un autre, nous mettons tous ces mots sur une même expression)
    for(i = 0; i < automaton.getIdTransition(); i++)
    {
        if(automaton.getTransitionList()->contains(i))
        {
            t = automaton.getTransition(i);
            //Si la transition fait partie de ce que nous ignorons, nous ajoutons la transition avec une chaine vide (pas le mot vide!)
            if((automaton.getEvent(t.getEvent()).getObservable() == false && ignoreUnobservable==true) || (automaton.getEvent(t.getEvent()).getControlable() == false && ignoreUncontrolable==true))
            {
                //Nous ajoutons l'existance de la transition sur une chaine vide UNIQUEMENT si elle n'existe déjà pas, afin d'autoriser une transition par défaut
                if(!expressionList[t.getDest()].contains(t.getSource()))
                    expressionList[t.getDest()].insert(t.getSource(), "");
            }
            else
            {
                //Transition Unique (si aucune transition n'existe pour le moment avec cette clé)
                if(!expressionList[t.getDest()].contains(t.getSource()))
                    expressionList[t.getDest()].insert(t.getSource(), automaton.getEvent(t.getEvent()).getName());
                else
                {
                    //Transition Multiple (si la clé existe déjà, nous rajoutons le nouveau mot dans l'expression. Les parenthèses seront rajoutées après toute la lecture.)
                    tmp = (expressionList[t.getDest()]).take(t.getSource());
                    if(tmp != "")   //Dans le cas où la transition vide existait, nous n'ajoutons PAS le +, car il y aura une transition visible dans ce cas et nous ne souhaitons pas de + inutilement
                        tmp.append("+");
                    tmp.append(automaton.getEvent(t.getEvent()).getName());
                    (expressionList[t.getDest()]).insert(t.getSource(), tmp);
                }
            }
        }
    }
    //Etape 1.1 : ajout des parenthèses au besoin dans les expressions (facilitera les complétions)
    for(i = 0; i < automatonStatesNumber; i++)
    {
        foreach(k, expressionList[i].keys()) //Suffisant car une seule clé lors de l'initialisation
        {
            tmp = expressionList[i].value(k);
            if(tmp.contains('+')) //Si plusieurs transitions sont possibles, on aura la présence d'un "+" et nous mettrons des parenthèses à cette expression
            {
                tmp.prepend("(");
                tmp.append(")");
                expressionList[i].remove(k);
                (expressionList[i]).insert(k, tmp);
            }
        }
    }

    //Etape 2 : solving
    //Etape 2.1 : suppression des états non-finaux (identique à 2.2)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && !automaton.getState(i).getAccepting())
        {
            //Si il existe au moins une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons toutes ces transitions que nous allons supprimer de la map
                expressionsTempo1 = (expressionList[i]).values(i);
                (expressionList[i]).remove(i);
                tmp = "";
                while(!expressionsTempo1.isEmpty())
                {
                    tmp2 = expressionsTempo1.takeFirst();
                    if(tmp.length() == 0)
                        tmp = tmp2;
                    else
                    {
                        tmp.append("+"); //les transitions sur le mot vide ne sont PAS traitées ici
                        tmp.append(tmp2);
                    }
                }

                tmp = star(tmp);

                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;
                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    if(traitementTempo[j+1] == false)
                    {
                        //Pour cela, nous retirons toutes les expressions clé par clé (dans le cas où la clé est utilisée plusieurs fois)
                        expressionsTempo1 = (expressionList[i]).values(j);
                        (expressionList[i]).remove(j);

                        //Puis pour chaque expression, nous lui ajoutons l'expression étoilée et nous insérons le tout dans la MultiMap à la clé correspondante
                        while(!expressionsTempo1.isEmpty())
                        {
                            tmp2 = expressionsTempo1.takeFirst();
                            tmp2.append(tmp);
                            (expressionList[i]).insert(j, tmp2);
                        }
                        traitementTempo[j+1] = true;
                    }
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (en ignorant les cas déjà traités)
            for(j = 0; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite
                if(traitement[j] == false && i != j && expressionList[j].contains(i))
                {
                    //Nous traiterons chaque expression
                    expressionsTempo1 = (expressionList[j]).values(i);
                    (expressionList[j]).remove(i);
                    while(!expressionsTempo1.isEmpty())
                    {
                        tmp = expressionsTempo1.takeFirst();
                        for(k=0;k<=automatonStatesNumber;k++)
                            traitementTempo[k] = false;
                        //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                        foreach(l, expressionList[i].keys())
                        {
                            if(traitementTempo[l+1] == false)
                            {
                                //Nous traiterons chaque expression
                                expressionsTempo2 = (expressionList[i]).values(l);
                                for(m=0; m<expressionsTempo2.count(); m++)
                                {
                                    //Expression qui a injecter dans l'état j
                                    tmp2 = tmp;
                                    tmp2.prepend(expressionsTempo2[m]);

                                    //Nous insérons ainsi l'expression dans l'état, donnant la possibilité d'une multiple transition pour un même état (clé déjà existante)
                                    (expressionList[j]).insert(l, tmp2);
                                }

                                traitementTempo[l+1] = true;
                            }
                        }
                    }
                }
            }

            traitement[i] = true;
        }
    }

    //Etape 2.2 : suppression des états finaux (similaire mais ne diffère en ne complétant que les autres états finaux)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
        {
            //Si il existe au moins une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons toutes ces transitions que nous allons supprimer de la map
                expressionsTempo1 = (expressionList[i]).values(i);
                (expressionList[i]).remove(i);
                tmp = "";
                while(!expressionsTempo1.isEmpty())
                {
                    tmp2 = expressionsTempo1.takeFirst();
                    if(tmp.length() == 0)
                        tmp = tmp2;
                    else
                    {
                        tmp.append("+"); //les transitions sur le mot vide ne sont PAS traitées ici
                        tmp.append(tmp2);
                    }
                }

                tmp = star(tmp);

                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;
                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    if(traitementTempo[j+1] == false)
                    {
                        //Pour cela, nous retirons toutes les expressions clé par clé (dans le cas où la clé est utilisée plusieurs fois)
                        expressionsTempo1 = (expressionList[i]).values(j);
                        (expressionList[i]).remove(j);

                        //Puis pour chaque expression, nous lui ajoutons l'expression étoilée et nous insérons le tout dans la MultiMap à la clé correspondante
                        while(!expressionsTempo1.isEmpty())
                        {
                            tmp2 = expressionsTempo1.takeFirst();
                            tmp2.append(tmp);
                            (expressionList[i]).insert(j, tmp2);
                        }
                        traitementTempo[j+1] = true;
                    }
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (les états précédents devraient déjà être traités, d'où le i+1)
            for(j = i+1; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite (excluant donc les états traités et les non-acceptants)
                if(automaton.getStateList()->contains(j) && traitement[j] == false && automaton.getState(j).getAccepting() && expressionList[j].contains(i))
                {
                    //Nous traiterons chaque expression
                    expressionsTempo1 = (expressionList[j]).values(i);
                    (expressionList[j]).remove(i);
                    while(!expressionsTempo1.isEmpty())
                    {
                        tmp = expressionsTempo1.takeFirst();
                        for(k=0;k<=automatonStatesNumber;k++)
                            traitementTempo[k] = false;
                        //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                        foreach(l, expressionList[i].keys())
                        {
                            if(traitementTempo[l+1] == false)
                            {
                                //Nous traiterons chaque expression
                                expressionsTempo2 = (expressionList[i]).values(l);
                                for(m=0; m<expressionsTempo2.count(); m++)
                                {
                                    //Expression qui a injecter dans l'état j
                                    tmp2 = tmp;
                                    tmp2.prepend(expressionsTempo2[m]);

                                    //Nous insérons ainsi l'expression dans l'état, donnant la possibilité d'une multiple transition pour un même état (clé déjà existante)
                                    (expressionList[j]).insert(l, tmp2);
                                }

                                traitementTempo[l+1] = true;
                            }
                        }
                    }
                }
            }

            traitement[i] = true;
        }
    }

    //Etape 3 : "Injection" des expressions des états finaux dans le sens inverse (pour y ajouter les boucles déjà traitées sur certains sommets)
    //Le dernier état est déjà traité
    for(i=automatonStatesNumber-2; i>-1; i--)
    {
        //Le cas n'est à traiter que pour les états acceptants
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
        {
            //Si le cas est valide nous récupérons les expressions à partir des sommets déjà traités
            for (j=automatonStatesNumber-1; j>i ; j--)
            {
                expressionsTempo1 = (expressionList[j]).values(-1);
                if(expressionList[i].contains(j))
                {
                    for(k=0; k<expressionsTempo1.count(); k++)
                    {
                        tmp = expressionsTempo1[k];

                        expressionsTempo2 = (expressionList[i]).values(j);
                        for(l = 0; l < expressionsTempo2.count(); l++)
                        {
                            tmp2 = expressionsTempo2[l];
                            tmp2.prepend(tmp);
                            (expressionList[i]).insert(-1, tmp2);
                        }
                    }
                }
            }
        }
    }

    //Etape 4 : récupération des expressions de tous les états finaux
    QString finalRegex;
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
        {
            //Dans le cas où l'état initial est aussi acceptant, nous ajoutons le mot vide
            if(automaton.getState(i).getInitial())
            {
                if(finalRegex.isEmpty())
                    finalRegex = "$";
                else
                    finalRegex.prepend("$+");
            }

            //Complétion par les autres transitions
            //Déjà, le traitement est inutile si l'on a un état final n'ayant jamais de cas l'utilisant
            if(!(expressionList[i]).values(-1).isEmpty())
            {
                //Ensuite, nous récupérons la liste des expressions finales obtenable par la clé -1
                QList<QString> valeurs = (expressionList[i]).values(-1);
                //Puis, pour chacune de ces expressions, nous ajoutons son contenu (une "optimisation" serait de trier les éléments pour supprimer les doublons)
                for(j=0;j<valeurs.count();j++)
                {
                    //Nous excluons les cas de mots vides
                    if(!valeurs[j].isEmpty())
                    {
                        //Nous vérifions si le final Regex est actuellement vide
                        if(finalRegex.isEmpty())
                            finalRegex = valeurs[j];
                        else //Sinon, nous ajoutons l'expression dans le final regex
                        {
                            if(finalRegex[finalRegex.size()-1] != '+')
                                finalRegex.append("+");
                            finalRegex.append(valeurs[j]);
                        }
                    }
                }
            }
        }
    }

    //Bien que l'expression sera une énumération grossière, nous pouvons obtenir tous les cas possibles
    regex = finalRegex;
}

void Translator::reverseBrzozowski(Automaton automaton, bool ignoreUnobservable, bool ignoreUncontrolable)
{
    int automatonStatesNumber = automaton.getIdState();
    int automatonEventsNumber = automaton.getIdEvent();
    int automatonTransitionsNumber = automaton.getIdTransition();
    int nombreStatesSet = 0, indiceInit;
    int i, j, k, compteurMap;
    QMap<int, QVector<int>> mapStatesSet;
    QVector<int> statesSet, statesSetTempo, statesSetTempo2;
    QMap<int, QMultiMap<int, QString>> mapStatesSetsTransitionsList;
    QMultiMap<int, QString> traitementMultiMap, traitementMultiMap2;
    QString expression, tmp;
    bool verification;

    //Etape 0 : récupération du states-set d'initialisation : l'ensemble des états acceptants
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
            statesSet.append(i);
    }

    //Ajout du states-set dans la QMap ainsi que la creation de la QMap
    mapStatesSet.insert(nombreStatesSet, statesSet);
    mapStatesSetsTransitionsList.insert(nombreStatesSet, QMultiMap<int, QString>());
    nombreStatesSet++;

    //Etape 1 : Création de nouveaux states-sets à partir des précédents créés jusqu'à ne plus pouvoir
    compteurMap = 0;
    //Tant que nous n'avons pas vérifiés tous les states-sets déjà créés
    while(compteurMap != mapStatesSet.size())
    {
        //Nous utiliserons le prochain states-set non traité
        statesSetTempo = mapStatesSet[compteurMap];
        //Pour chaque MOT possible, nous calculons les states-set allant avec le states-set actuel
        for(i=0;i<automatonEventsNumber;i++)
        {
            if(automaton.getEventList()->contains(i))
            {
                statesSet.clear();
                //Nous vérifions les transitions pour voir si un des états du states-set de l'itérateur est destination de la transition et si la transition est du bon mot
                for(j=0; j< automatonTransitionsNumber; j++)
                {
                    //Si la transition est valide, nous ajoutons l'état de source dans le States-set si il n'est pas déjà présent
                    if(automaton.getTransitionList()->contains(j) && statesSetTempo.contains(automaton.getTransition(j).getDest()) && automaton.getTransition(j).getEvent() == i && !statesSet.contains(automaton.getTransition(j).getSource()))
                    {
                        statesSet.append(automaton.getTransition(j).getSource());
                    }
                }
                //Une fois le states-set calculé, nous vérifions si il n'est PAS vide. Dans le cas où il est vide, nous ne faisons rien
                if(!statesSet.isEmpty())
                {
                    j = 0;
                    verification = false;
                    //Dans le cas où le states-set calculé n'est pas vide, nous vérifions si il existe déjà dans notre QMap
                    while(j<mapStatesSet.size() && verification == false)
                    {
                        statesSetTempo2 = mapStatesSet[j];
                        if(statesSet.size() == statesSetTempo2.size()) //Si les 2 states-set sont de même longueur, nous pouvons vérifier si leurs contenus sont identiques (ne pas utiliser l'operateur == qui vérifie l'ordre)
                        {
                            verification = true;
                            k = 0;
                            while(k < statesSet.size() && verification == true)
                            {
                                if(statesSetTempo2.contains(statesSet.at(k)))
                                    k++;
                                else
                                    verification = false;
                            }
                        }
                        if(verification == false)
                            j++;

                    }
                    //Si le states-set n'existe pas, nous le creons dans le QVector ainsi que sa QMap
                    if(verification == false)
                    {
                        mapStatesSet.insert(nombreStatesSet, statesSet);
                        nombreStatesSet+=1;
                        mapStatesSetsTransitionsList.insert(nombreStatesSet, QMultiMap<int, QString>());
                    }
                    //Dans tous les cas, nous récupérons la QMultiMap du states-set que nous traitons afin d'en ajouter la transition
                    traitementMultiMap = mapStatesSetsTransitionsList.take(compteurMap);

                    //Cependant, nous devons vérifier si le mot doit être ignoré
                    if((ignoreUncontrolable == true && automaton.getEvent(i).getControlable() == false) || (ignoreUnobservable  == true && automaton.getEvent(i).getObservable() == false))
                        traitementMultiMap.insert(j, "");
                    else
                        traitementMultiMap.insert(j, automaton.getEvent(i).getName());
                    //Une fois la transition ajoutée, nous remettons la MultiMap dans la Map
                    mapStatesSetsTransitionsList.insert(compteurMap, traitementMultiMap);
                }
            }
        }
        compteurMap++;
    }

    //Etape 1.1 : traitement pré-résolution. Une fois toutes les transitions traitees, nous recuperons le numero de l'etat initial
    i = 0;
    indiceInit = -1;
    while(i < automatonStatesNumber && indiceInit == -1)
    {
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getInitial())
            indiceInit = i;
        i++;
    }
    //Puis, double traitement des MultiMap : réduction des expressions par clé, ajout de la cle -1 dans la MultiMap si l'etat initial est inclus dans le States-set
    for(compteurMap=0; compteurMap < mapStatesSet.size(); compteurMap++)
    {
        statesSetTempo = mapStatesSet[compteurMap];
        traitementMultiMap = mapStatesSetsTransitionsList.take(compteurMap);

        //Pour chaque cle, nous reduisons au maximum afin de n'avoir plus qu'une cle, en gérant les parenthèses pour les traitements futurs
        foreach(i, traitementMultiMap.keys())
        {
            if(automaton.getStateList()->contains(i))
            {
                if(traitementMultiMap.count(i)>1)
                {
                    expression = traitementMultiMap.take(i);
                    while(traitementMultiMap.count(i)!=0)  //Tant que nous avons une expression a simplifier
                    {
                        tmp = traitementMultiMap.take(i);
                        if(tmp.length() != 0)
                        {
                            if(expression.length() == 0)
                                expression = tmp;
                            else //Dans le cas où les 2 expressions sont non nulles
                            {
                                if(!expression.contains("("))//Nous ajoutons les parenthèses au besoin
                                    expression.prepend("(");
                                else
                                    expression.remove(")");
                                expression.append("+");
                                expression.append(automaton.getEvent(i).getName());
                                expression.append(")");
                            }
                        }
                    }
                    //Nous réinsérons la nouvelle expression ainsi calculée
                    traitementMultiMap.insert(i, expression);
                }
            }
        }

        //Nous ajoutons dans la MultiMap l'indice -1 si le StatesSet contient l'etat initial
        if(statesSetTempo.contains(indiceInit))
            traitementMultiMap.insert(-1, "");

        //Nous réinsérons la nouvelle MultiMap
        mapStatesSetsTransitionsList.insert(compteurMap, traitementMultiMap);
    }

    //Etape 2 : Solving. Nous partons du dernier States-set enregistré jusqu'à revenir au début (non inclu), et faire un traitement
    for(compteurMap = nombreStatesSet-1; compteurMap > 0 ; compteurMap--)
    {
        traitementMultiMap = mapStatesSetsTransitionsList.take(compteurMap);
        //Pour ce traitement, nous faisons une résolution à partir des StatesSets déjà traités
        foreach(j, traitementMultiMap.keys())
        {
            //Le StatesSet est déjà traité si sa valeur d'enregistrement est plus grande que celle du StatesSet que nous traitons
            if(compteurMap<j)
            {
                //Nous récupérons ainsi toutes les expressions du StatesSet que nous concatenons avec celle du StatesSet actuel afin de former la nouvelle expression a la nouvelle cle
                traitementMultiMap2 = mapStatesSetsTransitionsList.value(j);
                expression = traitementMultiMap.take(j);
                foreach(i, traitementMultiMap2.keys())
                {
                    tmp = traitementMultiMap2.value(i);
                    tmp.append(expression);
                    traitementMultiMap.insert(i, tmp);
                }
            }
        }

        //Une fois les traitements effectués, nous effectuons à nouveau les rassemblements d'expressions par clé au besoin
        foreach(j, traitementMultiMap.keys())
        {
            if(traitementMultiMap.count(j) > 1)
            {
                verification = false; //Servira à ajouter au besoin des parenthèses
                expression = traitementMultiMap.take(j);
                while(traitementMultiMap.contains(j))
                {
                    tmp = traitementMultiMap.take(j);
                    if(expression.size() == 0)
                        expression.append(tmp);
                    else if(tmp.size() != 0)
                    {
                        verification = true;
                        expression.append("+");
                        expression.append(tmp);
                    }
                }

                if(verification)
                {
                    expression.prepend("(");
                    expression.append(")");
                }

                traitementMultiMap.insert(j, expression);
            }
        }

        //Une fois les rassemblements effectués, nous appliquons les effets de boucles via la fonction star si nécessaire
        if(traitementMultiMap.contains(compteurMap))
        {
            expression = star(traitementMultiMap.take(compteurMap));
            foreach(j, traitementMultiMap.keys())
            {
                tmp = traitementMultiMap.take(j);
                tmp.append(expression);
                traitementMultiMap.insert(j, tmp);
            }
        }

        //Une fois les sous-étapes précédentes appliquées, nous remettons la MultiMap dans la map originelle
        mapStatesSetsTransitionsList.insert(compteurMap, traitementMultiMap);
    }

    //Etape 3 : Solving final. Les précédents states-set étant traités, nous pouvons modifier le states-set ne contenant que les états finaux pour obtenir l'expression finale.
    //Cette expression sera obtenue que lorsqu'il ne restera qu'une seule clé dans la QMap : la clé -1
    compteurMap = 0;
    traitementMultiMap = mapStatesSetsTransitionsList.take(compteurMap);
    while(traitementMultiMap.count() != 1)
    {
        //La procédure de solving se fait de la même manière que précédemment
        foreach(j, traitementMultiMap.keys())
        {
            //Le StatesSet est déjà traité si sa valeur d'enregistrement est plus grande que celle du StatesSet que nous traitons
            if(compteurMap<j)
            {
                //Nous récupérons ainsi toutes les expressions du StatesSet que nous concatenons avec celle du StatesSet actuel afin de former la nouvelle expression a la nouvelle cle
                traitementMultiMap2 = mapStatesSetsTransitionsList.value(j);
                expression = traitementMultiMap.take(j);
                foreach(i, traitementMultiMap2.keys())
                {
                    tmp = traitementMultiMap2.value(i);
                    tmp.append(expression);
                    traitementMultiMap.insert(i, tmp);
                }
            }
        }

        //Une fois les traitements effectués, nous effectuons à nouveau les rassemblements d'expressions par clé au besoin
        foreach(j, traitementMultiMap.keys())
        {
            if(traitementMultiMap.count(j) > 1)
            {
                verification = false; //Servira à ajouter au besoin des parenthèses
                expression = traitementMultiMap.take(j);
                while(traitementMultiMap.contains(j))
                {
                    tmp = traitementMultiMap.take(j);
                    if(expression.size() == 0)
                        expression.append(tmp);
                    else if(tmp.size() != 0)
                    {
                        verification = true;
                        expression.append("+");
                        expression.append(tmp);
                    }
                }

                if(verification)
                {
                    expression.prepend("(");
                    expression.append(")");
                }

                traitementMultiMap.insert(j, expression);
            }
        }

        //Une fois les rassemblements effectués, nous appliquons les effets de boucles via la fonction star si nécessaire
        if(traitementMultiMap.contains(compteurMap))
        {
            expression = star(traitementMultiMap.take(compteurMap));
            foreach(j, traitementMultiMap.keys())
            {
                tmp = traitementMultiMap.take(j);
                tmp.append(expression);
                traitementMultiMap.insert(j, tmp);
            }
        }
    }

    //Une fois qu'il ne reste plus qu'une clé, nous devrions obtenir notre regex final
    regex = traitementMultiMap.value(-1);
    //Avant de quitter la fonction, nous vérifions si le mot vide est accessible : est-ce que l'etat initial est aussi acceptant?
    if(automaton.getStateList()->contains(indiceInit) && automaton.getState(indiceInit).getAccepting())
    {
        if(regex.isEmpty())
            regex = "$";
        else
            regex.prepend("$+");
    }
}

const QString plus("+");
const QString etoile("*");
const QString leftParent("(");
const QString rightParent(")");
const QString empty("$");
const std::shared_ptr<QString> plusPtr = std::make_shared<QString>(plus);
const std::shared_ptr<QString> etoilePtr = std::make_shared<QString>(etoile);
const std::shared_ptr<QString> leftParentPtr = std::make_shared<QString>(leftParent);
const std::shared_ptr<QString> rightParentPtr = std::make_shared<QString>(rightParent);
const std::shared_ptr<QString> emptyPtr = std::make_shared<QString>(empty);

QVector<std::shared_ptr<QString>> Translator::starV2(QVector<std::shared_ptr<QString>> text)
{
    QVector<std::shared_ptr<QString>> s;
    if (text.size() == 0)
        s = QVector<std::shared_ptr<QString>>();
    else if(text.size() == 1)
    {
        s = text;
        s.append(etoilePtr);
    }
    else if(text.first() == leftParentPtr && text.last() == rightParentPtr) //Si la chaine de caractères commence et fini par des parenthèses, nous vérifions si elles sont liées
    {
        //Pour cela, nous utilisons un compteur (méthode naive) : en parcourant la chaine de caractères, nous augmentons le compteur à la présence de '(' et diminuons en présence de ')'.
        //Si le compteur tombe à 0 avant la fin, les parenthèses ne sont pas liées!
        int i=1, compteur = 1;
        while(compteur!=0 && i != text.size()-1)
        {
            if(text[i] == leftParentPtr)
                compteur++;
            else if (text[i] == rightParentPtr)
                compteur--;
            i++;
        }
        if (compteur == 0)
        {
            s = text;
            s.prepend(leftParentPtr);
            s.append(rightParentPtr);
            s.append(etoilePtr);
        }
        else
        {
            s = text;
            s.append(etoilePtr);
        }
    }
    else
    {
        s = text;
        s.prepend(leftParentPtr);
        s.append(rightParentPtr);
        s.append(etoilePtr);
    }
    return s;
}

void Translator::brzozowskiMethodV2(Automaton automaton, bool ignoreUnobservable, bool ignoreUncontrolable)
{
    int automatonStatesNumber = automaton.getIdState();
    //Vecteur : liste selon les états RECEVEURS
    //QMultiMap : int : numéro de l'état qui donnera son expression, QString : l'expression qu'il recevra
    QVector<QMultiMap<int, QVector<std::shared_ptr<QString>>>> expressionList(automatonStatesNumber);
    QMap<int, bool> traitement;
    Transition t;
    int i, j, k, l, m;
    QList<QVector<std::shared_ptr<QString>>> expressionsTempo1, expressionsTempo2;
    QVector<std::shared_ptr<QString>> tmp, tmp2;
    QList<bool> traitementTempo;
    QMap<int, std::shared_ptr<QString>> arrayEventPtr;

    for(i = 0; i < automaton.getIdEvent(); i++)
    {
        if(automaton.getEventList()->contains(i))
            arrayEventPtr.insert(i, std::make_shared<QString>(automaton.getEvent(i).getName()));
    }

    //Etape 1 : Initialisation des expressions et du traitement à faux
    //-1 pour "l'état initial", donnera au final les expressions finies
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i))
        {
            if(automaton.getState(i).getInitial())
                expressionList[i].insert(-1, QVector<std::shared_ptr<QString>>());
        }
        traitement.insert(i, false);
        traitementTempo.insert(i, false);
    }
    traitementTempo.insert(automatonStatesNumber, false);

    //Ensuite, ajout de toutes les transitions dans les multiMaps (par défaut, un état transite de plusieurs manières sur un autre, nous mettons tous ces mots sur une même expression)
    for(i = 0; i < automaton.getIdTransition(); i++)
    {
        if(automaton.getTransitionList()->contains(i))
        {
            t = automaton.getTransition(i);
            //Si la transition fait partie de ce que nous ignorons, nous ajoutons la transition avec une chaine vide (pas le mot vide!)
            if((automaton.getEvent(t.getEvent()).getObservable() == false && ignoreUnobservable==true) || (automaton.getEvent(t.getEvent()).getControlable() == false && ignoreUncontrolable==true))
            {
                //Nous ajoutons l'existance de la transition sur une chaine vide UNIQUEMENT si elle n'existe déjà pas, afin d'autoriser une transition par défaut
                if(!expressionList[t.getDest()].contains(t.getSource()))
                    expressionList[t.getDest()].insert(t.getSource(), QVector<std::shared_ptr<QString>>());
            }
            else
            {
                //Transition Unique (si aucune transition n'existe pour le moment avec cette clé)
                if(!expressionList[t.getDest()].contains(t.getSource()))
                {
                    expressionList[t.getDest()].insert(t.getSource(), QVector<std::shared_ptr<QString>>({arrayEventPtr.value(t.getEvent())}));
                }
                else
                {
                    //Transition Multiple (si la clé existe déjà, nous rajoutons le nouveau mot dans l'expression. Les parenthèses seront rajoutées après toute la lecture.)
                    tmp = (expressionList[t.getDest()]).take(t.getSource());
                    if(tmp.size()>0)   //Dans le cas où la transition vide existait, nous n'ajoutons PAS le +, car il y aura une transition visible dans ce cas et nous ne souhaitons pas de + inutilement
                        tmp.append(plusPtr);
                    tmp.append(arrayEventPtr.value(t.getEvent()));
                    (expressionList[t.getDest()]).insert(t.getSource(), tmp);
                }
            }
        }
    }
    //Etape 1.1 : ajout des parenthèses au besoin dans les expressions (facilitera les complétions)
    for(i = 0; i < automatonStatesNumber; i++)
    {
        foreach(k, expressionList[i].keys()) //Suffisant car une seule clé lors de l'initialisation
        {
            tmp = expressionList[i].value(k);
            if(tmp.indexOf(plusPtr) > -1) //Si plusieurs transitions sont possibles, on aura la présence d'un "+" et nous mettrons des parenthèses à cette expression
            {
                tmp.prepend(leftParentPtr);
                tmp.append(rightParentPtr);
                expressionList[i].remove(k);
                (expressionList[i]).insert(k, tmp);
            }
        }
    }

    //Etape 2 : solving
    //Etape 2.1 : suppression des états non-finaux (identique à 2.2)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && !automaton.getState(i).getAccepting())
        {
            //Si il existe au moins une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons toutes ces transitions que nous allons supprimer de la map
                expressionsTempo1 = (expressionList[i]).values(i);
                (expressionList[i]).remove(i);
                tmp.clear();
                while(!expressionsTempo1.isEmpty())
                {
                    tmp2 = expressionsTempo1.takeFirst();
                    if(tmp.length() == 0)
                        tmp = tmp2;
                    else
                    {
                        tmp.append(plusPtr); //les transitions sur le mot vide ne sont PAS traitées ici
                        tmp.append(tmp2);
                    }
                }

                tmp = starV2(tmp);

                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;
                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    if(traitementTempo[j+1] == false)
                    {
                        //Pour cela, nous retirons toutes les expressions clé par clé (dans le cas où la clé est utilisée plusieurs fois)
                        expressionsTempo1 = (expressionList[i]).values(j);
                        (expressionList[i]).remove(j);

                        //Puis pour chaque expression, nous lui ajoutons l'expression étoilée et nous insérons le tout dans la MultiMap à la clé correspondante
                        while(!expressionsTempo1.isEmpty())
                        {
                            tmp2 = expressionsTempo1.takeFirst();
                            tmp2.append(tmp);
                            (expressionList[i]).insert(j, tmp2);
                        }
                        traitementTempo[j+1] = true;
                    }
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (en ignorant les cas déjà traités)
            for(j = 0; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite
                if(traitement[j] == false && i != j && expressionList[j].contains(i))
                {
                    //Nous traiterons chaque expression
                    expressionsTempo1 = (expressionList[j]).values(i);
                    (expressionList[j]).remove(i);
                    while(!expressionsTempo1.isEmpty())
                    {
                        tmp = expressionsTempo1.takeFirst();
                        for(k=0;k<=automatonStatesNumber;k++)
                            traitementTempo[k] = false;
                        //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                        foreach(l, expressionList[i].keys())
                        {
                            if(traitementTempo[l+1] == false)
                            {
                                //Nous traiterons chaque expression
                                expressionsTempo2 = (expressionList[i]).values(l);
                                for(m=0; m<expressionsTempo2.count(); m++)
                                {
                                    //Expression qui a injecter dans l'état j
                                    tmp2 = expressionsTempo2[m];
                                    tmp2.append(tmp);
                                    //Nous insérons ainsi l'expression dans l'état, donnant la possibilité d'une multiple transition pour un même état (clé déjà existante)
                                    (expressionList[j]).insert(l, tmp2);
                                }

                                traitementTempo[l+1] = true;
                            }
                        }
                    }
                }
            }

            traitement[i] = true;
        }
    }

    //Etape 2.2 : suppression des états finaux (similaire mais ne diffère en ne complétant que les autres états finaux)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
        {
            //Si il existe au moins une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons toutes ces transitions que nous allons supprimer de la map
                expressionsTempo1 = (expressionList[i]).values(i);
                (expressionList[i]).remove(i);
                tmp.clear();
                while(!expressionsTempo1.isEmpty())
                {
                    tmp2 = expressionsTempo1.takeFirst();
                    if(tmp.length() == 0)
                        tmp = tmp2;
                    else
                    {
                        tmp.append(plusPtr); //les transitions sur le mot vide ne sont PAS traitées ici
                        tmp.append(tmp2);
                    }
                }

                tmp = starV2(tmp);

                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;
                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    if(traitementTempo[j+1] == false)
                    {
                        //Pour cela, nous retirons toutes les expressions clé par clé (dans le cas où la clé est utilisée plusieurs fois)
                        expressionsTempo1 = (expressionList[i]).values(j);
                        (expressionList[i]).remove(j);

                        //Puis pour chaque expression, nous lui ajoutons l'expression étoilée et nous insérons le tout dans la MultiMap à la clé correspondante
                        while(!expressionsTempo1.isEmpty())
                        {
                            tmp2 = expressionsTempo1.takeFirst();
                            tmp2.append(tmp);
                            (expressionList[i]).insert(j, tmp2);
                        }
                        traitementTempo[j+1] = true;
                    }
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (les états précédents devraient déjà être traités, d'où le i+1)
            for(j = i+1; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite (excluant donc les états traités et les non-acceptants)
                if(automaton.getStateList()->contains(j) && traitement[j] == false && automaton.getState(j).getAccepting() && expressionList[j].contains(i))
                {
                    //Nous traiterons chaque expression
                    expressionsTempo1 = (expressionList[j]).values(i);
                    (expressionList[j]).remove(i);
                    while(!expressionsTempo1.isEmpty())
                    {
                        tmp = expressionsTempo1.takeFirst();
                        for(k=0;k<=automatonStatesNumber;k++)
                            traitementTempo[k] = false;
                        //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                        foreach(l, expressionList[i].keys())
                        {
                            if(traitementTempo[l+1] == false)
                            {
                                //Nous traiterons chaque expression
                                expressionsTempo2 = (expressionList[i]).values(l);
                                for(m=0; m<expressionsTempo2.count(); m++)
                                {
                                    //Expression qui a injecter dans l'état j
                                    tmp2 = expressionsTempo2[m];
                                    tmp2.append(tmp);

                                    //Nous insérons ainsi l'expression dans l'état, donnant la possibilité d'une multiple transition pour un même état (clé déjà existante)
                                    (expressionList[j]).insert(l, tmp2);
                                }

                                traitementTempo[l+1] = true;
                            }
                        }
                    }
                }
            }

            traitement[i] = true;
        }
    }

    //Etape 3 : "Injection" des expressions des états finaux dans le sens inverse (pour y ajouter les boucles déjà traitées sur certains sommets)
    //Le dernier état est déjà traité
    for(i=automatonStatesNumber-2; i>-1; i--)
    {
        //Le cas n'est à traiter que pour les états acceptants
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
        {
            //Si le cas est valide nous récupérons les expressions à partir des sommets déjà traités
            for (j=automatonStatesNumber-1; j>i ; j--)
            {
                expressionsTempo1 = (expressionList[j]).values(-1);
                if(expressionList[i].contains(j))
                {
                    for(k=0; k<expressionsTempo1.count(); k++)
                    {
                        tmp = expressionsTempo1[k];

                        expressionsTempo2 = (expressionList[i]).values(j);
                        for(l = 0; l < expressionsTempo2.count(); l++)
                        {
                            tmp2 = tmp;
                            tmp2.append(expressionsTempo2[l]);
                            (expressionList[i]).insert(-1, tmp2);
                        }
                    }
                }
            }
        }
    }

    //Etape 4 : récupération des expressions de tous les états finaux
    QString finalRegex = "";
    QList<QVector<std::shared_ptr<QString>>> valeurs;
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getStateList()->contains(i) && automaton.getState(i).getAccepting())
        {
            //Dans le cas où l'état initial est aussi acceptant, nous ajoutons le mot vide
            if(automaton.getState(i).getInitial())
            {
                if(finalRegex.isEmpty())
                    finalRegex = "$";
                else
                    finalRegex.prepend("$+");
            }

            //Complétion par les autres transitions
            //Déjà, le traitement est inutile si l'on a un état final n'ayant jamais de cas l'utilisant
            if(!(expressionList[i]).values(-1).isEmpty())
            {
                //Ensuite, nous récupérons la liste des expressions finales obtenable par la clé -1
                valeurs = (expressionList[i]).values(-1);
                //Puis, pour chacune de ces expressions, nous ajoutons son contenu (une "optimisation" serait de trier les éléments pour supprimer les doublons)
                for(j=0;j<valeurs.count();j++)
                {
                    //Nous excluons les cas de mots vides
                    if(!valeurs[j].isEmpty())
                    {
                        //Nous vérifions si le final Regex est actuellement vide
                        if(finalRegex.size() == 0)
                        {
                            for(k = 0; k < valeurs[j].size();k++)
                                finalRegex.append(*valeurs[j][k]);
                        }
                        else //Sinon, nous ajoutons l'expression dans le final regex
                        {
                            if(finalRegex[finalRegex.size()-1] != '+')
                                finalRegex.append("+");
                            for(k = 0; k < valeurs[j].size();k++)
                                finalRegex.append(*valeurs[j][k]);
                        }
                    }
                }
            }
        }
    }

    //Bien que l'expression sera une énumération grossière, nous pouvons obtenir tous les cas possibles
    regex = finalRegex;
}

void Translator::reduction(Automaton)
{

}

/*
 * Does not work
 */
void Translator::transitive_Closure(Automaton automaton, bool ignoreUnobservable, bool ignoreUncontrolable)
{
    int size = automaton.getStateList()->last().getId()+2;
    QVector<QVector<QString>> map(size);
    QVector<QVector<QString>> backMap(size);
    QString finalExpr = "", tmp;
    int initState = 0;

#pragma omp parallel
    {
#pragma omp for schedule(static)
        for(int i = 0; i < size; i++)
        {
            map[i].resize(size);
            backMap[i].resize(size);
            for(int j = 0; j < size; j++)
            {
                if(i == j)
                    backMap[i][j] = "$";
            }
        }
    }

    for(Transition t : *automaton.getTransitionList())
    {
        if(!((automaton.getEvent(t.getEvent()).getObservable() == false && ignoreUnobservable==true) || (automaton.getEvent(t.getEvent()).getControlable() == false && ignoreUncontrolable==true)))
        {
            if(backMap[t.getSource()+1][t.getDest()+1].isEmpty())
                backMap[t.getSource()+1][t.getDest()+1] = "";
            else
                backMap[t.getSource()+1][t.getDest()+1] += "+";
            backMap[t.getSource()+1][t.getDest()+1] += automaton.getEvent(t.getEvent()).getName();
        }
    }
    /*TO DO remplacer 3e dimention par back matrice*/
    //R[i,j,k] = R[i,j,k-1] + R[i,k,k-1] . (R[k,k,k-1])* . R[k,j,k-1]
    for(int k = 1; k < size; k++)
    {
#pragma omp parallel private(tmp)
        {
#pragma omp for collapse(2) schedule(static)
            for(int i = 1; i < size; i++)
            {
                for(int j = 1; j < size; j++)
                {
                    tmp = backMap[i][k] + star(backMap[k][k]) + backMap[k][j];
                    if(!backMap[i][j].isEmpty())
                    {
                        if(!tmp.isEmpty())
                        {
                            map[i][j] = "" +
                                    ((backMap[i][j].size() > 1)?"(" + backMap[i][j] + ")+" :backMap[i][j] + "+") +
                                    tmp;
                        }
                        else
                            map[i][j] = "" + backMap[i][j];
                    }
                    else
                        map[i][j] = "" + tmp;
                }
            }
        }
        if(k < size - 1)
            backMap.swap(map);
    }
    for(State s : *automaton.getStateList())
    {
        if(s.getInitial())
        {
            initState = s.getId()+1;
            break;
        }
    }
    for(State s : *automaton.getStateList())
    {
        if(s.getInitial() && s.getAccepting())
        {
            if(!finalExpr.isEmpty())
            {
                finalExpr.prepend("+");
                finalExpr.prepend("$");
                finalExpr.append("+");
                finalExpr.append(map[initState][s.getId() + 1][size-1]);
            }
            else
            {
                finalExpr.prepend("+");
                finalExpr.prepend("$");
                finalExpr.append(map[initState][s.getId() + 1][size-1]);
            }
        }
        else if (s.getAccepting())
        {
            if(!finalExpr.isEmpty())
                finalExpr.append("+");
            finalExpr.append(map[initState][s.getId() + 1][size-1]);
        }
    }
    regex = finalExpr;
}

/*
 * Automaton minimization by Hopcroft algorithm
 *
 * Does not work
 */
Automaton Translator::automatonMinimization(Automaton param)
{
    int size = param.getStateList()->last().getId()+1, it =0;
    Automaton minimized;
    QList<int> tmp1, tmp2, extract, accept, nonAccept;
    QList<QList<int>> waiting, set, newSet;
    QVector<QVector<QString>> transitionMatrix(param.getStateList()->size());
    QList<QList<int>> splitted;

    //contruct transitionMatrix
#pragma omp parallel
    {
#pragma omp for schedule(static)
        for(int i = 0; i < size; i++)
        {
            transitionMatrix[i].resize(size);
        }
    }
    for(Transition t : *param.getTransitionList())
    {
        transitionMatrix[t.getSource()][t.getDest()] = param.getEvent(t.getEvent()).getName();
    }

    //construct set and waiting
    for(State s: *param.getStateList())
    {
        if(s.getAccepting())
            accept.append(s.getId());
        else
            nonAccept.append(s.getId());
    }

    //if no two distinct sets (accepting and non-accepting)
    if(accept.isEmpty() || nonAccept.isEmpty())
        return param;
    waiting.append((accept.length() < nonAccept.length())?accept:nonAccept);
    set.append(accept);
    set.append(nonAccept);
    newSet = set;
    newSet.detach();

    //main loop
    while(waiting.length())
    {
        it++;
        extract = waiting.takeFirst();
        for(Event e : *param.getEventList())
        {
            for(QList<int> list : set)
            {
                if(list.size() > 1)
                {
                    splitted = split(list, extract, e, transitionMatrix);
                    tmp1 = splitted[0];
                    tmp2 = splitted[1];
                    if(tmp1.size() > 0 && tmp2.size() > 0)
                    {
                        newSet.removeOne(list);
                        newSet.append(tmp1);
                        newSet.append(tmp2);
                        if(tmp1.size()<tmp2.size())
                            waiting.append(tmp1);
                        else
                            waiting.append(tmp2);
                    }
                }
            }
        }
        if(waiting.length() == 0 && it == 1)
            waiting.append((extract == accept)?nonAccept:accept);
        set = newSet;
        set.detach();
    }

    minimized = reconstruct(set,param,transitionMatrix);
    return minimized;
}

QList<QList<int>> Translator::split(QList<int> source, QList<int> dest, Event e, QVector<QVector<QString>> transitions)
{
    QList<QList<int>> res;
    QList<int> in;
    QList<int> out;
    bool var;
    for(int i : source)
    {
        var = false;
        for(int j : dest)
        {
            if(transitions[i][j] == e.getName())
            {
                var = true;
                in.append(i);
                break;
            }
        }
        if(!var)
            out.append(i);
    }

    res.append(in);
    res.append(out);
    return res;
}

Automaton Translator::reconstruct(QList<QList<int>> newStates, Automaton a, QVector<QVector<QString>> transitions)
{
    Automaton res;
    bool accepting, initial;
    int i, j, k, idDest = 0, idTrans = 0;

    res.setEventList(*a.getEventList());
    while (res.getIdEvent() < a.getIdEvent())
        res.incrEvent();

    //new states for the minimized automaton
    for(i = 0; i < newStates.size();i++)
    {
        accepting = false;
        initial = false;
        for(j = 0;(!initial || !accepting) && j < newStates[i].size();j++)
        {
            if(a.getState(newStates[i][j]).getInitial())
                initial = true;
            if(a.getState(newStates[i][j]).getAccepting())
                accepting = true;
        }
        res.getStateList()->insert(i, State(i,"",initial,accepting));
        res.incrState();
    }

    //new transitions for the minimized automaton
    for(i =0; i < newStates.size(); i++)
    {
        for(State l : *a.getStateList())
        {
            if(!transitions[newStates[i][0]][l.getId()].isEmpty())
            {
                idDest = -1;
                for(j = 0; j < newStates.size(); j++)
                {
                    for(k = 0; k < newStates[j].size(); k++)
                    {
                        if(l.getId() == newStates[j][k])
                        {
                            idDest = j;
                            break;
                        }
                    }
                    if(idDest == j)
                        break;
                }
                for(Event e: *a.getEventList())
                {
                    if(e.getName() == transitions[newStates[i][0]][l.getId()])
                    {
                        res.getTransitionList()->insert(idTrans, Transition(idTrans, i, idDest, e.getId()));
                        idTrans++;
                        res.incrTransition();
                        break;
                    }
                }
            }
        }
    }
    return res;
}


/*
 * Automaton minimization by Hopcroft algorithm (teacher's version)
*/
Automaton Translator::automatonMinimizationV2(Automaton param)
{
    int size = param.getStateList()->last().getId()+1, s;
    Automaton minimized;
    QList<int> B2, extract, accept, nonAccept;
    QList<QList<int>> waiting, set, newSet;
    QVector<QVector<QString>> transitionMatrix(param.getStateList()->size());
    QList<QList<int>> splitted;
    bool change = true;

    //contruct transitionMatrix
#pragma omp parallel
    {
#pragma omp for schedule(static)
        for(int i = 0; i < size; i++)
        {
            transitionMatrix[i].resize(size);
        }
    }
    for(Transition t : *param.getTransitionList())
    {
        transitionMatrix[t.getSource()][t.getDest()] = param.getEvent(t.getEvent()).getName();
    }

    //construct set and waiting
    for(State s: *param.getStateList())
    {
        if(s.getAccepting())
            accept.append(s.getId());
        else
            nonAccept.append(s.getId());
    }

    //if no two distinct sets (accepting and non-accepting)
    if(accept.isEmpty() || nonAccept.isEmpty())
        return param;
    waiting.append((accept.length() < nonAccept.length())?accept:nonAccept);
    set.append(accept);
    set.append(nonAccept);

    while(change)
    {
        change = false;
        newSet.clear();
        for(QList<int> B : set)
        {
            while(B.size())
            {
                B2.clear();
                s = B.takeFirst();
                B2.append(s);
                for(int s2 : B)
                {
                    if(equivalent(s, s2, set, transitionMatrix))
                    {
                        B2.append(s2);
                        B.removeOne(s2);
                    }
                }
                newSet.append(B2);
                if(!B.isEmpty())
                    change = true;
            }
        }
        set = newSet;
        set.detach();
    }

    minimized = reconstruct(set,param,transitionMatrix);
    return minimized;
}

/*
 * Return if two states are aquivalents in a set
 * Two states are equivalents if for all of their transitions, their destinations are in the same sets
 * If even one transition goes to a set for which the other state have no transition to, they aren't equivalent
*/
bool Translator::equivalent(int a, int b, QList<QList<int>> set, QVector<QVector<QString>> transitionMatrix)
{
    QList<int> inA, inB;

    for(QList<int> list : set)
    {
        if(list.contains(a))
        {
            for(int i = 0; i < transitionMatrix[a].size(); i++)
            {
                if(!transitionMatrix[a][i].isEmpty())
                {
                    for(int j = 0; j < set.size(); j++)
                    {
                        if(set[j].contains(i))
                        {
                            inA.append(j);
                            break;
                        }
                    }
                }
            }
        }
        if(list.contains(b))
        {
            for(int i = 0; i < transitionMatrix[b].size(); i++)
            {
                if(!transitionMatrix[b][i].isEmpty())
                {
                    for(int j = 0; j < set.size(); j++)
                    {
                        if(set[j].contains(i))
                        {
                            inB.append(j);
                            break;
                        }
                    }
                }
            }
        }
    }

    for(int k : inA)
        if(!inB.contains(k))
            return false;
    for(int k : inB)
        if(!inA.contains(k))
            return false;

    return true;
}
