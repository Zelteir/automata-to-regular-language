#include "automaton.hpp"
#include <QDebug>

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
    /*if(this->source == rhs.source && this->dest == rhs.dest && this->event == rhs.event)
        return true;
    return false;*/
    return (this->source == rhs.source && this->dest == rhs.dest && this->event == rhs.event)? true : false;
}

void Transition::toSupremica(QXmlStreamWriter *stream)
{
    stream->writeStartElement("Transition");
    stream->writeAttribute("source", QString::number(this->source));
    stream->writeAttribute("dest", QString::number(this->dest));
    stream->writeAttribute("event", QString::number(this->event));
    stream->writeEndElement();
}

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
    type = QString("Specification");
}

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

/*TO DO*/
void Automaton::toSedma(QXmlStreamWriter *stream)
{

}

/*TO DO*/
void Automaton::fromSedma(QString)
{

}

QString SetterException::getMsg()
{
    return msg;
}
