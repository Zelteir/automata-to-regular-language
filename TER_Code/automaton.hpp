#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <QList>
#include <QString>
#include <QDomNode>
#include <QException>
#include <QXmlStreamWriter>

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
        Event(int idEvent, QDomElement);
        Event(int idEvent, QString label,bool observable,bool controlable) : id(idEvent),label(label),observable(observable),controlable(controlable){}

        int getId() {return id;}
        QString getLabel() {return label;}
        bool getObservable() {return observable;}
        bool getControlable(){return controlable;}
        void setObservable(bool b) {observable = b;}
        void setControlable(bool b) {controlable = b;}
        void setLabel(QString);
        void toSupremica(QXmlStreamWriter *stream);
};

class State{
    private:
        int id;
        QString name;
        bool initial;
        bool accepting;

    public:
        State(){id = -1; name = ""; initial = false; accepting = false;}
        State(int idState, QDomElement);
        State(int idState, QString name,bool initial,bool accepting) : id(idState),name(name),initial(initial),accepting(accepting){}

        int getId() {return id;}
        QString getName() {return name;}
        bool getInitial() {return initial;}
        bool getAccepting() {return accepting;}
        void setName(QString);
        void setInitial(bool b) {initial = b;}
        void setAccepting(bool b) {accepting = b;}
        void toSupremica(QXmlStreamWriter *stream);
};

class Transition{
    private:
        int id;
        int source;
        int dest;
        int event;
    public:
        Transition() {id = -1; source = -1; dest = -1; event = -1;}
        Transition(int idTransition, QDomElement);
        Transition(int idTransition, int source,int dest,int event) : id(idTransition),source(source),dest(dest),event(event){}

        int getSource() {return source;}
        int getDest() {return dest;}
        int getEvent() {return event;}
        int getId() {return id;}
        void setSource(int s) {source = s;}
        void setDest(int d) {dest = d;}
        void setEvent(int e) {event = e;}
        bool operator==(const Transition& rhs);
        bool operator!=(const Transition& rhs){return !(*this==rhs);}
        void toSupremica(QXmlStreamWriter *stream);
};

class Automaton{
    private:
        int id;
        int idTransition = 0;
        int idEvent = 0;
        int idState = 0;
        QString name;
        QString type;
        QMap<int, Event> eventList;
        QMap<int, State> stateList;
        QMap<int, Transition> transitionList;
        QString generatedLanguage;

    public:
        void fromSupremica(int, QDomNode);
        Automaton(int, QString);
        Automaton() {id = 0; name = "";}
        int getId() {return id;}
        QString getName() {return name;}
        void setName(QString s) {name = s;}
        QString getType() {return type;}
        QMap<int, Event> *getEventList(){return &eventList;}
        QMap<int, State> *getStateList(){return &stateList;}
        QMap<int, Transition> *getTransitionList(){return &transitionList;}
        void setEventList(QMap<int, Event> events) {eventList = events;}
        State getState(int i){return stateList.value(i);}
        Event getEvent(int i){return eventList.value(i);}
        Transition getTransition(int i){return transitionList.value(i);}
        void toSupremica(QXmlStreamWriter *stream);
        QString getGeneratedLanguage() {return generatedLanguage;}
        void setGeneratedLanguage(QString language) {generatedLanguage = language;}
        int getIdTransition() {return idTransition;}
        int getIdState() {return idState;}
        int getIdEvent() {return idEvent;}
        void incrTransition() {idTransition++;}
        void incrEvent() {idEvent++;}
        void incrState() {idState++;}
        void toSedma(QXmlStreamWriter *stream);
        void fromSedma(QString);
};

#endif // AUTOMATON_HPP
