#include "automaton.hpp"
#include <QDebug>

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

void Event::setLabel(QString l)
{
    if(l.isEmpty())throw SetterException("Name cannot be empty.");
    this->label = l;
}

int State::idState = 0;

State::State(QDomElement element)
{
    /*int id;
    QString name;
    bool initial;
    bool accepting;*/

    id = element.attribute("id","-1").toInt();
    name = element.attribute("name","");
    initial = element.attribute("initial","false")==QString("true");
    accepting = element.attribute("accepting","false")==QString("true");
    idState++;
}

void State::setName(QString n)
{
    if(n.isEmpty())throw SetterException("Name cannot be empty.");
    this->name = n;
}

Transition::Transition(QDomElement element) : id(idTransition++)
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
    idEvent = eventList.size();

    childElement = node.firstChildElement("States");
    for(element = childElement.firstChildElement("State");!element.isNull();element = element.nextSiblingElement())
    {
        stateList.append(State(element));
    }
    childElement = node.firstChildElement("Transitions");
    for(element = childElement.firstChildElement("Transition");!element.isNull();element = element.nextSiblingElement())
    {
        transitionList.append(Transition(element));
    }
}

QString SetterException::getMsg()
{
    return msg;
}

/*TO DO
 * QStringList get_name_list()
 * for events & states
*/
