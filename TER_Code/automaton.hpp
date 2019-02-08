#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <QString>

class Event{
    public:
        int id;
        QString label;
        bool observable;
        bool controlable;
};

class State{
    public:
        int id;
        QString name;
        bool initial;
        bool accepting;
};

class Transition{
    public:
        int source;
        int dest;
        int event;
};

class Automaton{
    public:
        QString name;
        QString type;
        Event* eventList;
        State* stateList;
        Transition* transitionList;
};

#endif // AUTOMATON_HPP
