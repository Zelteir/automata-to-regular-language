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
    else
        s = "(" + text + ")*";
    return s;
}

void Translator::brzozowskiMethod(Automaton automaton, bool ignoreNonObservable, bool ignoreUncontrolable)
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
        if((automaton.getEvent(t.getEvent()).getObservable() == false && ignoreNonObservable==true) || (automaton.getEvent(t.getEvent()).getControlable() == false && ignoreUncontrolable==true))
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

    //Etape 1.2 : Verification du contenu (à supprimer quand le code sera opérationnel)
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
    }

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


            //Etape 2.1.1 : Verification du contenu (a effacer une fois fini)
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
            }
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


            //Etape 2.2.1 : Verification du contenu (a effacer une fois fini)
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
            }
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
            qDebug() << "Verification de la modification a l'etape 3 en s'occupant de l'etat " << i;
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
            }
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

void Translator::brzozowskiMethod(Automaton automaton)
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
        //Transition Unique (si aucune transition n'existe pour le moment avec cette clé)
        if(!expressionList[t.getDest()].contains(t.getSource()))
            expressionList[t.getDest()].insert(t.getSource(), automaton.getEvent(t.getEvent()).getLabel());
        else {
            //Transition Multiple (si la clé existe déjà, nous rajoutons le nouveau mot dans l'expression. Les parenthèses seront rajoutées après toute la lecture.)
            tmp = (expressionList[t.getDest()]).take(t.getSource());
            tmp.append("+");
            tmp.append(automaton.getEvent(t.getEvent()).getLabel());
            (expressionList[t.getDest()]).insert(t.getSource(), tmp);
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

    //Etape 1.2 : Verification du contenu (à supprimer quand le code sera opérationnel)
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
    }

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


            //Etape 2.1.1 : Verification du contenu (a effacer une fois fini)
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
            }
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


            //Etape 2.2.1 : Verification du contenu (a effacer une fois fini)
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
            }
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
            qDebug() << "Verification de la modification a l'etape 3 en s'occupant de l'etat " << i;
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
            }
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

void Translator::reverseBrzozowski(Automaton automaton)
{

}
