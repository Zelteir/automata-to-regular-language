#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <QList>
#include <QString>
#include <QDomNode>
#include <QException>

static int idEvent = 0;
static int idState = 0;
static int idTransition = 0;

class SetterException : public QException
{
    public:
        SetterException(QString const& msg):msg(msg){}
        void raise() const override { throw *this; }
        SetterException *clone() const override { return new SetterException(*this); }
        virtual QString getMsg();
    private:
        QString msg;
};

class Event{
    private:
        int id;
        QString label;
        bool observable; //true==observable
        bool controlable; //true==controlable
   public:
        Event(){id = -1; label = ""; observable = true; controlable = true;}
        Event(QDomElement);
        Event(QString label,bool observable,bool controlable) : id(idEvent++),label(label),observable(observable),controlable(controlable){}

        int getId() {return id;}
        QString getLabel() {return label;}
        bool getObservable() {return observable;}
        bool getControlable(){return controlable;}
        void setObservable(bool b) {observable = b;}
        void setControlable(bool b) {controlable = b;}
        void setLabel(QString);
};

class State{
    private:
        int id;
        QString name;
        bool initial;
        bool accepting;

    public:
        State(){id = -1; name = ""; initial = false; accepting = false;}
        State(QDomElement);
        State(QString name,bool initial,bool accepting) : id(idState++),name(name),initial(initial),accepting(accepting){}

        int getId() {return id;}
        QString getName() {return name;}
        bool getInitial() {return initial;}
        bool getAccepting() {return accepting;}
        void setName(QString);
        void setInitial(bool b) {initial = b;}
        void setAccepting(bool b) {accepting = b;}
};

class Transition{
    private:
        int id;
        int source;
        int dest;
        int event;
    public:
        Transition() {id = -1; source = -1; dest = -1; event = -1;}
        Transition(QDomElement);
        Transition(int source,int dest,int event) : id(idTransition++),source(source),dest(dest),event(event){}

        int getSource() {return source;}
        int getDest() {return dest;}
        int getEvent() {return event;}
        int getId() {return id;}
        void setSource(int s) {source = s;}
        void setDest(int d) {dest = d;}
        void setEvent(int e) {event = e;}
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
        QList<Event> *getEventList(){return &eventList;}
        QList<State> *getStateList(){return &stateList;}
        QList<Transition> *getTransitionList(){return &transitionList;}
        State getState(int i){return stateList[i];}
        Event getEvent(int i){return eventList[i];}
        Transition getTransition(int i){return transitionList[i];}
};

#endif // AUTOMATON_HPP
