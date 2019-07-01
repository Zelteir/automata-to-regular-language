#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <QList>
#include <QString>
#include <QDomNode>
#include <QException>
#include <QXmlStreamWriter>

enum attribute_type{ATTRIBUTE, EVENT, STATE, TRANSITION, AUTOMATON};

class Attribute{
protected:
    int id;
    QString name;
    enum attribute_type type = ATTRIBUTE;

    Attribute(){id = -1;name = ""; type = ATTRIBUTE;}
    Attribute(int id, QString name, enum attribute_type type) : id(id), name(name), type(type){}

public :
    Attribute(const Attribute &param):id(param.id),name(param.name), type(param.type){}
    int getId() {return id;}
    QString getName() {return name;}
    enum attribute_type getType() {return type;}
    void setName(QString s);
};

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

class Event : public Attribute{
private:
    bool observable; //true==observable
    bool controlable; //true==controlable
public:
    Event(){id = -1; name = ""; observable = true; controlable = true;}
    Event(int idEvent, QString name,bool observable,bool controlable) : Attribute(idEvent, name, EVENT),observable(observable),controlable(controlable){}
    Event(const Event &param) : Attribute(param), observable(param.observable), controlable(param.controlable){}

    bool getObservable() {return observable;}
    bool getControlable(){return controlable;}
    void setObservable(bool b) {observable = b;}
    void setControlable(bool b) {controlable = b;}
    void toSupremica(QXmlStreamWriter *stream);
};

class State : public Attribute{
    private:
        bool initial;
        bool accepting;

    public:
        State(){id = -1; name = ""; initial = false; accepting = false;}
        State(int idState, QString name, bool initial,bool accepting) : Attribute(idState, name, STATE),initial(initial),accepting(accepting){}
        State(const State &param) : Attribute(param), initial(param.initial), accepting(param.accepting){}

        bool getInitial() {return initial;}
        bool getAccepting() {return accepting;}
        void setInitial(bool b) {initial = b;}
        void setAccepting(bool b) {accepting = b;}
        void toSupremica(QXmlStreamWriter *stream);
};

class Transition: public Attribute{
    private:
        int source;
        int dest;
        int event;
    public:
        Transition() {id = -1; source = -1; dest = -1; event = -1;}
        Transition(int idTransition, int source,int dest,int event) : Attribute(idTransition,"", TRANSITION),source(source),dest(dest),event(event){}
        Transition(const Transition &param) : Attribute(param), source(param.source), dest(param.dest), event(param.event){}

        int getSource() {return source;}
        int getDest() {return dest;}
        int getEvent() {return event;}
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
        QMap<int, Event> eventList;
        QMap<int, State> stateList;
        QMap<int, Transition> transitionList;
        QString generatedLanguage;

    public:

        Automaton(int, QString);
        Automaton() {id = 0; name = "";}
        int getId() {return id;}
        QString getName() {return name;}
        void setName(QString s) {name = s;}
        QMap<int, Event> *getEventList(){return &eventList;}
        QMap<int, State> *getStateList(){return &stateList;}
        QMap<int, Transition> *getTransitionList(){return &transitionList;}
        void setEventList(QMap<int, Event> events) {eventList = events;}
        State getState(int i){return stateList.value(i);}
        Event getEvent(int i){return eventList.value(i);}
        Transition getTransition(int i){return transitionList.value(i);}
        QString getGeneratedLanguage() {return generatedLanguage;}
        void setGeneratedLanguage(QString language) {generatedLanguage = language;}
        int getIdTransition() {return idTransition;}
        int getIdState() {return idState;}
        int getIdEvent() {return idEvent;}
        void incrTransition() {idTransition++;}
        void incrEvent() {idEvent++;}
        void incrState() {idState++;}
        void fromSupremica(int, QDomNode);
        void toSupremica(QXmlStreamWriter *stream);
        void fromSedma(int, QString, QString);
        void toSedma(QTextStream *stream);
        void fromDesuma(int, QDomNode);
        void toDesuma(QXmlStreamWriter *stream);
};

#endif // AUTOMATON_HPP
