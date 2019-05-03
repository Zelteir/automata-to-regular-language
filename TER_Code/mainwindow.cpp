#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "type_choice.hpp"
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Automatons_list->hideColumn(0);
    ui->Automatons_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Events_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Events_list->hideColumn(0);
    ui->States_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->States_list->hideColumn(0);
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch );
    ui->Transitions_list->hideColumn(0);
    method = new QActionGroup(this);
    ui->actionBrzozowski->setActionGroup(method);
    ui->actionBrzozowski_V2->setActionGroup(method);
    ui->actionReverse_Brzozowski->setActionGroup(method);
    method->setExclusive(true);
}

MainWindow::~MainWindow()
{
    delete method;
    delete ui;
}

/*
 * Open/import a file containing an automaton
 * TO DO
 * Call Type_choice window to choose what kind of file to import
 * bypassed for now : only Supremica XML files accepted
*/
void MainWindow::on_actionOpen_Import_triggered()
{
    /*Type_choice window_choice;
    window_choice.show();*/
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open automaton file"), "", tr("XML file (*.xml);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    if (automata.fromSupremica(file_name))
    {
        this->toggle_interface(true);
    }
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
    int pos = ui->States_list->rowCount();
    ui->States_list->insertRow(pos);
    //add id and name
    ui->States_list->setItem(pos,0,new QTableWidgetItem(QString::number(s.getId())));
    ui->States_list->setItem(pos,1,new QTableWidgetItem(s.getName()));
    ui->States_list->item(pos,1)->setTextAlignment(Qt::AlignHCenter);
    //access boolean value and set checkbox state accordingly, flags prevent edition of checkbox text
    ui->States_list->setItem(pos,2,new Table_Widget_Checkbox_Item(""));
    if(s.getInitial())
    {
        ui->States_list->item(pos,2)->setCheckState(Qt::Checked);
    }
    else {
        ui->States_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->States_list->item(pos,2)->setFlags(ui->States_list->item(pos,2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
    ui->States_list->setItem(pos,3,new Table_Widget_Checkbox_Item(""));
    if(s.getAccepting())
    {
        ui->States_list->item(pos,3)->setCheckState(Qt::Checked);
    }
    else {
        ui->States_list->item(pos,3)->setCheckState(Qt::Unchecked);
    }
    ui->States_list->item(pos,3)->setFlags(ui->States_list->item(pos,3)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
}

/*
 * Function to add an event to the interface list of event
*/
void MainWindow::add_event_to_list(Event e)
{
    int pos = ui->Events_list->rowCount();
    ui->Events_list->insertRow(pos);
    //add Id and label
    ui->Events_list->setItem(pos,0,new QTableWidgetItem(QString::number(e.getId())));
    ui->Events_list->setItem(pos,1,new QTableWidgetItem(e.getLabel()));
    ui->Events_list->item(pos,1)->setTextAlignment(Qt::AlignHCenter);
    //access boolean value and set checkbox state accordingly, flags prevent edition of checkbox text
    ui->Events_list->setItem(pos,2,new Table_Widget_Checkbox_Item(""));
    if(!e.getObservable())
    {
        ui->Events_list->item(pos,2)->setCheckState(Qt::Checked);
    }
    else
    {
        ui->Events_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->Events_list->item(pos,2)->setFlags(ui->Events_list->item(pos,2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
    ui->Events_list->setItem(pos,3,new Table_Widget_Checkbox_Item(""));
    if(!e.getControlable())
    {
        ui->Events_list->item(pos,3)->setCheckState(Qt::Checked);
    }
    else
    {
        ui->Events_list->item(pos,3)->setCheckState(Qt::Unchecked);
    }
    ui->Events_list->item(pos,3)->setFlags(ui->Events_list->item(pos,3)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
}

/*
 * Function to add a transition to the interface list of transition
*/
void MainWindow::add_transition_to_list(Transition t)
{
    int pos = ui->Transitions_list->rowCount();
    ui->Transitions_list->insertRow(pos);

    ui->Transitions_list->setItem(pos,0, new QTableWidgetItem(QString::number(t.getId())));
    ui->Transitions_list->setItem(pos,1, new QTableWidgetItem(currentAutomaton->getState(t.getSource()).getName()));
    ui->Transitions_list->setItem(pos,2, new QTableWidgetItem(currentAutomaton->getState(t.getDest()).getName()));
    ui->Transitions_list->setItem(pos,3, new QTableWidgetItem(currentAutomaton->getEvent(t.getEvent()).getLabel()));
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
            ui->actionSaveRL->setEnabled(false);
        else
            ui->actionSaveRL->setEnabled(true);
        fill_interface();
    }
}

void MainWindow::generateLanguage(Automaton *a)
{
    int nb_init = 0;
    bool accept = false;
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
    if(ui->actionAvoid_language_ambiguity->isChecked())
    {
        QMap<int, Event> tmpEventList;
        for(Event e : *a->getEventList())
        {
            e.setLabel(e.getLabel()+".");
            //tmpEventList.append(e);
            tmpEventList.insert(e.getId(),e);
        }
        a->setEventList(tmpEventList);
    }
    if(ui->actionBrzozowski->isChecked())
    {
        translator.brzozowskiMethod(*a, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    }
    else if(ui->actionBrzozowski_V2->isChecked())
    {
        translator.brzozowskiMethodV2(*a, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    }
    else if(ui->actionReverse_Brzozowski->isChecked())
    {
        translator.reverseBrzozowski(*a, ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    }

    if(ui->actionAvoid_language_ambiguity->isChecked())
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
}

/*
 * clear interface and disable it
 * */
void MainWindow::on_actionClose_triggered()
{

    toggle_interface(false);
    clear_interface();
    clear_automaton_list();
    ui->States_list->setRowCount(0);
    ui->Events_list->setRowCount(0);
    ui->Transitions_list->setRowCount(0);
    ui->actionSaveRL->setEnabled(false);
}

void MainWindow::on_Automatons_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker automatons_blocker(ui->Automatons_list);
    Automaton a = *currentAutomaton;
    QString old = a.getName();
    for (int i = 0;i < ui->Automatons_list->rowCount();i++) {
        if(ui->Automatons_list->item(i,1)->text() == item->text() && i != ui->Automatons_list->currentRow())
        {
            QMessageBox::information(this, tr("Error"),
            QString("This name is already in use."));
            item->setText(old);
            automatons_blocker.unblock();
            return;
        }
    }
    currentAutomaton->setName(item->text());
    automatons_blocker.unblock();
}


/*
 * whenever the user change an element in the state list, modification on the automaton
*/
void MainWindow::on_States_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker states_blocker(ui->States_list);
    QSignalBlocker transition_blocker(ui->Transitions_list);
    State s = currentAutomaton->getState(ui->States_list->item(item->row(),0)->text().toInt());
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
                transition_blocker.unblock();
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
                transition_blocker.unblock();
                return;
            }
        }
        try {
            //set the name
            s.setName(item->text());
            currentAutomaton->getStateList()->insert(s.getId(),s);
        } catch (SetterException &ex) {
            QMessageBox::information(this, tr("Error"),
                                     ex.getMsg());
            item->setText(currentAutomaton->getState(s.getId()).getName());
            return;
        }
        //modify all transition using the old name
        for(int i = 0; i < ui->Transitions_list->rowCount();i++)
        {
            if(ui->Transitions_list->item(i,1)->text() == old)
            {
                ui->Transitions_list->item(i,1)->setText(item->text());
            }
            if(ui->Transitions_list->item(i,2)->text() == old)
            {
                ui->Transitions_list->item(i,2)->setText(item->text());
            }
        }
    }
    transition_blocker.unblock();
    states_blocker.unblock();
}

/*
 * whenever the user change an element in the event list, modification on the automaton
*/
void MainWindow::on_Events_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker events_blocker(ui->Events_list);
    QSignalBlocker transition_blocker(ui->Transitions_list);
    Event e = currentAutomaton->getEvent(ui->Events_list->item(item->row(),0)->text().toInt());
    QString old = e.getLabel();
    if (item->column() == 1) {
    //if the label was changed
    //check if the name isn't already in use  as state or event
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,1)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(old);
                transition_blocker.unblock();
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
                transition_blocker.unblock();
                events_blocker.unblock();
                return;
            }
        }
        //set label
        try {
            e.setLabel(item->text());
            currentAutomaton->getEventList()->insert(e.getId(),e);
        } catch (SetterException &ex) {
            QMessageBox::information(this, tr("Error"),
            ex.getMsg());
            item->setText(currentAutomaton->getEvent(e.getId()).getLabel());
            return;
        }
        //modify transition using the old name
        for(int i = 0; i < ui->Transitions_list->rowCount();i++)
        {
            if(ui->Transitions_list->item(i,3)->text() == old)
            {
                ui->Transitions_list->item(i,3)->setText(item->text());
            }
        }
    }
    transition_blocker.unblock();
    events_blocker.unblock();
}

/*
 * whenever the user lmodify an element in the transition list, modification of the automaton
 */
void MainWindow::on_Transitions_list_itemChanged(QTableWidgetItem *item)
{
    QSignalBlocker transition_blocker(ui->Transitions_list);
    int s = -1;
    int d = -1;
    int e = -1;
    int i = 0;
    Transition t = currentAutomaton->getTransition(ui->Transitions_list->item(item->row(),0)->text().toInt());
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
        t.setSource(s);
        for(Transition tmp : *currentAutomaton->getTransitionList())
        {
            if(t == tmp)
            {
                QMessageBox::information(this, tr("Error"),
                QString("This transition already exist."));
                item->setText(currentAutomaton->getState(currentAutomaton->getTransition(t.getId()).getSource()).getName());
                return;
            }
        }
        currentAutomaton->getTransitionList()->insert(t.getId(),t);
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
        t.setDest(d);
        for(Transition tmp : *currentAutomaton->getTransitionList())
        {
            if(t == tmp)
            {
                QMessageBox::information(this, tr("Error"),
                QString("This transition already exist."));
                item->setText(currentAutomaton->getState(currentAutomaton->getTransition(t.getId()).getDest()).getName());
                return;
            }
        }
        currentAutomaton->getTransitionList()->insert(t.getId(),t);
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
            item->setText(currentAutomaton->getEvent(currentAutomaton->getTransition(t.getId()).getEvent()).getLabel());
            return;
        }
        t.setEvent(e);
        for(Transition tmp : *currentAutomaton->getTransitionList())
        {
            if(t == tmp)
            {
                QMessageBox::information(this, tr("Error"),
                QString("This transition already exist."));
                item->setText(currentAutomaton->getEvent(currentAutomaton->getTransition(t.getId()).getEvent()).getLabel());
                return;
            }
        }
        currentAutomaton->getTransitionList()->insert(t.getId(),t);
        break;
    }
    transition_blocker.unblock();
}

void MainWindow::on_actionAutomatonCreate_triggered()
{
    Create_automaton_dialog dialog(automata.getIdAutomaton(), *automata.get_automatons(), this);
    connect(&dialog, SIGNAL(creation_automaton(Automaton)), this, SLOT(createAutomaton_finished(Automaton)));
    dialog.exec();
}

void MainWindow::createAutomaton_finished(Automaton a)
{
    QSignalBlocker automaton_blocker(ui->Automatons_list);
    automata.get_automatons()->insert(a.getId(),a);
    automata.idAutomatonIncr();
    ui->Automatons_list->insertRow(ui->Automatons_list->rowCount());
    ui->Automatons_list->setItem(ui->Automatons_list->rowCount() - 1, 0, new QTableWidgetItem(QString::number(a.getId())));
    ui->Automatons_list->setItem(ui->Automatons_list->rowCount() - 1, 1, new QTableWidgetItem(a.getName()));
    automaton_blocker.unblock();
    if(ui->Automatons_list->rowCount() == 1)
        ui->Automatons_list->setCurrentCell(0,1);
}

/*
 * whenever the user create a state
 */
void MainWindow::on_actionStateCreate_triggered()
{
    Create_state_dialog dialog(currentAutomaton->getIdState(), *currentAutomaton->getEventList(),*currentAutomaton->getStateList(), this);
    connect(&dialog, SIGNAL(creation_state(State)), this, SLOT(createState_finished(State)));
    dialog.exec();
}

/*
 * add the created state into the list and interface
 */
void MainWindow::createState_finished(State s)
{
    QSignalBlocker states_blocker(ui->States_list);
    currentAutomaton->getStateList()->insert(s.getId(),s);
    currentAutomaton->incrState();
    add_state_to_list(s);
    states_blocker.unblock();
    if(ui->States_list->rowCount() == 1)
        ui->deleteState_button->setEnabled(true);
}

void MainWindow::on_actionEventCreate_triggered()
{
    Create_event_dialog dialog(currentAutomaton->getIdEvent(), *currentAutomaton->getEventList(),*currentAutomaton->getStateList(), this);
    connect(&dialog, SIGNAL(creation_event(Event)), this, SLOT(createEvent_finished(Event)));
    dialog.exec();
}

void MainWindow::createEvent_finished(Event e)
{
    QSignalBlocker events_blocker(ui->Events_list);
    currentAutomaton->getEventList()->insert(e.getId(),e);
    currentAutomaton->incrEvent();
    add_event_to_list(e);
    events_blocker.unblock();
    if(ui->Events_list->rowCount() == 1)
        ui->deleteEvent_button->setEnabled(true);
}

void MainWindow::on_actionTransitionCreate_triggered()
{
    Create_transition_dialog *dialog = new Create_transition_dialog(currentAutomaton->getIdTransition(),*currentAutomaton->getEventList(),*currentAutomaton->getStateList(),*currentAutomaton->getTransitionList(), this);
    connect(dialog, SIGNAL(creation_transition(Transition)), this, SLOT(createTransition_finished(Transition)));
    dialog->exec();
    delete dialog;
}

void MainWindow::createTransition_finished(Transition t)
{
    QSignalBlocker transition_blocker(ui->Transitions_list);
    currentAutomaton->getTransitionList()->insert(t.getId(),t);
    currentAutomaton->incrTransition();
    add_transition_to_list(t);
    transition_blocker.unblock();
    if(ui->Transitions_list->rowCount() == 1)
        ui->deleteTransition_button->setEnabled(true);
}

void MainWindow::on_actionSaveAutomaton_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Select XML file"), "", tr("XML file (*.xml);;All Files (*)"));
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

void MainWindow::on_actionSaveRL_triggered()
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
    out << automata.get_automaton_at(ui->Automatons_list->currentRow());
    out << " : ";
    out << ui->Generated_Regular_Language->toPlainText();
    out << "\n";
    file.close();
    QMessageBox::information(this, tr("Save sucessful"),"Regular expression saved sucessfuly.");
}

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

void MainWindow::on_actionStateDelete_triggered()
{
    Delete_state_dialog dialog(*currentAutomaton->getStateList(), this);
    connect(&dialog, SIGNAL(delete_state(QList<int>)), this, SLOT(deleteState_finished(QList<int>)));
    dialog.exec();
}

void MainWindow::deleteState_finished(QList<int> deleteList)
{
    int tmp;
    QString tmpString;
    QList<int> delTransitionList;
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

void MainWindow::on_actionEventDelete_triggered()
{
    Delete_event_dialog dialog(*currentAutomaton->getEventList(), this);
    connect(&dialog, SIGNAL(delete_event(QList<int>)), this, SLOT(deleteEvent_finished(QList<int>)));
    dialog.exec();
}

void MainWindow::deleteEvent_finished(QList<int> deleteList)
{
    int tmp;
    QString tmpString;
    QList<int> delTransitionList;
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

/*Calls the delete_transition_dialog*/
void MainWindow::on_actionTransitionDelete_triggered()
{
    Delete_transition_dialog dialog(*currentAutomaton->getEventList(), *currentAutomaton->getStateList(), *currentAutomaton->getTransitionList(), this);
    connect(&dialog, SIGNAL(delete_transition(QList<int>)), this, SLOT(deleteTransition_finished(QList<int>)));
    dialog.exec();
}

/*Delete the transitions selected by user*/
void MainWindow::deleteTransition_finished(QList<int> deleteList)
{
    int tmp;
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
        s.setInitial(!state);
        ui->States_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
        currentAutomaton->getStateList()->insert(id, s);
    }
    else if(column == 3)
    {
        id = ui->States_list->item(row,0)->text().toInt();
        s = currentAutomaton->getState(id);
        state = ui->States_list->item(row,column)->checkState();
        s.setAccepting(!state);
        ui->States_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
        currentAutomaton->getStateList()->insert(id, s);
    }
}

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

void MainWindow::on_deleteState_button_clicked()
{
    if(ui->States_list->currentItem() == NULL)
        ui->States_list->setCurrentCell(0,1);
    int id = ui->States_list->item(ui->States_list->currentRow(),0)->text().toInt();
    deleteState_finished(QList<int>({id}));
}

void MainWindow::on_deleteEvent_button_clicked()
{
    if(ui->Events_list->currentItem() == NULL)
        ui->Events_list->setCurrentCell(0,1);
    int id = ui->Events_list->item(ui->Events_list->currentRow(),0)->text().toInt();
    deleteEvent_finished(QList<int>({id}));
}

void MainWindow::on_deleteTransition_button_clicked()
{
    if(ui->Transitions_list->currentItem() == NULL)
        ui->Transitions_list->setCurrentCell(0,1);
    int id = ui->Transitions_list->item(ui->Transitions_list->currentRow(),0)->text().toInt();
    deleteTransition_finished(QList<int>({id}));
}

/*TO DO*/
void MainWindow::on_actionAutomatonDelete_triggered()
{
    Delete_automaton_dialog dialog(*automata.get_automatons(), this);
    connect(&dialog, SIGNAL(delete_automaton(QList<int>)), this, SLOT(deleteAutomaton_finished(QList<int>)));
    dialog.exec();
}

void MainWindow::deleteAutomaton_finished(QList<int> deleteList)
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

void MainWindow::on_deleteAutomaton_button_clicked()
{
    if(ui->Automatons_list->currentItem() == NULL)
        ui->Automatons_list->setCurrentCell(0,1);
    int id = ui->Automatons_list->item(ui->Automatons_list->currentRow(),0)->text().toInt();
    deleteAutomaton_finished(QList<int>({id}));
}
