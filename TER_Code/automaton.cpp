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
    id = element.attribute("id","-1").toInt();
    name = element.attribute("name","");
    initial = element.attribute("initial","true")==QString("true");
    accepting = element.attribute("accepting","true")==QString("true");
    /*int id;
    QString name;
    bool initial;
    bool accepting;*/
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

    QDomElement childElement = node.firstChildElement("Events");
    /*QDomNodeList secondList = childElement.elementsByTagName("Event");
    for(int i = 0; i< secondList.count(); i++)
    {
        eventList.insert(i, Event(secondList.at(i)));
    }*/

    for(QDomElement element = childElement.firstChildElement("Event");!element.isNull();element = element.nextSiblingElement())
    {
        eventList.append(Event(element));
    }

    childElement = node.nextSiblingElement("States");
    /*secondList = childElement.elementsByTagName("State");
    for(int i = 0; i< secondList.count(); i++)
    {
        stateList.insert(i, State(secondList.at(i)));
    }*/

    childElement = node.nextSiblingElement("Transitions");
    /*secondList = childElement.elementsByTagName("Transition");
    for(int i = 0; i< secondList.count(); i++)
    {
        transitionList.insert(i, Transition(secondList.at(i)));
    }*/
}
