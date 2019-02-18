#include "translator.hpp"

Translator::Translator()
{
    regex = "";
}

QString Translator::star(QString text)
{
    QString s = "(" + text + ")*";
    return s;
}

void Translator::brzozowskiMethod(Automaton automaton) //TO DO options desactiver les incontrolables, les invisibles
{
    int automatonStatesNumber = automaton.getStateList().size(), i, j;
    QVector<QString> b(automatonStatesNumber, NULL);
    QVector<QVector<QString>> a(automatonStatesNumber,b);
    QString tmp;
    //Initialisation
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
            b[i] = "";
    }

    for(i = 0; i < automaton.getTransitionList().size(); i++)
    {
        Transition t = automaton.getTransition(i);
        if(a[t.getSource()][t.getDest()].isNull())
            a[t.getSource()][t.getDest()] = automaton.getEvent(t.getEvent()).getLabel();
        else {
            /*TO DO plusieurs transitions d'un etat vers un autre*/
        }
        //TO DO qdebug test
    }

    //Solving
    for(int n = automatonStatesNumber-1; n > -1; n--)
    {
        //B[n] := star(A[n,n]) . B[n]
        if(!a[n][n].isNull())
        {
            if(b[n].isNull())
                b[n] = star(a[n][n]);
            else
                b[n].append(star(a[n][n]));
            for(j = 0; j < n; j++)
            {
                //A[n,j] := star(A[n,n]) . A[n,j];
                if(a[n][j].isNull())
                    a[n][j] = star(a[n][n]);
                else
                    a[n][j].append(star(a[n][n]));
            }
        }
        for(i = 0; i < n; i++)
        {
            //B[i] += A[i,n] . B[n]
            tmp = QString();
            if(!a[i][n].isNull() && !b[n].isNull())
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
}
