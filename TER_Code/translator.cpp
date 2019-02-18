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
    QVector<QString> a(automatonStatesNumber * automatonStatesNumber, NULL);
    QVector<QString> b(automatonStatesNumber, NULL);
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
        if(a[t.getSource()*automatonStatesNumber + t.getDest()].isNull())
            a[t.getSource()*automatonStatesNumber + t.getDest()] = automaton.getEvent(t.getEvent()).getLabel();
        else {
            /*TO DO plusieurs transitions d'un etat vers un autre*/
        }
        //TO DO qdebug test
    }

    //Solving
    for(int n = automatonStatesNumber-1; n > -1; n--)
    {
        //B[n] := star(A[n,n]) . B[n]
        if(!a[n*automatonStatesNumber + n].isNull())
        {
            if(b[n].isNull())
                b[n] = star(a[n*automatonStatesNumber + n]);
            else
                b[n].append(star(a[n*automatonStatesNumber + n]));
            for(j = 0; j < n; j++)
            {
                //A[n,j] := star(A[n,n]) . A[n,j];
                if(a[n*automatonStatesNumber + j].isNull())
                    a[n*automatonStatesNumber + j] = star(a[n*automatonStatesNumber + n]);
                else
                    a[n*automatonStatesNumber + j].append(star(a[n*automatonStatesNumber + n]));
            }
        }
        for(i = 0; i < n; i++)
        {
            //B[i] += A[i,n] . B[n]
            tmp = QString();
            if(!a[i*automatonStatesNumber + n].isNull() && !b[n].isNull())
            {
                tmp = a[i*automatonStatesNumber + n];
                tmp.append(b[n]);
                //b[i].append(a[i*automatonStatesNumber + n]);
                //b[i].append(b[n]);
            }else if (!a[i*automatonStatesNumber + n].isNull()) {
                tmp = a[i*automatonStatesNumber + n];
                //b[i].append(a[i*automatonStatesNumber + n]);
            }else if (!b[n].isNull()) {
                tmp = b[n];
                //b[i].append(b[n]);
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
                if(!a[i*automatonStatesNumber + n].isNull() && !a[n*automatonStatesNumber + j].isNull())
                {
                    tmp = a[i*automatonStatesNumber + n];
                    tmp.append(a[n*automatonStatesNumber + j]);
                }else if (!a[i*automatonStatesNumber + n].isNull()) {
                    tmp = a[i*automatonStatesNumber + n];
                }else if (!a[n*automatonStatesNumber + j].isNull()) {
                    tmp = a[n*automatonStatesNumber + j];
                }
                if(!a[i*automatonStatesNumber + j].isNull() && !tmp.isNull())
                {
                    if(!a[i*automatonStatesNumber + j].isEmpty())
                        a[i*automatonStatesNumber + j].append("+");
                    a[i*automatonStatesNumber + j].append(tmp);
                }else {
                    a[i*automatonStatesNumber + j] = tmp;
                }
                //a[i*automatonStatesNumber + j].append(a[i*automatonStatesNumber + n]);
                //a[i*automatonStatesNumber + j].append(a[n*automatonStatesNumber + j]);
            }
        }
    }

    regex = b[0];
}
