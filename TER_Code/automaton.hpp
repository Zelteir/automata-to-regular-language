#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <QList>
#include <QString>

class Event{
    private:
        int id;
        QString label;
        bool observable;
        bool controlable;
   public:
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

};

#endif // AUTOMATON_HPP
