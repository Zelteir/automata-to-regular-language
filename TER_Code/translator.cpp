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

void Translator::brzozowskiMethod(Automaton automaton) //Penser aux options desactiver les incontrolables, les invisibles
{
    int automatonStatesNumber = automaton.getStateList().size(), i, j;
    QVector<QString> a(automatonStatesNumber * automatonStatesNumber, "");
    QVector<QString> b(automatonStatesNumber, ""); //A modifier pour une liste/vector
    //Initialisation
    for (i = 0; i < automatonStatesNumber; i++)
    {
        if(automaton.getState(i).getAccepting())
            b[i] = "e";
    }

    for(i = 0; i < automaton.getTransitionList().size(); i++)
    {
        Transition t = automaton.getTransition(i);
        a[t.getSource()*automatonStatesNumber + t.getDest()] = automaton.getEvent(t.getEvent()).getLabel();
        //TO DO qdebug test
    }

    //Solving
    for(int n = automatonStatesNumber-1; n > -1; n--)
    {
        b[n].prepend(star(a[n*automatonStatesNumber + n]));
        for(j = 0; j < n; j++)
          a[n*automatonStatesNumber + j].prepend(star(a[n*automatonStatesNumber + n]));
        for(i = 0; i < n; i++)
        {
            b[i].append(a[i*automatonStatesNumber + n]);
            b[i].append(b[n]);
            for(j = 0; j < n; j++)
            {
                a[i*automatonStatesNumber + j].append(a[i*automatonStatesNumber + n]);
                a[i*automatonStatesNumber + j].append(a[n*automatonStatesNumber + j]);
            }
        }
    }

    regex = b[0];
}

