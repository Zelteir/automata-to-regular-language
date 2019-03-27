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
    int automatonStatesNumber = automaton.getStateList()->size();
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
        if(automaton.getState(i).getInitial())
            expressionList[i].insert(-1, "");
        traitement.insert(i, false);
        traitementTempo.insert(i, false);
    }
    traitementTempo.insert(automatonStatesNumber, false);

    //Ensuite, ajout de toutes les transitions dans les multiMaps (par défaut, un état transite de plusieurs manières sur un autre, nous mettons tous ces mots sur une même expression)
    for(i = 0; i < automaton.getTransitionList()->size(); i++)
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
                expressionList[t.getDest()].insert(t.getSource(), automaton.getEvent(t.getEvent()).getLabel());
            else
            {
                //Transition Multiple (si la clé existe déjà, nous rajoutons le nouveau mot dans l'expression. Les parenthèses seront rajoutées après toute la lecture.)
                tmp = (expressionList[t.getDest()]).take(t.getSource());
                if(tmp != "")   //Dans le cas où la transition vide existait, nous n'ajoutons PAS le +, car il y aura une transition visible dans ce cas et nous ne souhaitons pas de + inutilement
                    tmp.append("+");
                tmp.append(automaton.getEvent(t.getEvent()).getLabel());
                (expressionList[t.getDest()]).insert(t.getSource(), tmp);
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

    /*//Etape 1.2 : Verification du contenu (à supprimer quand le code sera opérationnel)
    qDebug() << "Verification de l'initialisation";
    for(i = 0; i < automatonStatesNumber; i++)
    {
        qDebug() << "Etat " << i;
        foreach(k, expressionList[i].keys())
        {
            QList<QString> expressions = expressionList[i].values(k);
            for(j = 0; j<expressions.count(); j++)
                qDebug() << "cle : " << k << " expression : " << expressions[j];
        }
    }*/

    //Etape 2 : solving
    //Etape 2.1 : suppression des états non-finaux (identique à 2.2)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(!automaton.getState(i).getAccepting())
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


            /*//Etape 2.1.1 : Verification du contenu (a effacer une fois fini)
            qDebug() << "Verification de la modification a l'etape 2.1 en s'occupant de l'etat " << i;
            for(j = 0; j < automatonStatesNumber; j++)
            {
                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;

                qDebug() << "Etat " << j;
                foreach(k, expressionList[j].keys())
                {
                    if(traitementTempo[k+1] == false)
                    {
                        expressionsTempo1 = (expressionList[j]).values(k);
                        for(l = 0; l < expressionsTempo1.count(); l++)
                            qDebug() << "cle : " << k << " expression : " << expressionsTempo1[l];
                    }
                    traitementTempo[k+1] = true;
                }
            }*/
        }
    }

    //Etape 2.2 : suppression des états finaux (similaire mais ne diffère en ne complétant que les autres états finaux)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
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
                if(traitement[j] == false && automaton.getState(j).getAccepting() && expressionList[j].contains(i))
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


            /*//Etape 2.2.1 : Verification du contenu (a effacer une fois fini)
            qDebug() << "Verification de la modification a l'etape 2.2 en s'occupant de l'etat " << i;
            for(j = 0; j < automatonStatesNumber; j++)
            {
                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;

                qDebug() << "Etat " << j;
                foreach(k, expressionList[j].keys())
                {
                    if(traitementTempo[k+1] == false)
                    {
                        expressionsTempo1 = (expressionList[j]).values(k);
                        for(l = 0; l < expressionsTempo1.count(); l++)
                            qDebug() << "cle : " << k << " expression : " << expressionsTempo1[l];
                    }
                    traitementTempo[k+1] = true;
                }
            }*/
        }
    }

    //Etape 3 : "Injection" des expressions des états finaux dans le sens inverse (pour y ajouter les boucles déjà traitées sur certains sommets)
    //Le dernier état est déjà traité
    for(i=automatonStatesNumber-2; i>-1; i--)
    {
        //Le cas n'est à traiter que pour les états acceptants
        if(automaton.getState(i).getAccepting())
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

            //Etape 3.1 : Verification du contenu (a effacer une fois fini)
            /*qDebug() << "Verification de la modification a l'etape 3 en s'occupant de l'etat " << i;
            for(j = 0; j < automatonStatesNumber; j++)
            {
                for(k=0;k<=automatonStatesNumber;k++)
                    traitementTempo[k] = false;

                qDebug() << "Etat " << j;
                foreach(k, expressionList[j].keys())
                {
                    if(traitementTempo[k+1] == false)
                    {
                        expressionsTempo1 = (expressionList[j]).values(k);
                        for(l = 0; l < expressionsTempo1.count(); l++)
                            qDebug() << "cle : " << k << " expression : " << expressionsTempo1[l];
                    }
                    traitementTempo[k+1] = true;
                }
            }*/
        }
    }

    //Etape 4 : récupération des expressions de tous les états finaux
    QString finalRegex;
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
        {
            //Dans le cas où l'état initial est aussi acceptant, nous ajoutons le mot vide
            if(automaton.getState(i).getInitial())
            {
                if(finalRegex.isEmpty())
                    finalRegex = "Є";
                else
                    finalRegex.prepend("Є+");
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
    int automatonStatesNumber = automaton.getStateList()->size();
    int automatonEventsNumber = automaton.getEventList()->size();
    int automatonTransitionsNumber = automaton.getTransitionList()->size();
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
        if(automaton.getState(i).getAccepting())
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
            statesSet.clear();
            //Nous vérifions les transitions pour voir si un des états du states-set de l'itérateur est destination de la transition et si la transition est du bon mot
            for(j=0; j< automatonTransitionsNumber; j++)
            {
                //Si la transition est valide, nous ajoutons le sommet de source dans le States-set
                if(statesSetTempo.contains(automaton.getTransition(j).getDest()) && automaton.getTransition(j).getEvent() == i)
                    statesSet.append(automaton.getTransition(j).getSource());
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
                    if(statesSetTempo.size() == statesSetTempo2.size()) //Si les 2 states-set sont de même longueur, nous pouvons vérifier si leurs contenus sont identiques (ne pas utiliser l'operateur == qui vérifie l'ordre)
                    {
                        verification = true;
                        k = 0;
                        while(k < statesSetTempo.size() && verification == true)
                        {
                            if(statesSetTempo2.contains(statesSetTempo.at(k)))
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
                    nombreStatesSet++;
                    mapStatesSetsTransitionsList.insert(nombreStatesSet, QMultiMap<int, QString>());
                }
                //Dans tous les cas, nous récupérons la QMultiMap du states-set que nous traitons afin d'en ajouter la transition
                traitementMultiMap = mapStatesSetsTransitionsList.take(compteurMap);

                //Cependant, nous devons vérifier si le mot doit être ignoré
                if((ignoreUnobservable == true && automaton.getEvent(i).getControlable() == false) || (ignoreUncontrolable == true && automaton.getEvent(i).getObservable() == false))
                    traitementMultiMap.insert(j, "");
                else
                    traitementMultiMap.insert(j, automaton.getEvent(i).getLabel());
                //Une fois la transition ajoutée, nous remettons la MultiMap dans la Map
                mapStatesSetsTransitionsList.insert(compteurMap, traitementMultiMap);
            }
        }
        compteurMap++;
    }

    //Etape 1.1 : traitement pré-résolution. Une fois toutes les transitions traitees, nous recuperons le numero de l'etat initial
    i = 0;
    indiceInit = -1;
    while(i < automatonStatesNumber && indiceInit == -1)
    {
        if(automaton.getState(i).getInitial())
            indiceInit = i;
    }
    //Puis, double traitement des MultiMap : réduction des expressions par clé, ajout de la cle -1 dans la MultiMap si l'etat initial est inclus dans le States-set
    for(compteurMap=0; compteurMap < mapStatesSet.size(); compteurMap++)
    {
        statesSetTempo = mapStatesSet[compteurMap];
        traitementMultiMap = mapStatesSetsTransitionsList.take(compteurMap);

        //Pour chaque cle, nous reduisons au maximum afin de n'avoir plus qu'une cle, en gérant les parenthèses pour les traitements futurs
        foreach(i, traitementMultiMap.keys())
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
                            expression.append(automaton.getEvent(i).getLabel());
                            expression.append(")");
                        }
                    }
                }
                //Nous réinsérons la nouvelle expression ainsi calculée
                traitementMultiMap.insert(i, expression);
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
    if(automaton.getState(indiceInit).getAccepting())
    {
        if(regex.isEmpty())
            regex = "Є";
        else
            regex.prepend("Є+");
    }

}

/*void Translator::reverseBrzozowski(Automaton automaton, bool ignoreUnobservable, bool ignoreUncontrolable)
{
    int automatonStatesNumber = automaton.getStateList()->size();
    int automatonEventsNumber = automaton.getEventList()->size();
    int automatonTransitionsNumber = automaton.getTransitionList()->size();
    int indiceInit, i, j;
    QVector<int> statesSet, initState, statesSetTempo;
    QMap<QVector<int>, QMultiMap<QVector<int>, QString>> expressionMap;
    QMap<QVector<int>, QMultiMap<QVector<int>, QString>>::iterator iterator, iterator2;
    QMultiMap<QVector<int>, QString> temporaryMap, temporaryMap2;
    QString expression, tmp;
    bool verifParenthesis;

    //Etape 0 : récupération du states-set d'initialisation : l'ensemble des états acceptants
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
            statesSet.append(i);
    }

    //Ajout du states-set dans la QMap initiale pour le 1er traitement
    expressionMap.insert(statesSet, QMultiMap<QVector<int>, QString>());
    //Etape 1 : Création de nouveaux states-sets à partir des précédents créés jusqu'à ne plus pouvoir
    iterator = expressionMap.begin();
    while(iterator != expressionMap.end())
    {
        //Pour chaque MOT possible, nous calculons les states-set allant avec le states-set actuel
        for(i=0;i<automatonEventsNumber;i++)
        {
            statesSet.clear();
            //Nous vérifions les transitions pour voir si un des états du states-set de l'itérateur est destination de la transition et si la transition est du bon mot
            for(j=0; j< automatonTransitionsNumber; j++)
            {
                //Si la transition est valide, nous ajoutons la valeur dans le States-set
                if(iterator.key().contains(automaton.getTransition(j).getSource()) && automaton.getTransition(i).getEvent() == j)
                    statesSet.append(j);
            }

            //Une fois le states-set calculé, nous vérifions si il n'est PAS vide
            if(!statesSet.isEmpty())
            {
                //Puis nous si il existe déjà dans la QMap. Si non, nous l'ajoutons. Dans tous les cas, nous ajoutons la clé/valeur dans le states-set de l'itérateur
                if(!expressionMap.contains(statesSet))
                {
                    expressionMap.insert(statesSet, QMultiMap<QVector<int>, QString>());
                    //Nous sommes obligés de faire une copie, ne pouvant pas manipuler directement (Comme le states-set est nouveau, il n'est pas déjà présent dans la map, ce qui nous fait gagner du temps à la vérification)
                    temporaryMap = expressionMap[iterator.key()];
                    //Cependant, nous devons vérifier si le mot doit être ignoré
                    if((ignoreUnobservable == true && automaton.getEvent(i).getControlable() == false) || (ignoreUncontrolable == true && automaton.getEvent(i).getObservable() == false))
                        temporaryMap.insert(statesSet, "");
                    else
                        temporaryMap.insert(statesSet, automaton.getEvent(i).getLabel());
                    expressionMap[iterator.key()] = temporaryMap;
                }
                else //Dans le cas où le states-set existe déjà, nous ajouterons la transition dans la map si la clé n'y était pas avant ou nous ajoutons le mot à la chaine pré-existante si besoin
                {
                    temporaryMap = expressionMap[iterator.key()];
                    //Dans le cas où la clé existe, nous n'ajoutons le mot si il n'est pas ignoré
                    if(temporaryMap.contains(statesSet) && !((ignoreUnobservable == true && automaton.getEvent(i).getControlable() == false) || (ignoreUncontrolable == true && automaton.getEvent(i).getObservable() == false)))
                    {
                        tmp = temporaryMap.take(statesSet);
                        if(tmp.size() != 0)
                        {
                            if(!tmp.contains("("))//Nous ajoutons les parenthèses au besoin
                                tmp.prepend("(");
                            else
                                tmp.remove(")");
                            tmp.append("+");
                            tmp.append(automaton.getEvent(i).getLabel());
                            tmp.append(")");
                        }
                        else
                            tmp.append(automaton.getEvent(i).getLabel());
                        temporaryMap.insert(statesSet, tmp);
                    }else
                    {
                        //Cependant, nous devons vérifier si le mot doit être ignoré
                        if((ignoreUnobservable == true && automaton.getEvent(i).getControlable() == false) || (ignoreUncontrolable == true && automaton.getEvent(i).getObservable() == false))
                            temporaryMap.insert(statesSet, "");
                        else
                            temporaryMap.insert(statesSet, automaton.getEvent(i).getLabel());
                    }
                    expressionMap[iterator.key()] = temporaryMap;
                }
            }
        }
        iterator++;
    }

    //Etape 1.1 : Pour chaque ensemble de la QMap, si elle contient l'état initial, nous ajoutons une chaine vide ayant pour clé un QVector contenant uniquement -1
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getInitial())
            indiceInit = i;
    }

    initState.append(-1);
    iterator = expressionMap.begin();
    while(iterator != expressionMap.end())
    {
        //Nous sommes obligés de faire une copie, ne pouvant pas manipuler directement
        temporaryMap = expressionMap[iterator.key()];

        //Ajout de la chaine vide si contient l'état initial
        if(iterator.key().contains(indiceInit))
            temporaryMap.insert(initState, "");

        expressionMap[iterator.key()] = temporaryMap;
        iterator++;
    }

    //Etape 2 : Solving. Nous partons du dernier élément de la QMap jusqu'à revenir au début, et faire un traitement
    iterator = expressionMap.end();
    iterator--;
    //Nous traiterons la dernière expression en une étape particulière
    while(iterator != expressionMap.begin())
    {
        //Durant l'étape 1, nous avons déjà concaténé les chaines, nous n'avons donc plus qu'à récupérer les contenus des maps nécessaires étant déjà traitées
        temporaryMap = expressionMap[iterator.key()];
        for(iterator2 = iterator; iterator2 != expressionMap.end(); iterator2++)
        {
            //Si la clé du states-set déjà traité est présent, nous effectuons le traitement sur celle-ci
            if(temporaryMap.contains(iterator2.key()))
            {
                expression = temporaryMap.take(iterator2.key());
                temporaryMap2 = expressionMap[iterator2.key()];
                foreach(statesSetTempo, temporaryMap2.keys()) //A VERIFIER
                {
                    tmp = temporaryMap2.value(statesSetTempo);
                    tmp.append(expression);
                    temporaryMap.insert(statesSetTempo, tmp);
                }
            }
        }

        //Une fois les traitements effectués, nous effectuons à nouveau les rassemblements d'expressions par clé au besoin
        foreach(statesSetTempo, temporaryMap.keys())
        {
            if(temporaryMap.count(statesSetTempo) > 2)
            {
                verifParenthesis = false; //Servira à ajouter au besoin des parenthèses
                expression = temporaryMap.take(statesSetTempo);
                while(temporaryMap.contains(statesSetTempo))
                {
                    tmp = temporaryMap.take(statesSetTempo);
                    if(expression.size() == 0)
                        expression.append(tmp);
                    else if(tmp.size() != 0)
                    {
                        verifParenthesis = true;
                        expression.append("+");
                        expression.append(tmp);
                    }
                }

                if(verifParenthesis)
                {
                    expression.prepend("(");
                    expression.append(")");
                }
            }
        }

        //Une fois les rassemblements effectués, nous appliquons les effets de boucles via la fonction star si nécessaire
        if(temporaryMap.contains(iterator.key()))
        {
            expression = star(temporaryMap.take(iterator.key()));
            foreach(statesSetTempo, temporaryMap.keys()) //A VERIFIER
            {
                tmp = temporaryMap.take(statesSetTempo);
                tmp.append(expression);
                temporaryMap.insert(statesSetTempo, tmp);
            }
        }

        //Une fois les sous-étapes précédentes appliquées, nous remettons la "temporaryMap" en tant que nouvelle valeur à la position de l'itérateur, puis nous passons à la valeur précédente
        expressionMap[iterator.key()] = temporaryMap;
        iterator--;
    }

    //Etape 3 : Solving final. Les précédents states-set étant traités, nous pouvons modifier le states-set ne contenant que les états finaux pour obtenir l'expression finale.
    //Cette expression sera obtenue que lorsqu'il ne restera qu'une seule clé dans la map : le QVecteur n'ayant que la clé -1
    temporaryMap = expressionMap.first();
    while(temporaryMap.count() != 1)
    {
        //La procédure de solving se fait de la même manière que précédemment
        for(iterator = expressionMap.begin(); iterator != expressionMap.end(); iterator++)
        {
            //Si la clé du states-set déjà traité est présent, nous effectuons le traitement sur celle-ci
            if(temporaryMap.contains(iterator.key()))
            {
                expression = temporaryMap.take(iterator.key());
                temporaryMap2 = expressionMap[iterator.key()];
                foreach(statesSetTempo, temporaryMap2.keys()) //A VERIFIER
                {
                    tmp = temporaryMap2.value(statesSetTempo);
                    tmp.append(expression);
                    temporaryMap.insert(statesSetTempo, tmp);
                }
            }
        }

        //Une fois les traitements effectués, nous effectuons à nouveau les rassemblements d'expressions par clé au besoin
        foreach(statesSetTempo, temporaryMap.keys())
        {
            if(temporaryMap.count(statesSetTempo) > 2)
            {
                verifParenthesis = false; //Servira à ajouter au besoin des parenthèses
                expression = temporaryMap.take(statesSetTempo);
                while(temporaryMap.contains(statesSetTempo))
                {
                    tmp = temporaryMap.take(statesSetTempo);
                    if(expression.size() == 0)
                        expression.append(tmp);
                    else if(tmp.size() != 0)
                    {
                        verifParenthesis = true;
                        expression.append("+");
                        expression.append(tmp);
                    }
                }

                if(verifParenthesis)
                {
                    expression.prepend("(");
                    expression.append(")");
                }
            }
        }

        //Une fois les rassemblements effectués, nous appliquons les effets de boucles via la fonction star si nécessaire
        if(temporaryMap.contains(iterator.key()))
        {
            expression = star(temporaryMap.take(iterator.key()));
            foreach(statesSetTempo, temporaryMap.keys()) //A VERIFIER
            {
                tmp = temporaryMap.take(statesSetTempo);
                tmp.append(expression);
                temporaryMap.insert(statesSetTempo, tmp);
            }
        }
    }

    //Une fois qu'il ne reste plus qu'une clé, nous devrions obtenir notre regex final
    regex = temporaryMap.value(initState);
}
*/
