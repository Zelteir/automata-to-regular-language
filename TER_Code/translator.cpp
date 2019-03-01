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
    if(text.size()== 1)
        s = text + "*";
    else
        s = "(" + text + ")*";
    return s;
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
    /*for (i = 0; i < automatonStatesNumber; i++)
    {
        if(!automaton.getState(i).getAccepting())
        {
            //Si il existe au moins une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons toutes ces transitions que nous allons supprimer de la map
                tmp = "";
                while(expressionList[i].contains(i))
                {
                    tmp2 = (expressionList[i]).take(i);
                    if(tmp2.length() != 0) //Dans le cas où la transition n'est pas sur le mot vide
                    {
                        if(tmp2.length() > 1) //Nous mettons des parenthèses par prudence pour les cas de chaines concaténées (voir comment simplifier)
                        {
                            tmp2.prepend("(");
                            tmp2.append(")");
                        }
                        if(tmp.length() == 0)
                            tmp = tmp2;
                        else
                        {
                            tmp.append("+");
                            tmp.append(tmp2);
                        }
                    }
                }

                tmp = star(tmp);

                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    tmp2 = (expressionList[i]).take(j);
                    tmp2.append(tmp);
                    (expressionList[i]).insert(j, tmp2);
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (en ignorant les cas déjà traités)
            for(j = 0; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite
                if(traitement[j] == false && i != j && expressionList[j].contains(i))
                {
                    tmp = (expressionList[j]).take(i);
                    //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                    foreach(k, expressionList[i].keys())
                    {
                        //Expression qui a injecter dans l'état j
                        tmp2 = tmp;
                        tmp2.prepend((expressionList[i]).value(k));

                        //Nous insérons ainsi l'expression dans l'état, donnant la possibilité d'une multiple transition pour un même état (clé déjà existante)
                        (expressionList[j]).insert(k, tmp2);
                    }
                }
            }

            traitement[i] = true;

            //Etape 2.1.1 : Verification du contenu
            qDebug() << "Verification de la modification a l'etape 2.1 en s'occupant de l'etat " << i;
            for(l = 0; l < automatonStatesNumber; l++)
            {
                qDebug() << "Etat " << l;
                foreach(k, expressionList[l].keys())
                {
                    QList<QString> expressions = expressionList[l].values(k);
                    for(j = 0; j<expressions.count(); j++)
                        qDebug() << "cle : " << k << " expression : " << expressions[j];
                }
            }
        }
    }*/

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

    //Etape 3 : récupération des expressions de tous les états finaux
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
            if(finalRegex.isEmpty() && !(expressionList[i]).value(-1).isEmpty())
                finalRegex = (expressionList[i]).value(-1);
            else //Le final régex est déjà initialisé, donc ajout de l'autre résultat possible
            {
                QList<QString> valeurs = (expressionList[i]).values(-1);
                for(j=0;j<valeurs.count();j++)
                {
                    if(!valeurs[j].isEmpty())
                    {
                        if(finalRegex[finalRegex.size()-1] != '+')
                            finalRegex.append("+");
                        finalRegex.append(valeurs[j]);
                    }
                }
            }
        }
    }

    regex = finalRegex;
}

/*void Translator::brzozowskiMethod(Automaton automaton) //TO DO options desactiver les incontrolables, les invisibles
{
    int automatonStatesNumber = automaton.getStateList().size(), i, j;
    QVector<QString> b(automatonStatesNumber, NULL);
    QVector<QVector<QString>> a(automatonStatesNumber,b);
    QString tmp;
    Transition t;

    //Initialisation of b
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
            b[i] = "";
    }

    //Initialisation of a
    for(i = 0; i < automaton.getTransitionList().size(); i++)
    {
        t = automaton.getTransition(i);
        //Unique translation
        if(a[t.getSource()][t.getDest()].isNull())
            a[t.getSource()][t.getDest()] = automaton.getEvent(t.getEvent()).getLabel();
        else {  // -> a verifier (ajouter des parenthèses pour le cas a(b+c)?)
            //Multiple translation from a state to another one
            a[t.getSource()][t.getDest()].append("+");
            a[t.getSource()][t.getDest()].append(automaton.getEvent(t.getEvent()).getLabel());
        }
        //TO DO qdebug test
    }*/

    /*//Vérification
    for(i=0; i< automatonStatesNumber; i++)
    {
         for(j=0; j< automatonStatesNumber; j++)
         {
             if(a[i][j].isNull())
                 qDebug() << "vide ";
             else
                 qDebug() << a[i][j] << " ";
         }
         qDebug() << "\n";
    }*/

    //Solving
    /*for(int n = automatonStatesNumber-1; n > -1; n--)
    {
        //B[n] := star(A[n,n]) . B[n]
        if(!a[n][n].isNull())
        {
            if(b[n].isNull())
                b[n] = star(a[n][n]);
            else
                b[n].prepend(star(a[n][n]));
            for(j = 0; j <= n; j++)
            {
                //A[n,j] := star(A[n,n]) . A[n,j];
                if(a[n][j].isNull())
                    a[n][j] = star(a[n][n]);
                else
                    a[n][j].prepend(star(a[n][n]));
            }
        }
        for(i = 0; i <= n; i++)
        {
            //B[i] += A[i,n] . B[n]
            if(!a[i][n].isNull())
            {
                if(a[i][n].size()!=1) //TO DO : pour économiser des parenthèses, modifier en "si contient un + mais pas de parenthèse", autre problématique a+a(b+c), gérer cas de multiples + non imbriqués
                {
                    b[i].append("(");
                    b[i].append(a[i][n]);
                    b[i].append(")");
                }else
                    b[i].append(a[i][n]);
            }

            if(!b[n].isNull())
                b[i].append(b[n]);

            //A[i,j] += A[i,n] . A[n,j]
            for(j = 0; j <= n; j++)
            {
                if(!a[i][n].isNull())
                {
                    if(a[i][n].size()!=1) //TO DO : pour économiser des parenthèses, modifier en "si contient un + mais pas de parenthèse", autre problématique a+a(b+c), gérer cas de multiples + non imbriqués
                    {
                        a[i][j].append("+");
                        //a[i][j].append("(");
                        a[i][j].append(a[i][n]);
                        //a[i][j].append(")");
                    }else
                        a[i][j].append(a[i][n]);
                }

                if(!a[n][j].isNull())
                {
                    if(a[n][j].size()!=1) //TO DO : pour économiser des parenthèses, modifier en "si contient un + mais pas de parenthèse", autre problématique a+a(b+c), gérer cas de multiples + non imbriqués
                    {
                        //a[i][j].append("(");
                        a[i][j].append(a[n][j]);
                        //a[i][j].append(")");
                    }else
                        a[i][j].append(a[n][j]);
                }

            }

            /*if(!a[i][n].isNull() && !b[n].isNull())
            {
                tmp = a[i][n];
                tmp.append(b[n]);
            }else if (!a[i][n].isNull()) {
                tmp = a[i][n];
            }else if (!b[n].isNull()) {
                tmp = b[n];
            }
            if(!b[i].isNull() && !tmp.isNull())
            {
                if(!b[i].isEmpty())
                    b[i].append("+");
                b[i].append(tmp);
            }else {
                b[i] = tmp;
            }
            for(j = 0; j < n; j++)
            {
                //A[i,j] += A[i,n] . A[n,j]
                tmp = QString();
                if(!a[i][n].isNull() && !a[n][j].isNull())
                {
                    tmp = a[i][n];
                    tmp.append(a[n][j]);
                }else if (!a[i][n].isNull()) {
                    tmp = a[i][n];
                }else if (!a[n][j].isNull()) {
                    tmp = a[n][j];
                }
                if(!a[i][j].isNull() && !tmp.isNull())
                {
                    if(!a[i][j].isEmpty())
                        a[i][j].append("+");
                    a[i][j].append(tmp);
                }else {
                    a[i][j] = tmp;
                }
            }
        }
    }

    regex = b[0];
}*/

void Translator::reverseBrzozowski(Automaton automaton)
{

}
