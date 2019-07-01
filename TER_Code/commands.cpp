#include "commands.hpp"


EditCommand::EditCommand(const EditCommand &param)
{
    switch(param.type)
    {
    case STATE:
    {
        type = STATE;
        attrState = param.attrState;
        oldState = param.oldState;
        break;
    }
    case EVENT:
    {
        type = EVENT;
        attrEvent = param.attrEvent;
        oldEvent = param.oldEvent;
        break;
    }
    case TRANSITION:
    {
        type = TRANSITION;
        attrTransition = param.attrTransition;
        oldTransition = param.oldTransition;
        break;
    }
    case AUTOMATON:
    {
        type = AUTOMATON;
        attrAutomaton = param.attrAutomaton;
        oldAutomaton = param.oldAutomaton;
        break;
    }
    default:
    {
        break;
    }
    }

    setText(param.text());
    idAutomaton = param.idAutomaton;
}

EditCommand::EditCommand(State oldAttr, State attr, MainWindow *parent, int automaton):
    attrState(attr),
    oldState(oldAttr),
    idAutomaton(automaton),
    parent(parent)
{
    type = STATE;
    /*setText(QObject::tr("Edit %1")
            .arg(createCommandString(&oldAttr)));*/
    setText(QObject::tr(""));
}

EditCommand::EditCommand(Event oldAttr, Event attr, MainWindow *parent, int automaton):
    attrEvent(attr),
    oldEvent(oldAttr),
    idAutomaton(automaton),
    parent(parent)
{
    type = EVENT;
    /*setText(QObject::tr("Edit %1")
            .arg(createCommandString(&oldAttr)));*/
    setText(QObject::tr(""));
}

EditCommand::EditCommand(Transition oldAttr, Transition attr, MainWindow *parent, int automaton):
    attrTransition(attr),
    oldTransition(oldAttr),
    idAutomaton(automaton),
    parent(parent)
{
    type = TRANSITION;
    /*setText(QObject::tr("Edit %1")
            .arg(createCommandString(&oldAttr)));*/
    setText(QObject::tr(""));
}

EditCommand::EditCommand(Automaton oldAttr, Automaton attr, MainWindow *parent):
    attrAutomaton(attr),
    oldAutomaton(oldAttr),
    idAutomaton(attr.getId()),
    parent(parent)
{
    type = AUTOMATON;
    setText(QObject::tr(""));
}

void EditCommand::undo()
{
    switch(type)
    {
    case STATE:
    {
        emit(undo_editState(oldState, idAutomaton));
        break;
    }
    case EVENT:
    {
        emit(undo_editEvent(oldEvent, idAutomaton));
        break;
    }
    case TRANSITION:
    {
        emit(undo_editTransition(oldTransition, idAutomaton));
        break;
    }
    case AUTOMATON:
    {
        emit(undo_editAutomaton(oldAutomaton));
        break;
    }
    default:
    {
        break;
    }
    }
}

void EditCommand::redo()
{
    switch(type)
    {
    case STATE:
    {
        emit(redo_editState(attrState, idAutomaton));
        break;
    }
    case EVENT:
    {
        emit(redo_editEvent(attrEvent, idAutomaton));
        break;
    }
    case TRANSITION:
    {
        emit(redo_editTransition(attrTransition, idAutomaton));
        break;
    }
    case AUTOMATON:
    {
        emit(redo_editAutomaton(attrAutomaton));
        break;
    }
    default:
    {
        break;
    }
    }
}

DeleteCommand::DeleteCommand(const DeleteCommand &param): parent(param.parent)
{
    switch(param.type)
    {
    case STATE:
    {
        type = STATE;
        attrState = param.attrState;
        break;
    }
    case EVENT:
    {
        type = EVENT;
        attrEvent = param.attrEvent;
        break;
    }
    case TRANSITION:
    {
        type = TRANSITION;
        attrTransition = param.attrTransition;
        break;
    }
    case AUTOMATON:
    {
        type = AUTOMATON;
        attrAutomaton = param.attrAutomaton;
        break;
    }
    default:
    {
        break;
    }
    }

    setText(param.text());
    idAutomaton = param.idAutomaton;
}

DeleteCommand::DeleteCommand(QList<State> attr, MainWindow *parent, int automaton):
    attrState(attr),
    idAutomaton(automaton),
    parent(parent)
{
    type = STATE;
    if(attr.length()>1)
        for(int i = 0; i < attr.length(); i++)
            //setText(QObject::tr("Delete multiple states"));
            setText(QObject::tr(""));
    else
        /*setText(QObject::tr("Delete %1")
                .arg(createCommandString(&(attr[0]))));*/
                setText(QObject::tr("Delete"));
}

DeleteCommand::DeleteCommand(QList<Event> attr, MainWindow *parent, int automaton):
    attrEvent(attr),
    idAutomaton(automaton),
    parent(parent)
{
    type = EVENT;
    if(attr.length() > 1)
    for(int i = 0; i < attr.length(); i++)
        //setText(QObject::tr("Delete multiple events"));
        setText(QObject::tr(""));
    else
        /*setText(QObject::tr("Delete %1")
                .arg(createCommandString(&(attr[0]))));*/
        setText(QObject::tr("Delete"));

}

DeleteCommand::DeleteCommand(QList<Transition> attr, MainWindow *parent, int automaton):
    attrTransition(attr),
    idAutomaton(automaton),
    parent(parent)
{
    type = TRANSITION;
    if(attr.length() > 1)
    for(int i = 0; i < attr.length(); i++)
        //setText(QObject::tr("Delete multiple transitions"));
        setText(QObject::tr(""));
    else
        /*setText(QObject::tr("Delete %1")
                .arg(createCommandString(&(attr[0]))));*/
        setText(QObject::tr("Delete"));
}

DeleteCommand::DeleteCommand(QList<Automaton> attr, MainWindow *parent):
    attrAutomaton(attr),
    parent(parent)
{
    type = AUTOMATON;
}

void DeleteCommand::undo()
{
    switch(type)
    {
    case STATE:
    {
        for(State s : attrState)
            emit(undo_deleteState(s, idAutomaton));
        break;
    }
    case EVENT:
    {
        for(Event e : attrEvent)
            emit(undo_deleteEvent(e, idAutomaton));
        break;
    }
    case TRANSITION:
    {
        for(Transition t : attrTransition)
            emit(undo_deleteTransition(t, idAutomaton));
        break;
    }
    case AUTOMATON:
    {
        for(Automaton a : attrAutomaton)
            emit(undo_deleteAutomaton(a));
        break;
    }
    default:
    {
        break;
    }
    }
}

void DeleteCommand::redo()
{
    QList<int> deleteList;
    switch(type)
    {
    case STATE:
    {
        for(State s : attrState)
            deleteList.append(s.getId());
        emit(redo_deleteState(deleteList, idAutomaton));
        break;
    }
    case EVENT:
    {
        for(Event e : attrEvent)
            deleteList.append(e.getId());
        emit(redo_deleteEvent(deleteList, idAutomaton));
        break;
    }
    case TRANSITION:
    {
        for(Transition t : attrTransition)
            deleteList.append(t.getId());
        emit(redo_deleteTransition(deleteList, idAutomaton));
        break;
    }
    case AUTOMATON:
    {
        for(Automaton a : attrAutomaton)
            deleteList.append(a.getId());
        emit(redo_deleteAutomaton(deleteList));
        break;
    }
    default:
    {
        break;
    }
    }
}

AddCommand::AddCommand(const AddCommand &param): parent(param.parent)
{
    switch(param.type)
    {
    case STATE:
    {
        type = STATE;
        attrState = param.attrState;
        break;
    }
    case EVENT:
    {
        type = EVENT;
        attrEvent = param.attrEvent;
        break;
    }
    case TRANSITION:
    {
        type = TRANSITION;
        attrTransition = param.attrTransition;
        break;
    }
    case AUTOMATON:
    {
        type = AUTOMATON;
        attrAutomaton = param.attrAutomaton;
        break;
    }
    default:
    {
        break;
    }
    }
    setText(param.text());
    idAutomaton = param.idAutomaton;
}

AddCommand::AddCommand(State attr, MainWindow *parent, int automaton):
    attrState(attr),
    idAutomaton(automaton),
    parent(parent)
{
    type = STATE;
    /*setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));*/
    setText(QObject::tr("Add"));
}

AddCommand::AddCommand(Event attr, MainWindow *parent, int automaton):
    attrEvent(attr),
    idAutomaton(automaton),
    parent(parent)
{
    type = EVENT;
    /*setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));*/
    setText(QObject::tr("Add"));
}

AddCommand::AddCommand(Transition attr, MainWindow *parent, int automaton):
    attrTransition(attr),
    idAutomaton(automaton),
    parent(parent)
{
    type = TRANSITION;
    /*setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));*/
    setText(QObject::tr("Add"));
}

AddCommand::AddCommand(Automaton attr, MainWindow *parent):
    attrAutomaton(attr),
    parent(parent)
{
    type = AUTOMATON;
    /*setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));*/
    setText(QObject::tr("Add"));
}

void AddCommand::undo()
{
    switch(type)
    {
    case STATE:
    {
        emit(undo_addState(QList<int>({attrState.getId()}), idAutomaton));
        break;
    }
    case EVENT:
    {
        emit(undo_addEvent(QList<int>({attrEvent.getId()}), idAutomaton));
        break;
    }
    case TRANSITION:
    {
        emit(undo_addTransition(QList<int>({attrTransition.getId()}), idAutomaton));
        break;
    }
    case AUTOMATON:
    {
        emit(undo_addAutomaton(QList<int>({attrAutomaton.getId()})));
        break;
    }
    default:
    {
        break;
    }
    }
}

void AddCommand::redo()
{
    switch(type)
    {
    case STATE:
    {
        emit(redo_addState(attrState, idAutomaton));
        break;
    }
    case EVENT:
    {
        emit(redo_addEvent(attrEvent, idAutomaton));
        break;
    }
    case TRANSITION:
    {
        emit(redo_addTransition(attrTransition, idAutomaton));
        break;
    }
    case AUTOMATON:
    {
        emit(redo_addAutomaton(attrAutomaton));
        break;
    }
    default:
    {
        break;
    }
    }
}

/*QString createCommandString(Attribute *item)
{
    return QObject::tr("%1 %3")
            .arg((item->getType() == STATE)?"state":((item->getType() == EVENT)?"event":"transition"))
            .arg(item->getName());
}
*/
