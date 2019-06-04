#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "automaton.hpp"
#include "mainwindow.hpp"

#include <QUndoStack>
#include <QObject>
#include <QMetaType>

class EditCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    explicit EditCommand();
    explicit EditCommand(const EditCommand &param);
    explicit EditCommand(State oldAttr, State attr, MainWindow *parent);
    explicit EditCommand(Event oldAttr, Event attr, MainWindow *parent);
    explicit EditCommand(Transition oldAttr, Transition attr, MainWindow *parent);


    void undo() override;
    void redo() override;

private:
    State attrState;
    State oldState;
    Event attrEvent;
    Event oldEvent;
    Transition attrTransition;
    Transition oldTransition;
    MainWindow *parent;
    enum attribute_type type;

signals :
    //two signals per type
    void redo_editTransition(Transition);
    void undo_editTransition(Transition);

    void redo_editState(State);
    void undo_editState(State);

    void redo_editEvent(Event);
    void undo_editEvent(Event);

};

class DeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    explicit DeleteCommand();
    explicit DeleteCommand(const DeleteCommand& param);
    explicit DeleteCommand(QList<State> attr, MainWindow *parent);
    explicit DeleteCommand(QList<Event> attr, MainWindow *parent);
    explicit DeleteCommand(QList<Transition> attr, MainWindow *parent);

    void undo() override;
    void redo() override;

private:
    QList<State> attrState;
    QList<Event> attrEvent;
    QList<Transition> attrTransition;
    MainWindow *parent;
    enum attribute_type type;

signals :
    //two signals per type
    void redo_deleteTransition(QList<int>);
    void undo_deleteTransition(Transition);

    void redo_deleteState(QList<int>);
    void undo_deleteState(State);

    void redo_deleteEvent(QList<int>);
    void undo_deleteEvent(Event);

};

class AddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    explicit AddCommand();
    explicit AddCommand(State attr, MainWindow *parent);
    explicit AddCommand(Event attr, MainWindow *parent);
    explicit AddCommand(Transition attr, MainWindow *parent);
    explicit AddCommand(const AddCommand& param);

    void undo() override;
    void redo() override;

private:
    State attrState;
    Event attrEvent;
    Transition attrTransition;
    MainWindow *parent;
    enum attribute_type type;

signals :
    //two signals per type
    void redo_addTransition(Transition);
    void undo_addTransition(QList<int>);

    void redo_addState(State);
    void undo_addState(QList<int>);

    void redo_addEvent(Event);
    void undo_addEvent(QList<int>);
};

QString createCommandString(Attribute *item);

Q_DECLARE_METATYPE(AddCommand);
Q_DECLARE_METATYPE(DeleteCommand);
Q_DECLARE_METATYPE(EditCommand);

#endif // COMMANDS_HPP
