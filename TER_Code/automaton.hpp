#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <QList>
#include <QString>
#include <QDomNode>

class Event{
    private:
        int id;
        QString label;
        bool observable;
        bool controlable;
   public:
        Event(QDomNode);

        int getId() {return id;}
        QString getLabel() {return label;}
        bool getObservable() {return observable;}
        bool getControlable(){return controlable;}


};

class State{
    private:
        int id;
        QString name;
        bool initial;
        bool accepting;

    public:
        State(QDomNode);

        int getId() {return id;}
        QString getName() {return name;}
        bool getInitial() {return initial;}
        bool getAccepting() {return accepting;}
};

class Transition{
    private:
        int source;
        int dest;
        int event;
    public:
        Transition(QDomNode);

        int getSource() {return source;}
        int getDest() {return dest;}
        int getEvent() {return event;}
};

class Automaton{
    private:
        QString name;
        QString type;
        QList<Event> eventList;
        QList<State> stateList;
        QList<Transition> transitionList;

    public:
        Automaton(QDomNode);
        QString getName() {return name;}
        QString getType() {return type;}
        QList<Event> getEventList(){return eventList;}
        QList<State> getStateList(){return stateList;}
        QList<Transition> getTransitionList(){return transitionList;}
};

#endif // AUTOMATON_HPP
