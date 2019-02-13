#include "automaton.hpp"

Event::Event(QDomElement element)
{
    /*int id;
    QString label;
    bool observable;
    bool controlable;*/

    id = element.attribute("id","-1").toInt();
    label = element.attribute("label","");
    observable = element.attribute("observable","true")==QString("true");
    controlable = element.attribute("controlable","true")==QString("true");
}

State::State(QDomElement element)
{
    /*int id;
    QString name;
    bool initial;
    bool accepting;*/

    id = element.attribute("id","-1").toInt();
    name = element.attribute("name","");
    initial = element.attribute("initial","true")==QString("true");
    accepting = element.attribute("accepting","true")==QString("true");
}

Transition::Transition(QDomElement element)
{
   /* int source;
    int dest;
    int event;*/

    source = element.attribute("source","-1").toInt();
    dest = element.attribute("dest","-1").toInt();
    event = element.attribute("event","-1").toInt();
}

Automaton::Automaton(QDomNode node)
{
    name = node.attributes().namedItem("name").nodeValue();
    type = node.attributes().namedItem("type").nodeValue();

    QDomElement element;
    QDomElement childElement = node.firstChildElement("Events");
    for(element = childElement.firstChildElement("Event");!element.isNull();element = element.nextSiblingElement())
    {
        eventList.append(Event(element));
    }

    childElement = node.nextSiblingElement("States");
    for(element = childElement.firstChildElement("State");!element.isNull();element = element.nextSiblingElement())
    {
        stateList.append(State(element));
    }

    childElement = node.nextSiblingElement("Transitions");
    for(element = childElement.firstChildElement("Transition");!element.isNull();element = element.nextSiblingElement())
    {
        transitionList.append(Transition(element));
    }
}
