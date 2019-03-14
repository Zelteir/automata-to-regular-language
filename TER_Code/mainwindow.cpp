#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "type_choice.hpp"
#include <QFileDialog>
#include <QDebug>
#include <QSignalBlocker>
#include "create_state_dialog.hpp"
#include "create_event_dialog.hpp"

TableWidgetCheckboxItem::TableWidgetCheckboxItem(const QString &text, int type) : QTableWidgetItem(text, type){

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Events_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Events_list->hideColumn(0);
    ui->States_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->States_list->hideColumn(0);
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch );
    ui->Transitions_list->hideColumn(0);
}

MainWindow::~MainWindow()
{
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
    ui->Ignore_Unobservable_check->setEnabled(b);
    ui->Ignore_Uncontrolable_check->setEnabled(b);
    /*
     * TO DO
     * Other things to toggle
    */
    this->fill_automaton_list();
}

void MainWindow::add_state_to_list(State s)
{
    int pos = ui->States_list->rowCount();
    ui->States_list->insertRow(pos);
    qDebug() << "id " << s.getId();
    qDebug() << "pos" << pos;

    ui->States_list->setItem(pos,0,new QTableWidgetItem(QString::number(s.getId())));
    ui->States_list->setItem(pos,1,new QTableWidgetItem(s.getName()));
    ui->States_list->item(pos,1)->setTextAlignment(Qt::AlignHCenter);
    ui->States_list->setItem(pos,2,new TableWidgetCheckboxItem(""));
    if(s.getInitial())
    {
        ui->States_list->item(pos,2)->setCheckState(Qt::Checked);
    }
    else {
        ui->States_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->States_list->item(pos,2)->setFlags(ui->States_list->item(pos,2)->flags() & (~Qt::ItemIsEditable));
    ui->States_list->setItem(pos,3,new TableWidgetCheckboxItem(""));
    if(s.getAccepting())
    {
        ui->States_list->item(pos,3)->setCheckState(Qt::Checked);
    }
    else {
        ui->States_list->item(pos,3)->setCheckState(Qt::Unchecked);
    }
    ui->States_list->item(pos,3)->setFlags(ui->States_list->item(pos,3)->flags() & (~Qt::ItemIsEditable));
}

void MainWindow::add_event_to_list(Event e)
{
    int pos = ui->Events_list->rowCount();
    ui->Events_list->insertRow(pos);

    ui->Events_list->setItem(pos,0,new QTableWidgetItem(QString::number(e.getId())));
    ui->Events_list->setItem(pos,1,new QTableWidgetItem(e.getLabel()));
    ui->Events_list->item(pos,1)->setTextAlignment(Qt::AlignHCenter);

    ui->Events_list->setItem(pos,2,new TableWidgetCheckboxItem(""));
    if(!e.getObservable())
    {
        ui->Events_list->item(pos,2)->setCheckState(Qt::Checked);
    }
    else
    {
        ui->Events_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->Events_list->item(pos,2)->setFlags(ui->Events_list->item(pos,2)->flags() & (~Qt::ItemIsEditable));
    ui->Events_list->setItem(pos,3,new TableWidgetCheckboxItem(""));
    if(!e.getControlable())
    {
        ui->Events_list->item(pos,3)->setCheckState(Qt::Checked);
    }
    else
    {
        ui->Events_list->item(pos,3)->setCheckState(Qt::Unchecked);
    }
    ui->Events_list->item(pos,3)->setFlags(ui->Events_list->item(pos,3)->flags() & (~Qt::ItemIsEditable));
}

void MainWindow::add_transition_to_list(Transition t)
{
    int pos = ui->Transitions_list->rowCount();
    ui->Transitions_list->insertRow(pos);

    ui->Transitions_list->setItem(pos,0, new QTableWidgetItem(QString::number(t.getId())));
    ui->Transitions_list->setItem(pos,1, new QTableWidgetItem(*ui->States_list->item(t.getSource(),1)));
    ui->Transitions_list->setItem(pos,2, new QTableWidgetItem(*ui->States_list->item(t.getDest(),1)));
    ui->Transitions_list->setItem(pos,3, new QTableWidgetItem(*ui->Events_list->item(t.getEvent(),1)));
}

/*
 * Fill the interface with the information contained in the current automaton
*/
void MainWindow::fill_interface()
{
    /*BLock signals to avoid calling 'on_item_changed' functions*/
    QSignalBlocker states_blocker(ui->States_list);
    QSignalBlocker events_blocker(ui->Events_list);
    QSignalBlocker transitions_blocker(ui->Transitions_list);
    clear_interface();
    int id = ui->Automatons_list->currentRow();
    /*fill states table with name, initial and accepted information*/
    for(State s: *(automata.get_automaton_at(id)->getStateList()))
    {
        add_state_to_list(s);
    }
    /*fill events table with name, observable and controlable information*/
    for(Event e : *(automata.get_automaton_at(id)->getEventList()))
    {
        add_event_to_list(e);
    }
    /*fill transition table with origin, destination and event information*/
    for(Transition t : *(automata.get_automaton_at(id)->getTransitionList()))
    {
        add_transition_to_list(t);
    }
    states_blocker.unblock();
    events_blocker.unblock();
    transitions_blocker.unblock();
}

/*
 * Fill the list of automaton wontained inside the automata
*/
void MainWindow::fill_automaton_list()
{
    clear_automaton_list();
    for(Automaton tmp : *this->automata.get_automatons())
        ui->Automatons_list->addItem(tmp.getName());
    ui->Automatons_list->setCurrentRow(0);
}

/*
 * Clear interface of information
*/
void MainWindow::clear_interface()
{
    ui->Events_list->clearContents();
    ui->Events_list->setRowCount(0);
    ui->States_list->clearContents();
    ui->States_list->setRowCount(0);
    ui->Transitions_list->clearContents();
    ui->Transitions_list->setRowCount(0);
    ui->Generated_Regular_Language->clear();
}

/*
 * Clear the automaton list
*/
void MainWindow::clear_automaton_list()
{
    ui->Automatons_list->clear();
}

/*
 * Refresh display when user select another automaton
*/
void MainWindow::on_Automatons_list_itemSelectionChanged()
{
    fill_interface();
}

/*
 * Send the current automaton's information to the translator and generate the regular language assossiated
*/
void MainWindow::on_Generate_Button_clicked()
{
    int nb_init = 0;
    bool accept = false;
    int id = ui->Automatons_list->currentRow();
    //contain exactly one initial state
    //contain at least one eccepting state
    for(State s : *automata.get_automaton_at(id)->getStateList())
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
    translator.brzozowskiMethod(*automata.get_automaton_at(ui->Automatons_list->currentRow()), ui->Ignore_Unobservable_check->isChecked(), ui->Ignore_Uncontrolable_check->isChecked());
    ui->Generated_Regular_Language->setPlainText(translator.getRegex());
    ui->actionSave_as->setEnabled(true);
}

void MainWindow::on_actionClose_triggered()
{

    toggle_interface(false);
    clear_interface();
    clear_automaton_list();
    ui->States_list->setRowCount(0);
    ui->Events_list->setRowCount(0);
    ui->Transitions_list->setRowCount(0);
    ui->actionSave_as->setEnabled(false);
}

void MainWindow::on_actionSave_as_triggered()
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

void MainWindow::on_States_list_itemChanged(QTableWidgetItem *item)
{
    int id = ui->Automatons_list->currentRow();
    State s = automata.get_automaton_at(id)->getState(ui->States_list->item(item->row(),0)->text().toInt());
    QString old = s.getName();
    switch (item->column()) {
    case 1:
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,1)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(automata.get_automaton_at(id)->getState(s.getId()).getName());
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,1)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(automata.get_automaton_at(id)->getState(s.getId()).getName());
                return;
            }
        }
        try {

            s.setName(item->text());
            automata.get_automaton_at(id)->getStateList()->replace(s.getId(),s);
        } catch (SetterException &ex) {
            QMessageBox::information(this, tr("Error"),
            ex.getMsg());
            item->setText(automata.get_automaton_at(id)->getState(s.getId()).getName());
            return;
        }
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
        break;
    case 2:
        s.setInitial(item->checkState()==Qt::Checked?true:false);
        automata.get_automaton_at(id)->getStateList()->replace(s.getId(),s);
        break;
    case 3:
        s.setAccepting(item->checkState()==Qt::Checked?true:false);
        automata.get_automaton_at(id)->getStateList()->replace(s.getId(),s);
        break;
    }
}

void MainWindow::on_Events_list_itemChanged(QTableWidgetItem *item)
{
    int id = ui->Automatons_list->currentRow();
    Event e = automata.get_automaton_at(id)->getEvent(ui->Events_list->item(item->row(),0)->text().toInt());
    QString old = e.getLabel();
    switch (item->column()) {
    case 1:
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,1)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(automata.get_automaton_at(id)->getEvent(e.getId()).getLabel());
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,1)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use."));
                item->setText(automata.get_automaton_at(id)->getEvent(e.getId()).getLabel());
                return;
            }
        }
        try {
            e.setLabel(item->text());
            automata.get_automaton_at(id)->getEventList()->replace(e.getId(),e);
        } catch (SetterException &ex) {
            QMessageBox::information(this, tr("Error"),
            ex.getMsg());
            item->setText(automata.get_automaton_at(id)->getEvent(e.getId()).getLabel());
            return;
        }
        for(int i = 0; i < ui->Transitions_list->rowCount();i++)
        {
            if(ui->Transitions_list->item(i,3)->text() == old)
            {
                ui->Transitions_list->item(i,3)->setText(item->text());
            }
        }
        break;
    case 2:
        e.setObservable(item->checkState()==Qt::Checked?true:false);
        automata.get_automaton_at(id)->getEventList()->replace(e.getId(),e);
        break;
    case 3:
        e.setControlable(item->checkState()==Qt::Checked?true:false);
        automata.get_automaton_at(id)->getEventList()->replace(e.getId(),e);
        break;
    }
}

void MainWindow::on_Transitions_list_itemChanged(QTableWidgetItem *item)
{
    int id = ui->Automatons_list->currentRow();
    int s = -1;
    int d = -1;
    int e = -1;
    int i = 0;
    Transition t = automata.get_automaton_at(id)->getTransition(ui->Transitions_list->item(item->row(),0)->text().toInt());
    switch (item->column()) {
    case 1:
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
            item->setText(automata.get_automaton_at(id)->getState(automata.get_automaton_at(id)->getTransition(t.getId()).getSource()).getName());
            return;
        }
        t.setSource(s);
        automata.get_automaton_at(id)->getTransitionList()->replace(t.getId(),t);
        break;
    case 2:
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
            item->setText(automata.get_automaton_at(id)->getState(automata.get_automaton_at(id)->getTransition(t.getId()).getDest()).getName());
            return;
        }
        t.setDest(d);
        automata.get_automaton_at(id)->getTransitionList()->replace(t.getId(),t);
        break;
    case 3:
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
            item->setText(automata.get_automaton_at(id)->getEvent(automata.get_automaton_at(id)->getTransition(t.getId()).getEvent()).getLabel());
            return;
        }
        t.setEvent(e);
        automata.get_automaton_at(id)->getTransitionList()->replace(t.getId(),t);
        break;
    }
}

void MainWindow::on_actionAutomatonCreate_triggered()
{
    /*TO DO*/
}

void MainWindow::createAutomaton_finished(Automaton a)
{
    /*TO DO*/
}

void MainWindow::on_actionStateCreate_triggered()
{
    int id = ui->Automatons_list->currentRow();
    Create_state_dialog dialog(*automata.get_automaton_at(id)->getEventList(),*automata.get_automaton_at(id)->getStateList(), this);
    connect(&dialog, SIGNAL(creation_state(State)), this, SLOT(createState_finished(State)));
    dialog.exec();
}

void MainWindow::createState_finished(State s)
{
    QSignalBlocker states_blocker(ui->States_list);
    automata.get_automaton_at(ui->Automatons_list->currentRow())->getStateList()->append(s);
    add_state_to_list(s);
    states_blocker.unblock();
}

void MainWindow::on_actionEventCreate_triggered()
{
    int id = ui->Automatons_list->currentRow();
    Create_event_dialog dialog(*automata.get_automaton_at(id)->getEventList(),*automata.get_automaton_at(id)->getStateList(), this);
    connect(&dialog, SIGNAL(creation_event(Event)), this, SLOT(createEvent_finished(Event)));
    dialog.exec();
}

void MainWindow::createEvent_finished(Event e)
{
    QSignalBlocker events_blocker(ui->Events_list);
    automata.get_automaton_at(ui->Automatons_list->currentRow())->getEventList()->append(e);
    add_event_to_list(e);
    events_blocker.unblock();
}

void MainWindow::on_actionTransitionCreate_triggered()
{
    /*TO DO*/
}

void MainWindow::createTransition_finished(Transition t)
{
    /*TO DO*/
}
