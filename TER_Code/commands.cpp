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
    default:
    {
        break;
    }
    }

    setText(param.text());
}

EditCommand::EditCommand(State oldAttr, State attr, MainWindow *parent):
    attrState(attr),
    oldState(oldAttr),
    parent(parent)
{
    type = STATE;
    setText(QObject::tr("Edit %1")
            .arg(createCommandString(&oldAttr)));
}

EditCommand::EditCommand(Event oldAttr, Event attr, MainWindow *parent):
    attrEvent(attr),
    oldEvent(oldAttr),
    parent(parent)
{
    type = EVENT;
    setText(QObject::tr("Edit %1")
            .arg(createCommandString(&oldAttr)));
}

EditCommand::EditCommand(Transition oldAttr, Transition attr, MainWindow *parent):
    attrTransition(attr),
    oldTransition(oldAttr),
    parent(parent)
{
    type = TRANSITION;
    setText(QObject::tr("Edit %1")
            .arg(createCommandString(&oldAttr)));
}

void EditCommand::undo()
{
    switch(type)
    {
    case STATE:
    {
        emit(undo_editState(oldState));
        break;
    }
    case EVENT:
    {
        emit(undo_editEvent(oldEvent));
        break;
    }
    case TRANSITION:
    {
        emit(undo_editTransition(oldTransition));
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
        emit(redo_editState(attrState));
        break;
    }
    case EVENT:
    {
        emit(redo_editEvent(attrEvent));
        break;
    }
    case TRANSITION:
    {
        emit(redo_editTransition(attrTransition));
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
    default:
    {
        break;
    }
    }

    setText(param.text());
}

DeleteCommand::DeleteCommand(QList<State> attr, MainWindow *parent):
    attrState(attr),
    parent(parent)
{
    type = STATE;
    if(attr.length()>1)
        for(int i = 0; i < attr.length(); i++)
            setText(QObject::tr("Delete multiple states"));
    else
        setText(QObject::tr("Delete %1")
                .arg(createCommandString(&(attr[0]))));
}

DeleteCommand::DeleteCommand(QList<Event> attr, MainWindow *parent):
    attrEvent(attr),
    parent(parent)
{
    type = EVENT;
    if(attr.length() > 1)
    for(int i = 0; i < attr.length(); i++)
        setText(QObject::tr("Delete multiple events"));
    else
        setText(QObject::tr("Delete %1")
                .arg(createCommandString(&(attr[0]))));

}

DeleteCommand::DeleteCommand(QList<Transition> attr, MainWindow *parent):
    attrTransition(attr),
    parent(parent)
{
    type = TRANSITION;
    if(attr.length() > 1)
    for(int i = 0; i < attr.length(); i++)
        setText(QObject::tr("Delete multiple transitions"));
    else
        setText(QObject::tr("Delete %1")
                .arg(createCommandString(&(attr[0]))));
}

void DeleteCommand::undo()
{
    switch(type)
    {
    case STATE:
    {
        for(State s : attrState)
            emit(undo_deleteState(s));
        break;
    }
    case EVENT:
    {
        for(Event e : attrEvent)
            emit(undo_deleteEvent(e));
        break;
    }
    case TRANSITION:
    {
        for(Transition t : attrTransition)
            emit(undo_deleteTransition(t));
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
        emit(redo_deleteState(deleteList));
        break;
    }
    case EVENT:
    {
        for(Event e : attrEvent)
            deleteList.append(e.getId());
        emit(redo_deleteEvent(deleteList));
        break;
    }
    case TRANSITION:
    {
        for(Transition t : attrTransition)
            deleteList.append(t.getId());
        emit(redo_deleteTransition(deleteList));
        break;
    }
    default:
    {
        break;
    }
    }
}

AddCommand::AddCommand(State attr, MainWindow *parent):
    attrState(attr),
    parent(parent)
{
    type = STATE;
    setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));
}

AddCommand::AddCommand(Event attr, MainWindow *parent):
    attrEvent(attr),
    parent(parent)
{
    type = EVENT;
    setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));
}

AddCommand::AddCommand(Transition attr, MainWindow *parent):
    attrTransition(attr),
    parent(parent)
{
    type = TRANSITION;
    setText(QObject::tr("Add %1")
            .arg(createCommandString(&attr)));
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
    default:
    {
        break;
    }
    }
    setText(param.text());
}

void AddCommand::undo()
{
    switch(type)
    {
    case STATE:
    {
        emit(undo_addState(QList<int>({attrState.getId()})));
        break;
    }
    case EVENT:
    {
        emit(undo_addEvent(QList<int>({attrEvent.getId()})));
        break;
    }
    case TRANSITION:
    {
        emit(undo_addTransition(QList<int>({attrTransition.getId()})));
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
        emit(redo_addState(attrState));
        break;
    }
    case EVENT:
    {
        emit(redo_addEvent(attrEvent));
        break;
    }
    case TRANSITION:
    {
        emit(redo_addTransition(attrTransition));
        break;
    }
    default:
    {
        break;
    }
    }
}

QString createCommandString(Attribute *item)
{
    return QObject::tr("%1 %3")
            .arg((item->getType() == STATE)?"state":((item->getType() == EVENT)?"event":"transition"))
            .arg(item->getName());
}
