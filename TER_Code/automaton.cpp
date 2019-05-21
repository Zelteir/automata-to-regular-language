#include "automaton.hpp"
#include <QDebug>
#include <QFileInfo>

void Attribute::setName(QString arg)
{
    if(arg.isEmpty())throw SetterException("Name cannot be empty.");
    this->name = arg;
}

/*
 * Write the Event information inside the stream for Supremica XML
*/
void Event::toSupremica(QXmlStreamWriter *stream)
{
    //<Event id="3" label="f" controllable="false" observable="false"/>
    stream->writeStartElement("Event");
    stream->writeAttribute("id", QString::number(this->id));
    stream->writeAttribute("label", this->name);
    if(!this->controlable)
        stream->writeAttribute("controllable", "false");
    if(!this->observable)
        stream->writeAttribute("observable", "false");
    stream->writeEndElement();
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

    QDomElement element;
    QDomElement childElement = node.firstChildElement("Events");
    for(element = childElement.firstChildElement("Event");!element.isNull();element = element.nextSiblingElement())
    {
        eventList.insert(idEvent, Event(idEvent,
                                        element.attribute("label",""),
                                        element.attribute("observable","true")==QString("true"),
                                        element.attribute("controlable","true")==QString("true")));
        idEvent++;
    }
    childElement = node.firstChildElement("States");
    for(element = childElement.firstChildElement("State");!element.isNull();element = element.nextSiblingElement())
    {
        stateList.insert(idState, State(idState,
                                        element.attribute("name",""),
                                        element.attribute("initial","false")==QString("true"),
                                        element.attribute("accepting","false")==QString("true")));
        idState++;
    }
    childElement = node.firstChildElement("Transitions");
    for(element = childElement.firstChildElement("Transition");!element.isNull();element = element.nextSiblingElement())
    {
        transitionList.insert(idTransition, Transition(idTransition,
                                                       element.attribute("source","-1").toInt(),
                                                       element.attribute("dest","-1").toInt(),
                                                       element.attribute("event","-1").toInt()));
        idTransition++;
    }
}

Automaton::Automaton(int id,QString name) :
    id(id),
    name(name)
{}

/*
 * Export function to supremica file. Calling 'toSupremica' functions of every event, state and transition
 * writing inside the stream argument passed to the functions.
*/
void Automaton::toSupremica(QXmlStreamWriter *stream)
{
    stream->writeStartElement("Automaton");
    stream->writeAttribute("name", this->name);
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
        line += eventList[t.getEvent()].getName();
        line += "} black";
        *stream << line << endl;
    }
}

/*
 * Import informations from a QDomNode extracted from DESUMA XML file and create elements according to these informations
 * Call default constructor for Event, State and Transition
 * need workaround with vecState and vecEvent since DESUMA file use name rather than ID
*/
void Automaton::fromDesuma(int id, QDomNode node)
{
    QMap<QString, int> vecState, vecEvent;
    this->id = id;

    QDomElement element;
    for(element = node.firstChildElement("event");!element.isNull();element = element.nextSiblingElement("event"))
    {
        vecEvent.insert(element.attribute("name"), idEvent);
        eventList.insert(idEvent, Event(idEvent,
                                        element.attribute("name",""),
                                        element.attribute("observable","true")==QString("true"),
                                        element.attribute("controlable","true")==QString("true")));
        idEvent++;
    }
    for(element = node.firstChildElement("state");!element.isNull();element = element.nextSiblingElement("state"))
    {
        vecState.insert(element.attribute("name"), idState);
        stateList.insert(idState, State(idState,
                                        element.attribute("name",""),
                                        element.attribute("initial","false")==QString("true"),
                                        element.attribute("marked","false")==QString("true")));
        idState++;
    }
    for(element = node.firstChildElement("transition");!element.isNull();element = element.nextSiblingElement("transition"))
    {
        transitionList.insert(idTransition, Transition(idTransition,
                                                       vecState.value(element.attribute("from","")),
                                                       vecState.value(element.attribute("to","")),
                                                       vecEvent.value(element.attribute("name",""))));
        idTransition++;
    }
}

/*
 * Export function to DESUMA file.
 * DESUMA files use name rather then ID so access to lists are necessary.
*/
void Automaton::toDesuma(QXmlStreamWriter *stream)
{
    stream->writeStartElement("XmlAutomaton");
    for(State s : stateList)
    {
        stream->writeStartElement("state");
        stream->writeAttribute("name", s.getName());
        stream->writeAttribute("initial",(s.getInitial())?"true":"false");
        stream->writeAttribute("marked",(s.getAccepting())?"true":"false");
        stream->writeEndElement();
    }
    for(Transition t : transitionList)
    {
        stream->writeStartElement("transition");
        stream->writeAttribute("from", stateList[t.getSource()].getName());
        stream->writeAttribute("to", stateList[t.getDest()].getName());
        stream->writeAttribute("name", eventList[t.getEvent()].getName());
        stream->writeEndElement();
    }
    for(Event e : eventList)
    {
        stream->writeStartElement("event");
        stream->writeAttribute("name", e.getName());
        stream->writeAttribute("controllable",(e.getControlable())?"true":"false");
        stream->writeAttribute("observable",(e.getObservable())?"true":"false");
        stream->writeEndElement();
    }
    stream->writeEndElement();
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
                idState++;
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
                        idEvent++;
                    }
                    transitionList.insert(idTransition, Transition(idTransition,vecState.value(line[1]), vecState.value(line[2]), vecEvent.value(line[3])));
                }
                else
                {
                    transitionList.insert(idTransition, Transition(idTransition,vecState.value(line[1]), vecState.value(line[2]), 0));
                }
                idTransition++;
            }
        }
    }
    if(vecEvent.size() == 0 && transitionList.size() > 0)   //if no event created
    {
        eventList.insert(idEvent,Event(idEvent, "a", true, true));
        idEvent++;
    }
}

QString SetterException::getMsg()
{
    return msg;
}
