#include "automaton.hpp"

Event::Event(QDomNode node)
{
    /*int id;
    QString label;
    bool observable;
    bool controlable;

    id = node.attributes().namedItem("name").nodeValue();
    type = node.attributes().namedItem("type").nodeValue();
    name = node.attributes().namedItem("name").nodeValue();
    type = node.attributes().namedItem("type").nodeValue();*/
}

State::State(QDomNode node)
{
    /*int id;
    QString name;
    bool initial;
    bool accepting;*/
}

Transition::Transition(QDomNode node)
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
    QDomNodeList secondList = childElement.elementsByTagName("Event");
    for(int i = 0; i< secondList.count(); i++)
    {
        eventList.insert(i, Event(secondList.at(i)));
    }

    childElement = node.nextSiblingElement("States");
    secondList = childElement.elementsByTagName("State");
    for(int i = 0; i< secondList.count(); i++)
    {
        stateList.insert(i, State(secondList.at(i)));
    }

    childElement = node.nextSiblingElement("Transitions");
    secondList = childElement.elementsByTagName("Transition");
    for(int i = 0; i< secondList.count(); i++)
    {
        transitionList.insert(i, Transition(secondList.at(i)));
    }
}
