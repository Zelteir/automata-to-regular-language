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
    explicit EditCommand(State oldAttr, State attr, MainWindow *parent, int automaton);
    explicit EditCommand(Event oldAttr, Event attr, MainWindow *parent, int automaton);
    explicit EditCommand(Transition oldAttr, Transition attr, MainWindow *parent, int automaton);
    explicit EditCommand(Automaton oldAttr, Automaton attr, MainWindow *parent);


    void undo() override;
    void redo() override;

private:
    State attrState;
    State oldState;
    Event attrEvent;
    Event oldEvent;
    Transition attrTransition;
    Transition oldTransition;
    Automaton attrAutomaton;
    Automaton oldAutomaton;
    int idAutomaton;
    MainWindow *parent;
    enum attribute_type type;

signals :
    //two signals per type
    void redo_editTransition(Transition, int);
    void undo_editTransition(Transition, int);

    void redo_editState(State, int);
    void undo_editState(State, int);

    void redo_editEvent(Event, int);
    void undo_editEvent(Event, int);

    void redo_editAutomaton(Automaton);
    void undo_editAutomaton(Automaton);

};

class DeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    explicit DeleteCommand();
    explicit DeleteCommand(const DeleteCommand& param);
    explicit DeleteCommand(QList<State> attr, MainWindow *parent, int automaton);
    explicit DeleteCommand(QList<Event> attr, MainWindow *parent, int automaton);
    explicit DeleteCommand(QList<Transition> attr, MainWindow *parent, int automaton);
    explicit DeleteCommand(QList<Automaton> attr, MainWindow *parent);

    void undo() override;
    void redo() override;

private:
    QList<State> attrState;
    QList<Event> attrEvent;
    QList<Transition> attrTransition;
    QList<Automaton> attrAutomaton;
    int idAutomaton;
    MainWindow *parent;
    enum attribute_type type;

signals :
    //two signals per type
    void redo_deleteTransition(QList<int>, int);
    void undo_deleteTransition(Transition, int);

    void redo_deleteState(QList<int>, int);
    void undo_deleteState(State, int);

    void redo_deleteEvent(QList<int>, int);
    void undo_deleteEvent(Event, int);

    void redo_deleteAutomaton(QList<int>);
    void undo_deleteAutomaton(Automaton);

};

class AddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    explicit AddCommand();
    explicit AddCommand(const AddCommand& param);
    explicit AddCommand(State attr, MainWindow *parent, int automaton);
    explicit AddCommand(Event attr, MainWindow *parent, int automaton);
    explicit AddCommand(Transition attr, MainWindow *parent, int automaton);
    explicit AddCommand(Automaton attr, MainWindow *parent);

    void undo() override;
    void redo() override;

private:
    State attrState;
    Event attrEvent;
    Transition attrTransition;
    Automaton attrAutomaton;
    int idAutomaton;
    MainWindow *parent;
    enum attribute_type type;

signals :
    //two signals per type
    void redo_addTransition(Transition, int);
    void undo_addTransition(QList<int>, int);

    void redo_addState(State, int);
    void undo_addState(QList<int>, int);

    void redo_addEvent(Event, int);
    void undo_addEvent(QList<int>, int);

    void redo_addAutomaton(Automaton);
    void undo_addAutomaton(QList<int>);
};

//QString createCommandString(Attribute *item);

Q_DECLARE_METATYPE(AddCommand);
Q_DECLARE_METATYPE(DeleteCommand);
Q_DECLARE_METATYPE(EditCommand);

#endif // COMMANDS_HPP
