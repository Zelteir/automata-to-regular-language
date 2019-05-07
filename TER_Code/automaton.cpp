#include "automaton.hpp"
#include <QDebug>
#include <QFileInfo>

/*
 * Event constructor for Supremica input
*/
Event::Event(int idEvent, QDomElement element)
{
    /*int id;
    QString label;
    bool observable;
    bool controlable;*/

    id = idEvent;
    label = element.attribute("label","");
    observable = element.attribute("observable","true")==QString("true");
    controlable = element.attribute("controlable","true")==QString("true");

}

void Event::setLabel(QString l)
{
    if(l.isEmpty())throw SetterException("Name cannot be empty.");
    this->label = l;
}

/*
 * Write the Event information inside the stream for Supremica XML
*/
void Event::toSupremica(QXmlStreamWriter *stream)
{
    //<Event id="3" label="f" controllable="false" observable="false"/>
    stream->writeStartElement("Event");
    stream->writeAttribute("id", QString::number(this->id));
    stream->writeAttribute("label", this->label);
    if(!this->controlable)
        stream->writeAttribute("controllable", "false");
    if(!this->observable)
        stream->writeAttribute("observable", "false");
    stream->writeEndElement();
}

/*
 * State constructor for Supremica input
*/
State::State(int idState, QDomElement element)
{
    /*int id;
    QString name;
    bool initial;
    bool accepting;*/

    id = idState;
    name = element.attribute("name","");
    initial = element.attribute("initial","false")==QString("true");
    accepting = element.attribute("accepting","false")==QString("true");

}

void State::setName(QString n)
{
    if(n.isEmpty())throw SetterException("Name cannot be empty.");
    this->name = n;
}

/*
 * Write the State information inside the stream for Supremica XML
*/
void State::toSupremica(QXmlStreamWriter *stream)
{
    stream->writeStartElement("State");
    stream->writeAttribute("id", QString::number(this->id));
    stream->writeAttribute("name", this->name);
    if(this->initial)
        stream->writeAttribute("initial", "true");
    if(this->accepting)
        stream->writeAttribute("accepting", "true");
    stream->writeEndElement();
}

/*
 * Write the Transition information inside the stream for Supremica XML
*/
Transition::Transition(int idTransition, QDomElement element)
{
    /* int source;
    int dest;
    int event;*/

    id = idTransition;
    source = element.attribute("source","-1").toInt();
    dest = element.attribute("dest","-1").toInt();
    event = element.attribute("event","-1").toInt();
}

bool Transition::operator==(const Transition &rhs)
{
    return (this->source == rhs.source && this->dest == rhs.dest && this->event == rhs.event)? true : false;
}

/*
 * Write the Transition information inside the stream for Supremica XML
*/
void Transition::toSupremica(QXmlStreamWriter *stream)
{
    stream->writeStartElement("Transition");
    stream->writeAttribute("source", QString::number(this->source));
    stream->writeAttribute("dest", QString::number(this->dest));
    stream->writeAttribute("event", QString::number(this->event));
    stream->writeEndElement();
}

/*
 * Import informations from a QDomNode extracted from Supremica XML file and create elements according to these informations
 * Call constructors using QDomElement from Event, State and Transition
*/
void Automaton::fromSupremica(int id, QDomNode node)
{
    this->id = id;
    name = node.attributes().namedItem("name").nodeValue();
    type = node.attributes().namedItem("type").nodeValue();

    QDomElement element;
    QDomElement childElement = node.firstChildElement("Events");
    for(element = childElement.firstChildElement("Event");!element.isNull();element = element.nextSiblingElement())
    {
        eventList.insert(idEvent, Event(idEvent, element));
        idEvent++;
    }
    childElement = node.firstChildElement("States");
    for(element = childElement.firstChildElement("State");!element.isNull();element = element.nextSiblingElement())
    {
        stateList.insert(idState, State(idState, element));
        idState++;
    }
    childElement = node.firstChildElement("Transitions");
    for(element = childElement.firstChildElement("Transition");!element.isNull();element = element.nextSiblingElement())
    {
        transitionList.insert(idTransition, Transition(idTransition, element));
        idTransition++;
    }
}

Automaton::Automaton(int id,QString name) :
    id(id),
    name(name)
{
    type = QString("Specification");    //for Supremica
}

/*
 * Export function to supremica file. Calling 'toSupremica' functions of every event, state and transition
 * writing inside the stream argument passed to the functions.
*/
void Automaton::toSupremica(QXmlStreamWriter *stream)
{
    stream->writeStartElement("Automaton");
    stream->writeAttribute("name", this->name);
    stream->writeAttribute("type", this->type);
    stream->writeStartElement("Events");
    for(Event e : eventList)
        e.toSupremica(stream);
    stream->writeEndElement();
    stream->writeStartElement("States");
    for(State s : stateList)
        s.toSupremica(stream);
    stream->writeEndElement();
    stream->writeStartElement("Transitions");
    for(Transition t : transitionList)
        t.toSupremica(stream);
    stream->writeEndElement();
    stream->writeEndElement();
}

/*
 * Export function to SEDMA file. SEDMA files do not separated event and transitions so they have to be written together.
 * '0's and 'black's are default values necessary for compatibility
*/
void Automaton::toSedma(QTextStream *stream)
{
    QString line;
    for(State s : stateList)
    {
        line = "s 0 0 ";    //state coordX coordY
        line += s.getName();
        line += " {} black ";   //state_attribute   state_color
        if(s.getInitial() && s.getAccepting())
            line += "-iM";
        else if(s.getInitial())
            line+= "-i";
        else if(s.getAccepting())
            line += "-M";
        *stream << line << endl;
    }
    for(Transition t : transitionList)
    {
        line = "e ";
        line += stateList[t.getSource()].getName();
        line += " ";
        line += stateList[t.getDest()].getName();
        line += " {";
        line += eventList[t.getEvent()].getLabel();
        line += "} black";
        *stream << line << endl;
    }
}

/*
 * Import function from a SEDMA file. The read file is the 'arg' QString
 * SEDMA's transition doesn't require an event, if one doesn't have an event, it is defaulted to event zero
 * if no event is detected a default one is created.
*/
void Automaton::fromSedma(int id, QString name, QString arg)
{
    QFileInfo fInfo(name);
    QMap<QString, int> vecState, vecEvent;

    this->name = fInfo.baseName();
    this->id = id;

    QStringList lines = arg.split(QRegExp("(\\n\\r)|(\\n)|(\\r)"));
    QStringList line;
    for(QString s : lines)
    {
        if(!s.isEmpty())
        {
            line = s.split(' ');
            if(line[0] == "s")  //state
            {
                vecState.insert(line[3], idState);  //keep name and ID for transition creation
                stateList.insert(idState, State(idState,line[3],(line[6] == "-i" || line[6] == "-iM"),(line[6] == "-M" || line[6] == "-iM")));
                incrState();
            }
            else
            {
                line[3].remove('{');
                line[3].remove('}');
                if(!line[3].isEmpty())
                {
                    if(!vecEvent.contains(line[3]))
                    {
                        vecEvent.insert(line[3], idEvent);  //keep name and ID for transition creation and size verification
                        eventList.insert(idEvent, Event(idEvent, line[3], true, true));
                        incrEvent();
                    }
                    transitionList.insert(idTransition, Transition(idTransition,vecState.value(line[1]), vecState.value(line[2]), vecEvent.value(line[3])));
                }
                else
                {
                    transitionList.insert(idTransition, Transition(idTransition,vecState.value(line[1]), vecState.value(line[2]), 0));
                }
                incrTransition();
            }
        }
    }
    if(vecEvent.size() == 0 && transitionList.size() > 0)   //if no event created
    {
        eventList.insert(idEvent,Event(idEvent, "a", true, true));
        incrEvent();
    }
}

QString SetterException::getMsg()
{
    return msg;
}
