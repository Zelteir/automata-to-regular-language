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
    //Vecteur : liste selon les états RECEVEURS
    //QMap : int : numéro de l'état qui donnera son expression, QString : l'expression qu'il recevra
    int automatonStatesNumber = automaton.getStateList().size();
    QVector<QMap<int, QString>> expressionList(automatonStatesNumber);
    Transition t;
    QString tmp, tmp2;
    int i, j, k;

    //Etape 1 : Initialisation des expressions
    //-1 pour "l'expression fixe"
    for (i = 0; i < automatonStatesNumber; i++)
        expressionList[i].insert(-1, "");


    //Ensuite, ajout de toutes les transitions dans les maps
    for(i = 0; i < automaton.getTransitionList().size(); i++)
    {
        t = automaton.getTransition(i);
        //Unique translation
        if(!expressionList[t.getDest()].contains(t.getSource()))
            expressionList[t.getDest()].insert(t.getSource(), automaton.getEvent(t.getEvent()).getLabel());
        else { //TODO : ajouter correctement des parenthèses
            //Multiple translation from a state to another one
            tmp = (expressionList[t.getDest()]).take(t.getSource());
            tmp.append("+");
            tmp.append(automaton.getEvent(t.getEvent()).getLabel());
            (expressionList[t.getDest()]).insert(t.getSource(), tmp); //SET
        }
        //TO DO qdebug test
    }
    //Etape 1.1 : ajout des parenthèses au besoin dans les expressions (facilitera les complétions)
    for(i = 0; i < automatonStatesNumber; i++)
    {
        foreach(k, expressionList[i].keys())
        {
            tmp = expressionList[i].value(k);
            if(tmp.contains('+')) //Si plusieurs transitions sont possibles, on aura la présence d'un "+" et nous mettron
            {
                tmp.prepend("(");
                tmp.append(")");
                expressionList[i].remove(k);
                (expressionList[i]).insert(k, tmp);
            }
        }
    }

    //Etape 2 : solving
    //Etape 2.1 : suppression des états non-finaux
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(!automaton.getState(i).getAccepting())
        {
            //Si il existe une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons l'expression que nous allons supprimer de la map
                tmp = star((expressionList[i]).take(i));
                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    tmp2 = (expressionList[i]).take(j);
                    tmp2.append(tmp);
                    (expressionList[i]).insert(j, tmp); //SET
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (possibilité d'amélioration en ne traitant pas les états déjà traités)
            for(j = 0; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite
                if(i != j && expressionList[j].contains(i))
                {
                    tmp = (expressionList[j]).take(i);
                    //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                    foreach(k, expressionList[i].keys())
                    {
                        //Expression qui a injecter dans l'état j
                        tmp2 = tmp;
                        tmp2.prepend((expressionList[i]).value(k));

                        //Dans le cas où il n'y a pas la clé k, juste affecter
                        if(!expressionList[j].contains(k))
                            (expressionList[j]).insert(k, tmp2); //SET
                        //Dans le cas où la clé k existe, ajouter le "+" puis ajouter tmp2 (attention si transition sur le mot vide et optimisation des parenthèses possible)
                        else
                        {
                            tmp2.prepend("(");
                            tmp2.append(")+");
                            tmp2.append((expressionList[j]).take(k));
                            (expressionList[j]).insert(k, tmp2); //SET
                        }
                    }
                }
            }
        }
    }

    //Etape 2.2 : suppression des états finaux (similaire mais ne diffère en ne complétant que les autres états finaux)
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
        {
            //Si il existe une transition d'un état à lui-même
            if(expressionList[i].contains(i))
            {
                //Nous récupérons l'expression que nous allons supprimer de la map
                tmp = star((expressionList[i]).take(i));
                //Puis, pour chaque autre "transition" possible, nous ajoutons la transition étoilée à la fin de chaque autre transition de cet état
                foreach(j, expressionList[i].keys())
                {
                    tmp2 = (expressionList[i]).take(j);
                    tmp2.append(tmp);
                    (expressionList[i]).insert(j, tmp); //SET
                }
            }

            //Ensuite, ajout du contenu de cet état aux autres états le nécessitant (possibilité d'amélioration en ne traitant pas les états déjà traités)
            for(j = 0; j < automatonStatesNumber; j++)
            {
                //Inutile sur l'état lui-même ET si l'état le nécessite
                if(i != j && automaton.getState(j).getAccepting() && expressionList[j].contains(i))
                {
                    tmp = (expressionList[j]).take(i);
                    //Si le nécessite, nous rajoutons le contenu de l'état i dans j
                    foreach(k, expressionList[i].keys())
                    {
                        //Expression qui a injecter dans l'état j
                        tmp2 = tmp;
                        tmp2.prepend((expressionList[i]).value(k));

                        //Dans le cas où il n'y a pas la clé k, juste affecter
                        if(!expressionList[j].contains(k))
                            (expressionList[j]).insert(k, tmp2); //SET
                        //Dans le cas où la clé k existe, ajouter le "+" puis ajouter tmp2 (attention si transition sur le mot vide et optimisation des parenthèses possible)
                        else
                        {
                            tmp2.prepend("(");
                            tmp2.append(")+");
                            tmp2.append((expressionList[j]).take(k));
                            (expressionList[j]).insert(k, tmp2); //SET
                        }
                    }
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
            //Première complétion
            if(finalRegex.isEmpty() && !(expressionList[i]).value(-1).isEmpty())
                finalRegex = (expressionList[i]).value(-1);
            else if(!(expressionList[i]).value(-1).isEmpty())//Le final régex est déjà initialisé, donc ajout de l'autre résultat possible
            {
                if(finalRegex[finalRegex.size()-1] != '+')
                    finalRegex.append("+");
                finalRegex.append((expressionList[i]).value(-1));
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
