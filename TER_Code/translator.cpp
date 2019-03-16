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
    int indiceInit, i, j;
    QVector<int> statesSet, initState, statesSetTempo;
    QMap<QVector<int>, QMultiMap<QVector<int>, QString>> expressionMap;
    QMap<QVector<int>, QMultiMap<QVector<int>, QString>>::iterator iterator, iterator2;
    QMultiMap<QVector<int>, QString> temporaryMap, temporaryMap2;
    QString expression, tmp;


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

    //Etape 2 : Traitement des ensembles. Nous partons du dernier élément de la QMap jusqu'à revenir au début, et faire un traitement
    iterator = expressionMap.end();
    iterator--;
    while(iterator != expressionMap.begin())
    {
        //Nous commençons par contanéner ensemble les différentes chaines de même clé
        temporaryMap = expressionMap[iterator.key()];
        foreach(statesSetTempo, temporaryMap.keys()) //A VERIFIER
        {
            expression = "";
            //Si la clé est présente plusieurs fois, nous concaténons le tout
            if(temporaryMap.count(statesSetTempo) > 1)
            {
                expression.append(temporaryMap.take(statesSetTempo));
                while(temporaryMap.contains(statesSetTempo))
                {
                    tmp = temporaryMap.take(statesSetTempo);
                    if(tmp.size() != 0)
                    {
                        if (expression.size() == 0)
                            expression = tmp;
                        else
                        {
                            expression.append("+");
                            expression.append(tmp);
                        }
                    }
                }

                //Si la clé est identique à l'itérateur (donc si nous bouclons!), nous appelons la fonction star, sinon nous lui ajoutons juste des parenthèses
                if(statesSetTempo == iterator.key())
                {
                    expression = star(expression);
                    temporaryMap.insert(statesSetTempo, expression);
                }
                else
                {
                    expression.prepend("(");
                    expression.append(")");
                    temporaryMap.insert(statesSetTempo, expression);
                }
            }else if(statesSetTempo == iterator.key()) //Sinon, nous vérifions si la clé est identique à la clé de l'itérateur, donc si nous bouclons sur lui-même
            {
                expression.append(temporaryMap.take(statesSetTempo));
                expression = star(expression);
                temporaryMap.insert(statesSetTempo, expression);
            }
        }

        //Au besoin, si la clé de l'itérateur est présente dans la map, la fonction star a déjà été utilisée donc nous l'injectons dans les autres expressions
        if(temporaryMap.contains(iterator.key()))
        {
            expression = temporaryMap.take((iterator.key()));
            foreach(statesSetTempo, temporaryMap.keys())
            {
                tmp = temporaryMap.take(statesSetTempo);
                tmp.append(expression);
                temporaryMap.insert(statesSetTempo, tmp);
            }
        }

        //Ensuite, nous ajoutons le contenu des éléments déjà traités dans celui-ci
    }
}
