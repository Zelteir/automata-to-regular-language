#include "automaton.hpp"

Event::Event(QDomElement element)
{
    /*int id;
    QString label;
    bool observable;
    bool controlable;*/

    id=element.attribute("id","-1").toInt();
    label=element.attribute("label","");
    observable=element.attribute("observable","true")==QString("true)");
    controlable=element.attribute("controlable","true")==QString("true)");
}

State::State(QDomElement node)
{
    /*int id;
    QString name;
    bool initial;
    bool accepting;*/
}

Transition::Transition(QDomElement node)
{
   /* int source;
    int dest;
    int event;*/
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
