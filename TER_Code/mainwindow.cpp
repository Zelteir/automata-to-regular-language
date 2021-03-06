#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QSignalBlocker>
#include <QXmlStreamWriter>
#include <QElapsedTimer>
#include "create_automaton_dialog.hpp"
#include "create_state_dialog.hpp"
#include "create_event_dialog.hpp"
#include "create_transition_dialog.hpp"
#include "delete_transition_dialog.hpp"
#include "delete_state_dialog.hpp"
#include "delete_event_dialog.hpp"
#include "delete_automaton_dialog.hpp"
#include "help_dialog.hpp"
#include "commands.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Automatons_list->hideColumn(0); //hide ID column
    ui->Automatons_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Events_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Events_list->hideColumn(0);
    ui->States_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->States_list->hideColumn(0);
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch );
    ui->Transitions_list->hideColumn(0);
    method = new QActionGroup(this);    //exclusive options
    ui->actionBrzozowski->setActionGroup(method);
    ui->actionBrzozowski_V2->setActionGroup(method);
    ui->actionReverse_Brzozowski->setActionGroup(method);
    ui->actionTransitive_closure->setActionGroup(method);
    method->setExclusive(true);
    ui->menuBar->addAction(ui->actionHelp);

    undoStack = new QUndoStack(this);
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    ui->menuBar->addAction(undoAction);
    //undoAction->setVisible(false);
    ui->menuBar->addAction(redoAction);
    //redoAction->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete method;
    delete ui;
}

/*
 * Toggle the interface to user
*/
void MainWindow::toggle_interface(bool b)
{
    ui->Events_list->setEnabled(b);
    ui->States_list->setEnabled(b);
    ui->Automatons_list->setEnabled(b);
    ui->Transitions_list->setEnabled(b);
    ui->Generate_Button->setEnabled(b);
    ui->Generated_Regular_Language->setEnabled(b);
    ui->actionClose->setEnabled(b);
    ui->menuAdd->setEnabled(b);
    ui->actionStateCreate->setEnabled(b);
    ui->actionEventCreate->setEnabled(b);
    ui->actionTransitionCreate->setEnabled(b);
    ui->Ignore_Unobservable_check->setEnabled(b);
    ui->Ignore_Uncontrolable_check->setEnabled(b);
    ui->actionSaveAutomaton->setEnabled(b);
    ui->actionGenerate_all_languages->setEnabled(b);
    ui->Minimize_Language_check->setEnabled(b);
    ui->actionAvoid_language_ambiguity->setEnabled(b);
    ui->menuConversion_method->setEnabled(b);
    ui->menuDelete->setEnabled(b);
    ui->actionTransitionDelete->setEnabled(b);
    ui->actionStateDelete->setEnabled(b);
    ui->actionEventDelete->setEnabled(b);
    ui->addEvent_button->setEnabled(b);
    ui->addState_button->setEnabled(b);
    ui->addTransition_button->setEnabled(b);
    ui->deleteEvent_button->setEnabled(b);
    ui->deleteState_button->setEnabled(b);
    ui->deleteTransition_button->setEnabled(b);
    ui->actionAutomatonCreate->setEnabled(b);
    ui->actionAutomatonDelete->setEnabled(b);
    ui->addAutomaton_button->setEnabled(b);
    ui->deleteAutomaton_button->setEnabled(b);
    ui->menuExport->setEnabled(b);
    ui->actionSave_automatons_as->setEnabled(b);
    /*
     * TO DO
     * Other things to toggle
    */
    if(b)
        this->fill_automaton_list();
}

/*
 * Function to add a state to the interface list of state
*/
void MainWindow::add_state_to_list(State s)
{
    QSignalBlocker states_blocker(ui->States_list);
    int pos = ui->States_list->rowCount();
    ui->States_list->insertRow(pos);
    ui->States_list->setCurrentCell(pos,0);
    //add id and name
    ui->States_list->setItem(ui->States_list->currentRow(),0,new QTableWidgetItem(QString::number(s.getId())));
    ui->States_list->setItem(ui->States_list->currentRow(),1,new QTableWidgetItem(s.getName()));
    ui->States_list->item(ui->States_list->currentRow(),1)->setTextAlignment(Qt::AlignHCenter);
    //access boolean value and set checkbox state accordingly, flags prevent edition of checkbox text
    ui->States_list->setItem(ui->States_list->currentRow(),2,new Table_Widget_Checkbox_Item(""));
    if(s.getInitial())
    {
        ui->States_list->item(ui->States_list->currentRow(),2)->setCheckState(Qt::Checked);
    }
    else {
        ui->States_list->item(ui->States_list->currentRow(),2)->setCheckState(Qt::Unchecked);
    }
    ui->States_list->item(ui->States_list->currentRow(),2)->setFlags(ui->States_list->item(ui->States_list->currentRow(),2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
    ui->States_list->setItem(ui->States_list->currentRow(),3,new Table_Widget_Checkbox_Item(""));
    if(s.getAccepting())
    {
        ui->States_list->item(ui->States_list->currentRow(),3)->setCheckState(Qt::Checked);
    }
    else {
        ui->States_list->item(ui->States_list->currentRow(),3)->setCheckState(Qt::Unchecked);
    }
    ui->States_list->item(ui->States_list->currentRow(),3)->setFlags(ui->States_list->item(ui->States_list->currentRow(),3)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
    states_blocker.unblock();
}

/*
 * Function to add an event to the interface list of event
*/
void MainWindow::add_event_to_list(Event e)
{
    int pos = ui->Events_list->rowCount();
    ui->Events_list->insertRow(pos);
    ui->Events_list->setCurrentCell(pos,0);
    //add Id and label
    ui->Events_list->setItem(ui->Events_list->currentRow(),0,new QTableWidgetItem(QString::number(e.getId())));
    ui->Events_list->setItem(ui->Events_list->currentRow(),1,new QTableWidgetItem(e.getName()));
    ui->Events_list->item(ui->Events_list->currentRow(),1)->setTextAlignment(Qt::AlignHCenter);
    //access boolean value and set checkbox state accordingly, flags prevent edition of checkbox text
    ui->Events_list->setItem(ui->Events_list->currentRow(),2,new Table_Widget_Checkbox_Item(""));
    if(!e.getObservable())
    {
        ui->Events_list->item(ui->Events_list->currentRow(),2)->setCheckState(Qt::Checked);
    }
    else
    {
        ui->Events_list->item(ui->Events_list->currentRow(),2)->setCheckState(Qt::Unchecked);
    }
    ui->Events_list->item(ui->Events_list->currentRow(),2)->setFlags(ui->Events_list->item(ui->Events_list->currentRow(),2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
    ui->Events_list->setItem(ui->Events_list->currentRow(),3,new Table_Widget_Checkbox_Item(""));
    if(!e.getControlable())
    {
        ui->Events_list->item(ui->Events_list->currentRow(),3)->setCheckState(Qt::Checked);
    }
    else
    {
        ui->Events_list->item(ui->Events_list->currentRow(),3)->setCheckState(Qt::Unchecked);
    }
    ui->Events_list->item(ui->Events_list->currentRow(),3)->setFlags(ui->Events_list->item(ui->Events_list->currentRow(),3)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
}

/*
 * Function to add a transition to the interface list of transition
*/
void MainWindow::add_transition_to_list(Transition t)
{
    int pos = ui->Transitions_list->rowCount();
    ui->Transitions_list->insertRow(pos);
    ui->Transitions_list->setCurrentCell(pos,0);

    ui->Transitions_list->setItem(ui->Transitions_list->currentRow(),0, new QTableWidgetItem(QString::number(t.getId())));
    ui->Transitions_list->setItem(ui->Transitions_list->currentRow(),1, new QTableWidgetItem(currentAutomaton->getState(t.getSource()).getName()));
    ui->Transitions_list->setItem(ui->Transitions_list->currentRow(),2, new QTableWidgetItem(currentAutomaton->getState(t.getDest()).getName()));
    ui->Transitions_list->setItem(ui->Transitions_list->currentRow(),3, new QTableWidgetItem(currentAutomaton->getEvent(t.getEvent()).getName()));
}

/*
 * Fill the interface with the information contained in the current automaton
*/
void MainWindow::fill_interface()
{
    clear_interface();

    /*BLock signals to avoid calling 'on_item_changed' functions*/
    QSignalBlocker states_blocker(ui->States_list);
    QSignalBlocker events_blocker(ui->Events_list);
    QSignalBlocker transitions_blocker(ui->Transitions_list);

    /*fill states table with name, initial and accepted information*/
    for(State s: *(currentAutomaton->getStateList()))
    {
        add_state_to_list(s);
    }
    if(ui->States_list->rowCount() > 0)
    {
        ui->States_list->setCurrentCell(0,1);
        ui->deleteState_button->setEnabled(true);
    }
    else
        ui->deleteState_button->setEnabled(false);
    ui->addState_button->setEnabled(true);
    /*fill events table with name, observable and controlable information*/
    for(Event e : *(currentAutomaton->getEventList()))
    {
        add_event_to_list(e);
    }
    if(ui->Events_list->rowCount() > 0)
    {
        ui->Events_list->setCurrentCell(0,1);
        ui->deleteEvent_button->setEnabled(true);
    }
    else
        ui->deleteEvent_button->setEnabled(false);
    ui->addEvent_button->setEnabled(true);
    /*fill transition table with origin, destination and event information*/
    for(Transition t : *(currentAutomaton->getTransitionList()))
    {
        add_transition_to_list(t);
    }
    if(ui->Transitions_list->rowCount() > 0)
    {
        ui->Transitions_list->setCurrentCell(0,1);
        ui->deleteTransition_button->setEnabled(true);

    }
    else
        ui->deleteTransition_button->setEnabled(false);
    ui->addTransition_button->setEnabled(true);
    states_blocker.unblock();
    events_blocker.unblock();
    transitions_blocker.unblock();
    ui->Generated_Regular_Language->setPlainText(currentAutomaton->getGeneratedLanguage());
}

/*
 * Fill the list of automaton wontained inside the automata
*/
void MainWindow::fill_automaton_list()
{
    int i = 0;
    clear_automaton_list();
    QSignalBlocker automatons_blocker(ui->Automatons_list);
    for(Automaton tmp : *this->automata.get_automatons())
    {
        ui->Automatons_list->insertRow(i);
        ui->Automatons_list->setItem(i,0, new QTableWidgetItem(QString::number(tmp.getId())));
        ui->Automatons_list->setItem(i,1, new QTableWidgetItem(tmp.getName()));
        ui->Automatons_list->item(i,1)->setFlags(ui->Automatons_list->item(i,1)->flags() | Qt::ItemIsEditable);
        i++;
    }
    automatons_blocker.unblock();
    ui->Automatons_list->setCurrentCell(0,1);
}

/*
 * Clear interface of information
*/
void MainWindow::clear_interface()
{
    QSignalBlocker states_blocker(ui->States_list);
    QSignalBlocker events_blocker(ui->Events_list);
    QSignalBlocker transitions_blocker(ui->Transitions_list);
    ui->Events_list->clearContents();
    ui->Events_list->setRowCount(0);
    ui->States_list->clearContents();
    ui->States_list->setRowCount(0);
    ui->Transitions_list->clearContents();
    ui->Transitions_list->setRowCount(0);
    ui->Generated_Regular_Language->clear();
    states_blocker.unblock();
    events_blocker.unblock();
    transitions_blocker.unblock();
}

/*
 * Clear the automaton list
*/
void MainWindow::clear_automaton_list()
{
    QSignalBlocker automatons_blocker(ui->Automatons_list);
    ui->Automatons_list->clearContents();
    ui->Automatons_list->setRowCount(0);
    automatons_blocker.unblock();
}

/*
 * Refresh display when user select another automaton
*/
void MainWindow::on_Automatons_list_itemSelectionChanged()
{
    if(ui->Automatons_list->rowCount() == 0)
    {
        clear_interface();
    }
    else {
        currentAutomaton = automata.get_automaton_at(ui->Automatons_list->item(ui->Automatons_list->currentRow(), 0)->text().toInt());
        if(currentAutomaton->getGeneratedLanguage().isEmpty())
        {
            ui->actionSaveRL->setEnabled(false);
            ui->actionSaveRL_as->setEnabled(false);
        }
        else
        {
            ui->actionSaveRL->setEnabled(true);
            ui->actionSaveRL_as->setEnabled(true);
        }
        fill_interface();
    }
}

/*
 * Call selected conversion method for selected automaton
 * Ensure valid attributes for automaton (at least one accepting state & exactly one initial state)
*/
void MainWindow::generateLanguage(Automaton *a)
{
    int nb_init = 0;
    bool accept = false;
    QElapsedTimer timer;
    Automaton tmp;
    //contain exactly one initial state
    //contain at least one eccepting state
    for(State s : *a->getStateList())
    {
        if(s.getAccepting())
            accept = true;
        if(s.getInitial())
            nb_init++;
    }

    if(nb_init != 1)
    {
        QMessageBox::information(this, tr("Incorrect graph"),
        "A graph needs exactly one initial state.");
        return;
    }
    if(!accept)
    {
        QMessageBox::information(this, tr("Incorrect graph"),
        "A graph needs at least one accepting state.");
        return;
    }
    QMap<int, Event> backEventList;
    backEventList = *a->getEventList();
    if(ui->actionAvoid_language_ambiguity->isChecked()) //place '.' directly inside automaton
    {
        QMap<int, Event> tmpEventList;
        for(Event e : *a->getEventList())
        {
            e.setName(e.getName()+".");
            //tmpEventList.append(e);
            tmpEventList.insert(e.getId(),e);
        }
        a->setEventList(tmpEventList);
    }
    qDebug() << "nb states : " << a->getIdState();
    if(!ui->Minimize_Language_check->isChecked())
        //tmp = translator.automatonMinimization(*a);
        tmp = *a;
    else
    {
        timer.start();
        tmp = translator.automatonMinimizationV2(*a);
        qDebug() << "new nb states :" << tmp.getIdState();
        qDebug() << "The minimization took" << timer.elapsed() << "milliseconds";
    }

    if(ui->actionBrzozowski->isChecked())
    {
        timer.restart();
        translator.brzozowskiMethod(tmp, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
        qDebug() << "The conversion took" << timer.elapsed() << "milliseconds";
    }
    else if(ui->actionBrzozowski_V2->isChecked())
    {
        translator.brzozowskiMethodV2(tmp, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    }
    else if(ui->actionReverse_Brzozowski->isChecked())
    {
        translator.reverseBrzozowski(tmp, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    }
    else if(ui->actionTransitive_closure->isChecked())
    {
        translator.transitive_Closure(tmp, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    }

    if(ui->actionAvoid_language_ambiguity->isChecked()) //delete inappropriate or misplaced '.' from avoid ambiguity
    {
        int i = 0;
        QString tmpString = translator.getRegex();
        a->setEventList(backEventList);
        while (i < tmpString.length()) {
            if(tmpString.at(i) == '.')
            {
                if(i == 0 || i == tmpString.length()-1)
                    tmpString.remove(i,1);
                else if(tmpString.at(i+1) == '(' || tmpString.at(i+1) == ')' || tmpString.at(i+1) == '+' || tmpString.at(i+1) == '*')
                    tmpString.remove(i,1);
                else
                    i++;
            }
            else
                i++;
        }
        a->setGeneratedLanguage(tmpString);
    }
    else
        a->setGeneratedLanguage(translator.getRegex());
}

/*
 * Send the current automaton's information to the translator and generate the regular language assossiated
*/
void MainWindow::on_Generate_Button_clicked()
{
    generateLanguage(currentAutomaton);
    ui->Generated_Regular_Language->setPlainText(currentAutomaton->getGeneratedLanguage());
    ui->actionSaveRL->setEnabled(true);
    ui->actionSaveRL_as->setEnabled(true);
}

/*
 * clear interface and disable it when current file is closed
*/
void MainWindow::on_actionClose_triggered()
{
    toggle_interface(false);
    clear_interface();
    clear_automaton_list();
    ui->States_list->setRowCount(0);
    ui->Events_list->setRowCount(0);
    ui->Transitions_list->setRowCount(0);
    ui->actionSaveRL->setEnabled(false);
    ui->actionSaveRL_as->setEnabled(false);
}

/*
 * whenever user modify the name of an automaton
 * ensure name isn't already in use
 * create command for undo/redo
*/
void MainWindow::on_Automatons_list_itemChanged(QTableWidgetItem *item)
{
    Automaton a = *currentAutomaton;
    Automaton oldAutomaton = a;
    QString old = a.getName();
    for (int i = 0;i < ui->Automatons_list->rowCount();i++) {
        if(ui->Automatons_list->item(i,1)->text() == item->text() && i != ui->Automatons_list->currentRow())
        {
            QMessageBox::information(this, tr("Error"),
            QString("This name is already in use."));
            item->setText(old);
            return;
        }
    }

    a.setName(item->text());
    EditCommand *editCommand = new EditCommand(oldAutomaton, a, this);
    connect(editCommand, SIGNAL(redo_editAutomaton(Automaton)), this, SLOT(automatons_list_itemChanged(Automaton)));
    connect(editCommand, SIGNAL(undo_editAutomaton(Automaton)), this, SLOT(automatons_list_itemChanged(Automaton)));

    undoStack->push(editCommand);
}

/*
 * slot for undo/redo command
 * modify name in memory, and update display
*/
void MainWindow::automatons_list_itemChanged(Automaton a)
{
    int pos = 0;
    for(int i = 0; i < ui->Automatons_list->rowCount(); i++)
    {
        if(ui->States_list->item(i,0)->text().toInt() == a.getId())
            pos = i;
    }
    QSignalBlocker automatons_blocker(ui->Automatons_list);
    automata.get_automatons()->insert(a.getId(),a);
    ui->Automatons_list->item(pos, 1)->setText(a.getName());
    automatons_blocker.unblock();
}

/*
 * whenever the user change an element in the state list
 * ensure the name isn't alredy in use
 * modification on the automaton
*/
void MainWindow::on_States_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker states_blocker(ui->States_list);
    State s = currentAutomaton->getState(ui->States_list->item(item->row(),0)->text().toInt());
    State newS = s;

    QString old = s.getName();
    if (item->column() == 1) {
        //if the modifiaction was on the name
        //check if the name isn't already in use as state or event
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,1)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                                         QString("This name is already in use."));
                item->setText(old);
                states_blocker.unblock();
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,1)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                                         QString("This name is already in use."));
                item->setText(old);
                states_blocker.unblock();
                return;
            }
        }
    }

    states_blocker.unblock();

    if(item->column() == 1)
        newS.setName(ui->States_list->item(item->row(),1)->text());
    else if(item->column() == 2)
        newS.setInitial(!s.getInitial());
    else if(item->column() == 3)
        newS.setAccepting(!s.getAccepting());

    EditCommand *editCommand = new EditCommand(s, newS, this, currentAutomaton->getId());
    connect(editCommand, SIGNAL(redo_editState(State, int)), this, SLOT(states_list_itemChanged(State, int)));
    connect(editCommand, SIGNAL(undo_editState(State, int)), this, SLOT(states_list_itemChanged(State, int)));

    undoStack->push(editCommand);
}

void MainWindow::states_list_itemChanged(State s, int automaton)
{
    int pos = 0;
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    for(int i = 0; i < ui->States_list->rowCount(); i++)
    {
        if(ui->States_list->item(i,0)->text().toInt() == s.getId())
            pos = i;
    }
    QSignalBlocker states_blocker(ui->States_list);
    currentAutomaton->getStateList()->insert(s.getId(),s);
    ui->States_list->item(pos,1)->setText(s.getName());
    ui->States_list->item(pos,2)->setCheckState(s.getInitial()?Qt::Checked:Qt::Unchecked);
    ui->States_list->item(pos,3)->setCheckState(s.getAccepting()?Qt::Checked:Qt::Unchecked);
    states_blocker.unblock();
    for(Transition t : *currentAutomaton->getTransitionList())
    {
        if(t.getSource() == s.getId()|| t.getDest() == s.getId())
            for(int i = 0; i < ui->Transitions_list->rowCount();i++)
            {
                if(ui->Transitions_list->item(i,0)->text().toInt() == t.getId())
                    emit(transitions_list_itemChanged(t, automaton));
            }
    }
}


/*
 * whenever the user change an element in the event list
 * ensure the name isn't already in use
 * modification on the automaton
*/
void MainWindow::on_Events_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker events_blocker(ui->Events_list);
    Event e = currentAutomaton->getEvent(ui->Events_list->item(item->row(),0)->text().toInt());
    Event newE = e;
    QString old = e.getName();
    if (item->column() == 1) {
    //if the label was changed
    //check if the name isn't already in use  as state or event
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,1)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(old);
                events_blocker.unblock();
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,1)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(old);
                events_blocker.unblock();
                return;
            }
        }
    }

    events_blocker.unblock();

    if(item->column() == 1)
        newE.setName(ui->Events_list->item(item->row(),1)->text());
    else if(item->column() == 2)
        newE.setObservable(!e.getObservable());
    else if(item->column() == 3)
        newE.setControlable(!e.getControlable());

    EditCommand *editCommand = new EditCommand(e, newE, this, currentAutomaton->getId());
    connect(editCommand, SIGNAL(redo_editEvent(Event, int)), this, SLOT(events_list_itemChanged(Event, int)));
    connect(editCommand, SIGNAL(undo_editEvent(Event, int)), this, SLOT(events_list_itemChanged(Event, int)));

    undoStack->push(editCommand);
}

void MainWindow::events_list_itemChanged(Event e, int automaton)
{
    int pos = 0;
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    for(int i = 0; i < ui->Events_list->rowCount(); i++)
    {
        if(ui->Events_list->item(i,0)->text().toInt() == e.getId())
            pos = i;
    }
    QSignalBlocker events_blocker(ui->Events_list);
    currentAutomaton->getEventList()->insert(e.getId(),e);
    ui->Events_list->item(pos,1)->setText(e.getName());
    ui->Events_list->item(pos,2)->setCheckState(e.getObservable()?Qt::Unchecked:Qt::Checked);
    ui->Events_list->item(pos,3)->setCheckState(e.getControlable()?Qt::Unchecked:Qt::Checked);
    events_blocker.unblock();
    for(Transition t : *currentAutomaton->getTransitionList())
    {
        if(t.getEvent() == e.getId())
            for(int i = 0; i < ui->Transitions_list->rowCount();i++)
            {
                if(ui->Transitions_list->item(i,0)->text().toInt() == t.getId())
                    emit(transitions_list_itemChanged(t, automaton));
            }
    }
}

/*
 * whenever the user modify an element in the transition list,
 * ensure the transition doesn't already exist & modified element exist
 * modification of the automaton
 */
void MainWindow::on_Transitions_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker transition_blocker(ui->Transitions_list);
    int s = -1;
    int d = -1;
    int e = -1;
    int i = 0;
    Transition t = currentAutomaton->getTransition(ui->Transitions_list->item(item->row(),0)->text().toInt());
    Transition newT = t;
    switch (item->column()) {
    case 1:
        //if the source was modified, check if it exist
        while (i < ui->States_list->rowCount()) {
            if(ui->States_list->item(i,1)->text() == item->text())
            {
                s = i;
                break;
            }
            i++;
        }
        if(s == -1)
        {
            QMessageBox::information(this, tr("Error"),
            QString("This state does not exist."));
            item->setText(currentAutomaton->getState(currentAutomaton->getTransition(t.getId()).getSource()).getName());
            return;
        }
        newT.setSource(s);
        for(Transition tmp : *currentAutomaton->getTransitionList())
        {
            if(newT == tmp)
            {
                QMessageBox::information(this, tr("Error"),
                QString("This transition already exist."));
                item->setText(currentAutomaton->getState(currentAutomaton->getTransition(t.getId()).getSource()).getName());
                return;
            }
        }
        break;
    case 2:
        //check if the destination was modified
        while (i < ui->States_list->rowCount()) {
            if(ui->States_list->item(i,1)->text() == item->text())
            {
                d = i;
                break;
            }
            i++;
        }
        if(d == -1)
        {
            QMessageBox::information(this, tr("Error"),
            QString("This state does not exist."));
            item->setText(currentAutomaton->getState(currentAutomaton->getTransition(t.getId()).getDest()).getName());
            return;
        }
        newT.setDest(d);
        for(Transition tmp : *currentAutomaton->getTransitionList())
        {
            if(newT == tmp)
            {
                QMessageBox::information(this, tr("Error"),
                QString("This transition already exist."));
                item->setText(currentAutomaton->getState(currentAutomaton->getTransition(t.getId()).getDest()).getName());
                return;
            }
        }
        break;
    case 3:
        //if the event was modified
        while (i < ui->Events_list->rowCount()) {
            if(ui->Events_list->item(i,1)->text() == item->text())
            {
                e = i;
                break;
            }
            i++;
        }
        if(e == -1)
        {
            QMessageBox::information(this, tr("Error"),
            QString("This event does not exist."));
            item->setText(currentAutomaton->getEvent(currentAutomaton->getTransition(t.getId()).getEvent()).getName());
            return;
        }
        newT.setEvent(e);
        for(Transition tmp : *currentAutomaton->getTransitionList())
        {
            if(newT == tmp)
            {
                QMessageBox::information(this, tr("Error"),
                QString("This transition already exist."));
                item->setText(currentAutomaton->getEvent(currentAutomaton->getTransition(t.getId()).getEvent()).getName());
                return;
            }
        }
        break;
    }

    transition_blocker.unblock();

    EditCommand *editCommand = new EditCommand(t, newT, this, currentAutomaton->getId());
    connect(editCommand, SIGNAL(redo_editTransition(Transition, int)), this, SLOT(transitions_list_itemChanged(Transition, int)));
    connect(editCommand, SIGNAL(undo_editTransition(Transition, int)), this, SLOT(transitions_list_itemChanged(Transition, int)));
    undoStack->push(editCommand);
}

void MainWindow::transitions_list_itemChanged(Transition t, int automaton)
{
    int pos = 0;
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    for(int i = 0; i < ui->Transitions_list->rowCount(); i++)
    {
        if(ui->Transitions_list->item(i,0)->text().toInt() == t.getId())
            pos = i;
    }
    QSignalBlocker transition_blocker(ui->Transitions_list);
    currentAutomaton->getTransitionList()->insert(t.getId(),t);
    ui->Transitions_list->item(pos,1)->setText(currentAutomaton->getState(t.getSource()).getName());
    ui->Transitions_list->item(pos,2)->setText(currentAutomaton->getState(t.getDest()).getName());
    ui->Transitions_list->item(pos,3)->setText(currentAutomaton->getEvent(t.getEvent()).getName());
    transition_blocker.unblock();
}

/*
 * slot for 'create automaton' button
 * display create automaton dialog
*/
void MainWindow::on_actionAutomatonCreate_triggered()
{
    Create_automaton_dialog dialog(automata.getIdAutomaton(), *automata.get_automatons(), this);
    connect(&dialog, SIGNAL(creation_automaton(Automaton)), this, SLOT(createAutomaton_finished(Automaton)));
    dialog.exec();
}

/*
 * Insert new automaton in automaton group and add it to display
*/
void MainWindow::createAutomaton_finished(Automaton a)
{
    AddCommand *addCommand = new AddCommand(a, this);
    connect(addCommand, SIGNAL(redo_addAutomaton(Automaton)), this, SLOT(createAutomaton(Automaton)));
    connect(addCommand, SIGNAL(undo_addAutomaton(QList<int>)), this, SLOT(deleteAutomaton(QList<int>)));
    undoStack->push(addCommand);
}

void MainWindow::createAutomaton(Automaton a)
{
    QSignalBlocker automaton_blocker(ui->Automatons_list);
    automata.get_automatons()->insert(a.getId(),a);
    if(automata.getIdAutomaton() == a.getId())
        automata.idAutomatonIncr();
    ui->Automatons_list->insertRow(ui->Automatons_list->rowCount());
    ui->Automatons_list->setItem(ui->Automatons_list->rowCount() - 1, 0, new QTableWidgetItem(QString::number(a.getId())));
    ui->Automatons_list->setItem(ui->Automatons_list->rowCount() - 1, 1, new QTableWidgetItem(a.getName()));
    automaton_blocker.unblock();
    if(ui->Automatons_list->rowCount() == 1)    //if only automaton, display it
        ui->Automatons_list->setCurrentCell(0,1);
}

/*
 * slot for 'create state' button
 * display create state dialog
 */
void MainWindow::on_actionStateCreate_triggered()
{
    Create_state_dialog dialog(currentAutomaton->getIdState(), *currentAutomaton->getEventList(),*currentAutomaton->getStateList(), this);
    connect(&dialog, SIGNAL(creation_state(State)), this, SLOT(createState_finished(State)));
    dialog.exec();
}

/*
 * Insert new state in current automaton and update display
 */
void MainWindow::createState_finished(State s)
{
    AddCommand *addCommand = new AddCommand(s, this, currentAutomaton->getId());
    connect(addCommand, SIGNAL(redo_addState(State, int)), this, SLOT(createState(State, int)));
    connect(addCommand, SIGNAL(undo_addState(QList<int>, int)), this, SLOT(deleteState(QList<int>, int)));
    undoStack->push(addCommand);
}

void MainWindow::createState(State s, int automaton)
{
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    QSignalBlocker states_blocker(ui->States_list);
    currentAutomaton->getStateList()->insert(s.getId(),s);
    if(currentAutomaton->getIdState() == s.getId())
        currentAutomaton->incrState();
    add_state_to_list(s);
    states_blocker.unblock();
    if(ui->States_list->rowCount() == 1)    //if first state, allow deletion
        ui->deleteState_button->setEnabled(true);
}

/*
 * slot for 'create event' button
 * display create event dialog
*/
void MainWindow::on_actionEventCreate_triggered()
{
    Create_event_dialog dialog(currentAutomaton->getIdEvent(), *currentAutomaton->getEventList(),*currentAutomaton->getStateList(), this);
    connect(&dialog, SIGNAL(creation_event(Event)), this, SLOT(createEvent_finished(Event)));
    dialog.exec();
}

/*
 * Insert new event in current automaton and update display
*/
void MainWindow::createEvent_finished(Event e)
{
    AddCommand *addCommand = new AddCommand(e, this, currentAutomaton->getId());
    connect(addCommand, SIGNAL(redo_addEvent(Event, int)), this, SLOT(createEvent(Event, int)));
    connect(addCommand, SIGNAL(undo_addEvent(QList<int>, int)), this, SLOT(deleteEvent(QList<int>, int)));
    undoStack->push(addCommand);
}

void MainWindow::createEvent(Event e, int automaton)
{
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    QSignalBlocker events_blocker(ui->Events_list);
    currentAutomaton->getEventList()->insert(e.getId(),e);
    if(currentAutomaton->getIdEvent() == e.getId())
        currentAutomaton->incrEvent();
    add_event_to_list(e);
    events_blocker.unblock();
    if(ui->Events_list->rowCount() == 1)    //if first event allow deletion
        ui->deleteEvent_button->setEnabled(true);
}

/*
 * slot for 'create transition' button
 * display create transition dialog
*/
void MainWindow::on_actionTransitionCreate_triggered()
{
    Create_transition_dialog *dialog = new Create_transition_dialog(currentAutomaton->getIdTransition(),*currentAutomaton->getEventList(),*currentAutomaton->getStateList(),*currentAutomaton->getTransitionList(), this);
    connect(dialog, SIGNAL(creation_transition(Transition)), this, SLOT(createTransition_finished(Transition)));
    dialog->exec();
    delete dialog;
}

/*
 * Insert new transition in current automaton and update display
*/
void MainWindow::createTransition_finished(Transition t)
{
    AddCommand *addCommand = new AddCommand(t, this, currentAutomaton->getId());
    connect(addCommand, SIGNAL(redo_addTransition(Transition, int)), this, SLOT(createTransition(Transition, int)));
    connect(addCommand, SIGNAL(undo_addTransition(QList<int>, int)), this, SLOT(deleteTransition(QList<int>, int)));
    undoStack->push(addCommand);
}

void MainWindow::createTransition(Transition t, int automaton)
{
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    QSignalBlocker transition_blocker(ui->Transitions_list);
    currentAutomaton->getTransitionList()->insert(t.getId(),t);
    if(currentAutomaton->getIdTransition() == t.getId())
        currentAutomaton->incrTransition();
    add_transition_to_list(t);
    transition_blocker.unblock();
    if(ui->Transitions_list->rowCount() == 1)   //if first transition allow deletion
        ui->deleteTransition_button->setEnabled(true);
}

/*
 * slot for 'save automaton' button
 * call registered type save via signal
*/
void MainWindow::on_actionSaveAutomaton_triggered()
{
    switch (automata.getType())
    {
    case NONE :
    {
        break;
    }
    case SUPREMICA:
    {
        emit(on_actionExportSupremica_triggered());
        break;
    }
    case SEDMA:
    {
        emit(on_actionExportSedma_triggered());
        break;
    }
    case DESUMA:
    {
        emit(on_actionExportDESUMA_triggered());
        break;
    }
        /*
         *TO DO more supported softs
        */
    }
}

/*
 * slot for 'save RL' (regular language)
 * save the generated regular language in a designated text file
 * will replace any content if the file already exist
*/
void MainWindow::on_actionSaveRL_triggered()
{
    QString file_name;
    if(automata.getRlFilePath().isEmpty())
    {
        file_name = QFileDialog::getSaveFileName(this, tr("Save regular language"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (file_name.isEmpty())
            return;
        automata.setRlFilePath(file_name);
    }
    else {
        file_name = automata.getRlFilePath();
    }
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
        file.errorString());
        return;
    }
    QTextStream out(&file);
    out << automata.get_automaton_at(ui->Automatons_list->currentRow());
    out << " : ";
    out << ui->Generated_Regular_Language->toPlainText();
    out << "\n";
    file.close();
    QMessageBox::information(this, tr("Save sucessful"),"Regular expression saved sucessfuly.");
}

/*
 * slot for 'generate all languages'
 * will generate and save in a single file regular language for all automata in current group
 * if an automaton is invalid its name will appear in the file with no language
*/
void MainWindow::on_actionGenerate_all_languages_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save regular language"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
        file.errorString());
        return;
    }
    QTextStream out(&file);
    for(Automaton a: *automata.get_automatons())
    {
        out << a.getName();
        out << " : ";
        generateLanguage(&a);
        out << a.getGeneratedLanguage();
        out << "\n";
    }
    file.close();
    QMessageBox::information(this, tr("Save sucessful"),"All regular expressions saved sucessfuly.");
}

/*
 * slot for 'delete state' button
 * display delete state dialog
*/
void MainWindow::on_actionStateDelete_triggered()
{
    Delete_state_dialog dialog(*currentAutomaton->getStateList(), this);
    connect(&dialog, SIGNAL(delete_state(QList<int>)), this, SLOT(deleteState_finished(QList<int>)));
    dialog.exec();
}

/*
 * delete states selected by user and all transitions using it
 * update display and automaton
*/
void MainWindow::deleteState_finished(QList<int> deleteList)
{
    QList<State> deleteStateList;
    for(int i : deleteList)
        deleteStateList.append(currentAutomaton->getState(i));
    DeleteCommand *deleteCommand = new DeleteCommand(deleteStateList, this, currentAutomaton->getId());
    connect(deleteCommand, SIGNAL(undo_deleteState(State, int)), this, SLOT(createState(State, int)));
    connect(deleteCommand, SIGNAL(redo_deleteState(QList<int>, int)), this, SLOT(deleteState(QList<int>, int)));
    undoStack->beginMacro("Delete State");
    undoStack->push(deleteCommand);
    undoStack->endMacro();
}

void MainWindow::deleteState(QList<int> deleteList, int automaton)
{
    int tmp;
    QString tmpString;
    QList<int> delTransitionList;
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    for(int i : deleteList)
    {
        currentAutomaton->getStateList()->remove(i);
        tmp = 0;
        while (tmp < ui->States_list->rowCount())
        {
            if(ui->States_list->item(tmp,0)->text().toInt() == i)
            {
                tmpString = ui->States_list->item(tmp,1)->text();
                ui->States_list->removeRow(tmp);
                break;
            }
            tmp++;
        }
        tmp = 0;
        while (tmp < ui->Transitions_list->rowCount())
        {
            if(ui->Transitions_list->item(tmp,1)->text() == tmpString || ui->Transitions_list->item(tmp,2)->text() == tmpString)
            {
                delTransitionList.append(ui->Transitions_list->item(tmp,0)->text().toInt());
            }
            tmp++;
        }
    }
    if(delTransitionList.size() > 0)
        emit(deleteTransition_finished(delTransitionList));
    if(ui->States_list->rowCount() == 0)
        ui->deleteState_button->setEnabled(false);
}

/*
 * slot for 'delete event' button
 * display delete event dialog
*/
void MainWindow::on_actionEventDelete_triggered()
{
    Delete_event_dialog dialog(*currentAutomaton->getEventList(), this);
    connect(&dialog, SIGNAL(delete_event(QList<int>)), this, SLOT(deleteEvent_finished(QList<int>)));
    dialog.exec();
}

/*
 * delete events selected by user and all transitions using it
 * update display and automaton
*/
void MainWindow::deleteEvent_finished(QList<int> deleteList)
{
    QList<Event> deleteEventList;
    for(int i : deleteList)
        deleteEventList.append(currentAutomaton->getEvent(i));
    DeleteCommand *deleteCommand = new DeleteCommand(deleteEventList, this, currentAutomaton->getId());
    connect(deleteCommand, SIGNAL(undo_deleteEvent(Event, int)), this, SLOT(createEvent(Event, int)));
    connect(deleteCommand, SIGNAL(redo_deleteEvent(QList<int>, int)), this, SLOT(deleteEvent(QList<int>, int)));
    undoStack->beginMacro("delete Event)");
    undoStack->push(deleteCommand);
    undoStack->endMacro();
}

void MainWindow::deleteEvent(QList<int> deleteList, int automaton)
{
    int tmp;
    QString tmpString;
    QList<int> delTransitionList;
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    for(int i : deleteList)
    {
        currentAutomaton->getEventList()->remove(i);
        tmp = 0;
        while (tmp < ui->Events_list->rowCount())
        {
            if(ui->Events_list->item(tmp,0)->text().toInt() == i)
            {
                tmpString = ui->Events_list->item(tmp,1)->text();
                ui->Events_list->removeRow(tmp);
                break;
            }
            tmp++;
        }
        tmp = 0;
        while (tmp < ui->Transitions_list->rowCount())
        {
            if(ui->Transitions_list->item(tmp,3)->text() == tmpString)
            {
                delTransitionList.append(ui->Transitions_list->item(tmp,0)->text().toInt());
            }
            tmp++;
        }
    }
    if(delTransitionList.size() > 0)
        emit(deleteTransition_finished(delTransitionList));
    if(ui->Events_list->rowCount() == 0)
        ui->deleteEvent_button->setEnabled(false);
}

/*
 * slot for 'delete transition' button
 * display delete event dialog
*/
void MainWindow::on_actionTransitionDelete_triggered()
{
    Delete_transition_dialog dialog(*currentAutomaton->getEventList(), *currentAutomaton->getStateList(), *currentAutomaton->getTransitionList(), this);
    connect(&dialog, SIGNAL(delete_transition(QList<int>)), this, SLOT(deleteTransition_finished(QList<int>)));
    dialog.exec();
}

/*
 * delete transitons selected by user
 * update display and automaton
*/
void MainWindow::deleteTransition_finished(QList<int> deleteList)
{
    QList<Transition> deleteTransitionList;
    for(int i : deleteList)
        deleteTransitionList.append(currentAutomaton->getTransition(i));
    DeleteCommand *deleteCommand = new DeleteCommand(deleteTransitionList, this, currentAutomaton->getId());
    connect(deleteCommand, SIGNAL(undo_deleteTransition(Transition, int)), this, SLOT(createTransition(Transition, int)));
    connect(deleteCommand, SIGNAL(redo_deleteTransition(QList<int>, int)), this, SLOT(deleteTransition(QList<int>, int)));
    undoStack->push(deleteCommand);
}

void MainWindow::deleteTransition(QList<int> deleteList, int automaton)
{
    int tmp;
    if(currentAutomaton->getId() != automaton)
    {
        for(int j = 0; j < ui->Automatons_list->rowCount(); j++)
        {
            if(ui->Automatons_list->item(j,0)->text().toInt() == automaton)
                ui->Automatons_list->setCurrentCell(j,1);
        }
    }
    for(int i : deleteList)
    {
        currentAutomaton->getTransitionList()->remove(i);
        tmp = 0;
        while (tmp < ui->Transitions_list->rowCount())
        {
            if(ui->Transitions_list->item(tmp,0)->text().toInt() == i)
            {
                ui->Transitions_list->removeRow(tmp);
                break;
            }
            else
                tmp++;
        }
    }
    if(ui->Transitions_list->rowCount() == 0)
        ui->deleteTransition_button->setEnabled(false);
}

/*
 * Detect selection of whole cell instead of just checkbox
*/
void MainWindow::on_States_list_cellClicked(int row, int column)
{
    int id;
    bool state;
    State s;
    if(column == 2)
    {
        id = ui->States_list->item(row,0)->text().toInt();
        s = currentAutomaton->getState(id);
        state = ui->States_list->item(row,column)->checkState();
        //s.setInitial(!state);
        ui->States_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
        //currentAutomaton->getStateList()->insert(id, s);
    }
    else if(column == 3)
    {
        id = ui->States_list->item(row,0)->text().toInt();
        s = currentAutomaton->getState(id);
        state = ui->States_list->item(row,column)->checkState();
        //s.setAccepting(!state);
        ui->States_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
        //currentAutomaton->getStateList()->insert(id, s);
    }
}

/*
 * Detect selection of whole cell instead of just checkbox
*/
void MainWindow::on_Events_list_cellClicked(int row, int column)
{
    int id;
    bool state;
    Event e;
    if(column == 2)
    {
        id = ui->Events_list->item(row,0)->text().toInt();
        e = currentAutomaton->getEvent(id);
        state = ui->Events_list->item(row,column)->checkState();
        e.setObservable(state);
        ui->Events_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
        currentAutomaton->getEventList()->insert(id,e);
    }
    else if(column == 3)
    {
        id = ui->Events_list->item(row,0)->text().toInt();
        e = currentAutomaton->getEvent(id);
        state = ui->Events_list->item(row,column)->checkState();
        e.setControlable(state);
        ui->Events_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
        currentAutomaton->getEventList()->insert(id,e);
    }
}

/*
 * slot for delete state button under the list
 * will call 'delete_state_finished' with only the currently selected state
*/
void MainWindow::on_deleteState_button_clicked()
{
    if(ui->States_list->currentItem() == nullptr)
        ui->States_list->setCurrentCell(0,1);
    int id = ui->States_list->item(ui->States_list->currentRow(),0)->text().toInt();
    deleteState_finished(QList<int>({id}));
}

/*
 * slot for delete event button under the list
 * will call 'delete_event_finished' with only the currently selected event
*/
void MainWindow::on_deleteEvent_button_clicked()
{
    if(ui->Events_list->currentItem() == nullptr)
        ui->Events_list->setCurrentCell(0,1);
    int id = ui->Events_list->item(ui->Events_list->currentRow(),0)->text().toInt();
    deleteEvent_finished(QList<int>({id}));
}

/*
 * slot for delete transition button under the list
 * will call 'delete_transition_finished' with only the currently selected transition
*/
void MainWindow::on_deleteTransition_button_clicked()
{
    if(ui->Transitions_list->currentItem() == nullptr)
        ui->Transitions_list->setCurrentCell(0,1);
    int id = ui->Transitions_list->item(ui->Transitions_list->currentRow(),0)->text().toInt();
    deleteTransition_finished(QList<int>({id}));
}

/*
 * slot for 'delete transition' button
 * display delete event dialog
*/
void MainWindow::on_actionAutomatonDelete_triggered()
{
    Delete_automaton_dialog dialog(*automata.get_automatons(), this);
    connect(&dialog, SIGNAL(delete_automaton(QList<int>)), this, SLOT(deleteAutomaton_finished(QList<int>)));
    dialog.exec();
}

/*
 * delete automata selected by user
 * update display and automaton group
*/
void MainWindow::deleteAutomaton_finished(QList<int> deleteList)
{
    QList<Automaton> deleteAutomatonList;
    for(int i : deleteList)
        deleteAutomatonList.append(*automata.get_automaton_at(i));
    DeleteCommand *deleteCommand = new DeleteCommand(deleteAutomatonList, this);
    connect(deleteCommand, SIGNAL(undo_deleteAutomaton(Automaton)), this, SLOT(createAutomaton(Automaton)));
    connect(deleteCommand, SIGNAL(redo_deleteAutomaton(QList<int>)), this, SLOT(deleteAutomaton(QList<int>)));
    undoStack->push(deleteCommand);
}

void MainWindow::deleteAutomaton(QList<int> deleteList)
{
    int tmp;
    int pos = ui->Automatons_list->currentRow();
    QSignalBlocker automaton_blocker(ui->Automatons_list);
    for(int i : deleteList)
    {
        automata.get_automatons()->remove(i);
        tmp = 0;
        while (tmp < ui->Automatons_list->rowCount())
        {
            if(ui->Automatons_list->item(tmp,0)->text().toInt() == i)
            {
                ui->Automatons_list->removeRow(tmp);
                break;
            }
            tmp++;
        }
    }
    automaton_blocker.unblock();
    if(ui->Automatons_list->rowCount() > 0)
    {
        if(deleteList.contains(pos))
            ui->Automatons_list->setCurrentCell(0,1);
    }
    else
    {
        ui->addEvent_button->setEnabled(false);
        ui->addState_button->setEnabled(false);
        ui->addTransition_button->setEnabled(false);
        ui->deleteAutomaton_button->setEnabled(false);
        ui->deleteEvent_button->setEnabled(false);
        ui->deleteState_button->setEnabled(false);
        ui->deleteTransition_button->setEnabled(false);
        emit(on_Automatons_list_itemSelectionChanged());
    }
}

/*
 * slot for delete transition button under the list
 * will call 'delete_automaton_finished' with only the currently selected automaton
*/
void MainWindow::on_deleteAutomaton_button_clicked()
{
    if(ui->Automatons_list->currentItem() == nullptr)
        ui->Automatons_list->setCurrentCell(0,1);
    int id = ui->Automatons_list->item(ui->Automatons_list->currentRow(),0)->text().toInt();
    deleteAutomaton_finished(QList<int>({id}));
}

/*
 * slot for 'export supremica' button
*/
void MainWindow::on_actionExportSupremica_triggered()
{
    QString file_name;
    if(automata.getFilePath().isEmpty() || automata.getType() != SUPREMICA) //if automata not from supremica file ask for new file
    {
        file_name = QFileDialog::getSaveFileName(this, tr("Select XML file"), "", tr("XML file (*.xml);;All Files (*)"));
        automata.setFilePath(file_name);
        automata.setType(SUPREMICA);
    }
    else
        file_name = automata.getFilePath();
    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::information(this, "Unable to open the file!",file.errorString());
        return;
    }
    QXmlStreamWriter *stream = new QXmlStreamWriter(&file);
    stream->setAutoFormatting(true);
    stream->writeStartDocument();
    automata.toSupremica(stream);
    stream->writeEndDocument();
    file.close();
    delete stream;
    QMessageBox::information(this, tr("Save sucessful"),"Automaton saved sucessfuly.");
}

/*
 * slot for 'export SEDMA' button
*/
void MainWindow::on_actionExportSedma_triggered()
{
    QString file_name = "";
    if((automata.getFilePath().isEmpty() || automata.getType() != SEDMA) && automata.get_automatons()->size() == 1) //if automata not from SEDMA file ask for new fill, only if only one automaton in group
    {
        file_name = QFileDialog::getSaveFileName(this, tr("Select automata file"), "", tr("Automata file (*.automata);;All Files (*)"));
        automata.setFilePath(file_name);
        automata.setType(SEDMA);
    }
    else if(!automata.getFilePath().isEmpty() && automata.getType() == SEDMA)   //if automata from SEDMA file set it as default file
        file_name = automata.getFilePath();
    automata.toSedma(file_name);    //if multiple automata and not from a SEDMA file, toSedma() will ask for a folder
    QMessageBox::information(this, tr("Save sucessful"),"Automaton saved sucessfuly.");
}

/*
 * Open/import a file containing a Supremica automaton, load it into memory and enable interface
*/
void MainWindow::on_actionImportSupremica_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open XML file"), "", tr("XML file (*.xml);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    if (automata.fromSupremica(file_name))
    {
        this->toggle_interface(true);
        undoStack->clear();
    }
}

/*
 * Open/import a file containing a SEDMA automaton, load it into memory and enable interface
*/
void MainWindow::on_actionImportSedma_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open automata file"), "", tr("Automata file (*.automata);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    if (automata.fromSedma(file_name))
    {
        this->toggle_interface(true);
        undoStack->clear();
    }
}

/*
 * slot for 'help' button
 * display the help dialog
*/
void MainWindow::on_actionHelp_triggered()
{
    Help_dialog dialog(this);
    dialog.exec();
}

/*
 * Open/import a file containing a DESUMA automaton, load it into memory and enable interface
*/
void MainWindow::on_actionImportDESUMA_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open XML file"), "", tr("XML file (*.xml);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    if (automata.fromDesuma(file_name))
    {
        this->toggle_interface(true);
        undoStack->clear();
    }
}

/*
 * slot for 'export DESUMA' button
*/
void MainWindow::on_actionExportDESUMA_triggered()
{
    QString file_name = "";
    if((automata.getFilePath().isEmpty() || automata.getType() != DESUMA) && automata.get_automatons()->size() == 1) //if automata not from DESUMA file ask for new fill, only if only one automaton in group
    {
        file_name = QFileDialog::getSaveFileName(this, tr("Select XML file"), "", tr("XML file (*.xml);;All Files (*)"));
        automata.setFilePath(file_name);
        automata.setType(DESUMA);
    }
    else if(!automata.getFilePath().isEmpty() && automata.getType() == DESUMA)   //if automata from DESUMA file set it as default file
        file_name = automata.getFilePath();
    automata.toDesuma(file_name);
    QMessageBox::information(this, tr("Save sucessful"),"Automaton saved sucessfuly.");
}

/*
 * slot for 'save automata as...'
 * empty path in memory and call save function
*/
void MainWindow::on_actionSave_automatons_as_triggered()
{
    automata.setFilePath("");
    emit(on_actionSaveAutomaton_triggered());
}

/*
 * slot for 'save regular language as...'
 * empty path in memory and call save function
*/
void MainWindow::on_actionSaveRL_as_triggered()
{
    automata.setRlFilePath("");
    emit(on_actionSaveRL_triggered());
}
