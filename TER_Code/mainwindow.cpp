#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "type_choice.hpp"
#include <QFileDialog>
#include <QDebug>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Events_list->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch );
    ui->States_list->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch );
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
 * Enable the interface to user
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
    /*
     * TO DO
     * Other things to enable
    */
    this->fill_automaton_list();
}

/*
 * Fill the interface with the information contained in the current automaton
*/
void MainWindow::fill_interface()
{
    QSignalBlocker states_blocker(ui->States_list);
    QSignalBlocker events_blocker(ui->Events_list);
    /*TO DO disconnect slots*/
    clear_interface();
    QString string;
    int i;
    int id = ui->Automatons_list->currentRow();
    ui->States_list->setRowCount(automata.get_automaton_at(id)->getStateList()->length());
    /*fill states table with name, initial and accepted information*/
    for(i = 0; i< automata.get_automaton_at(id)->getStateList()->length();i++)
    {
        ui->States_list->setItem(i,0,new QTableWidgetItem((*automata.get_automaton_at(id)->getStateList())[i].getName()));
        ui->States_list->item(i,0)->setTextAlignment(Qt::AlignHCenter);
        if((*automata.get_automaton_at(id)->getStateList())[i].getInitial())
        {
            ui->States_list->setItem(i,1,new QTableWidgetItem(""));
            ui->States_list->item(i,1)->setCheckState(Qt::Checked);
        }
        else {
            ui->States_list->setItem(i,1,new QTableWidgetItem(""));
            ui->States_list->item(i,1)->setCheckState(Qt::Unchecked);
        }
        ui->States_list->item(i,1)->setFlags(ui->States_list->item(i,1)->flags() & (~Qt::ItemIsEditable));
        if((*automata.get_automaton_at(id)->getStateList())[i].getAccepting())
        {
            ui->States_list->setItem(i,2,new QTableWidgetItem(""));
            ui->States_list->item(i,2)->setCheckState(Qt::Checked);
        }
        else {
            ui->States_list->setItem(i,2,new QTableWidgetItem(""));
            ui->States_list->item(i,2)->setCheckState(Qt::Unchecked);
        }
        ui->States_list->item(i,2)->setFlags(ui->States_list->item(i,1)->flags() & (~Qt::ItemIsEditable));
    }
    ui->Events_list->setRowCount(automata.get_automaton_at(id)->getEventList()->length());
    /*fill events table with name, observable and controlable information*/
    for(i = 0; i< automata.get_automaton_at(id)->getEventList()->length();i++)
    {
        ui->Events_list->setItem(i,0,new QTableWidgetItem((*automata.get_automaton_at(id)->getEventList())[i].getLabel()));
        ui->Events_list->item(i,0)->setTextAlignment(Qt::AlignHCenter);
        if(!(*automata.get_automaton_at(id)->getEventList())[i].getObservable())
        {
            ui->Events_list->setItem(i,1,new QTableWidgetItem(""));
            ui->Events_list->item(i,1)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->Events_list->setItem(i,1,new QTableWidgetItem(""));
            ui->Events_list->item(i,1)->setCheckState(Qt::Unchecked);
        }
        ui->Events_list->item(i,1)->setFlags(ui->Events_list->item(i,1)->flags() & (~Qt::ItemIsEditable));
        if(!(*automata.get_automaton_at(id)->getEventList())[i].getControlable())
        {
            ui->Events_list->setItem(i,2,new QTableWidgetItem(""));
            ui->Events_list->item(i,2)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->Events_list->setItem(i,2,new QTableWidgetItem(""));
            ui->Events_list->item(i,2)->setCheckState(Qt::Unchecked);
        }
        ui->Events_list->item(i,2)->setFlags(ui->Events_list->item(i,1)->flags() & (~Qt::ItemIsEditable));
    }
    ui->Transitions_list->setRowCount(automata.get_automaton_at(id)->getTransitionList()->length());
    /*fill transition table with origin, destination and event information*/
    for(i = 0; i< automata.get_automaton_at(id)->getTransitionList()->length();i++)
    {
        ui->Transitions_list->setItem(i,0, new QTableWidgetItem(*ui->States_list->item((*automata.get_automaton_at(id)->getTransitionList())[i].getSource(),0)));
        ui->Transitions_list->setItem(i,1, new QTableWidgetItem(*ui->States_list->item((*automata.get_automaton_at(id)->getTransitionList())[i].getDest(),0)));
        ui->Transitions_list->setItem(i,2, new QTableWidgetItem(*ui->Events_list->item((*automata.get_automaton_at(id)->getTransitionList())[i].getEvent(),0)));
    }
    states_blocker.unblock();
    events_blocker.unblock();
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
    ui->States_list->clearContents();
    ui->Transitions_list->clearContents();
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
    translator.brzozowskiMethod(*automata.get_automaton_at(ui->Automatons_list->currentRow()));
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
    out << ui->Generated_Regular_Language->toPlainText();
    file.close();
    QMessageBox::information(this, tr("Save sucessful"),"Regular expression saved sucessfuly.");
}

void MainWindow::on_States_list_itemChanged(QTableWidgetItem *item)
{
    int id = ui->Automatons_list->currentRow();
    switch (item->column()) {
    case 0:
        qDebug() <<"setName";
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,0)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use"));
                item->setText(automata.get_automaton_at(id)->getState(item->row()).getName());
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,0)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use"));
                item->setText(automata.get_automaton_at(id)->getState(item->row()).getName());
                return;
            }
        }
        try {
            State s = automata.get_automaton_at(id)->getState(item->row());
            s.setName(item->text());
            automata.get_automaton_at(id)->getStateList()->replace(item->row(),s);
        } catch (SetterException &e) {
            QMessageBox::information(this, tr("Error"),
            e.getMsg());
            item->setText(automata.get_automaton_at(id)->getState(item->row()).getName());
            return;
        }
        break;
    case 1:
        automata.get_automaton_at(id)->getState(item->row()).setInitial(item->checkState()==Qt::Checked?true:false);
        break;
    case 2:
        automata.get_automaton_at(id)->getState(item->row()).setAccepting(item->checkState()==Qt::Checked?true:false);
        break;
    }
}

void MainWindow::on_Events_list_itemChanged(QTableWidgetItem *item)
{
    int id = ui->Automatons_list->currentRow();
    switch (item->column()) {
    case 0:
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,0)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use"));
                item->setText(automata.get_automaton_at(id)->getEvent(item->row()).getLabel());
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,0)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use"));
                item->setText(automata.get_automaton_at(id)->getEvent(item->row()).getLabel());
                return;
            }
        }
        try {
            Event s = automata.get_automaton_at(id)->getEvent(item->row());
            s.setLabel(item->text());
            automata.get_automaton_at(id)->getEventList()->replace(item->row(),s);
        } catch (SetterException &e) {
            QMessageBox::information(this, tr("Error"),
            e.getMsg());
            item->setText(automata.get_automaton_at(id)->getEvent(item->row()).getLabel());
            return;
        }
        break;
    case 1:
        automata.get_automaton_at(id)->getEvent(item->row()).setObservable(item->checkState()==Qt::Checked?true:false);
        break;
    case 2:
        automata.get_automaton_at(id)->getEvent(item->row()).setControlable(item->checkState()==Qt::Checked?true:false);
        break;
    }
}

void MainWindow::on_Transitions_list_itemChanged(QTableWidgetItem *item)
{
    /*TO DO*/
    /*int id = ui->Automatons_list->currentRow();
    int s;
    int d;
    int e;
    switch (item->column()) {
    case (0):
        for (int i = 0;i < ui->States_list->rowCount();i++) {
            if(ui->States_list->item(i,0)->text() == item->text())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use"));
                item->setText(automata.get_automaton_at(id)->getEvent(item->row()).getLabel());
                return;
            }
        }
        for (int i = 0;i < ui->Events_list->rowCount();i++) {
            if(ui->Events_list->item(i,0)->text() == item->text() && i != item->row())
            {
                QMessageBox::information(this, tr("Error"),
                QString("This name is already in use"));
                item->setText(automata.get_automaton_at(id)->getEvent(item->row()).getLabel());
                return;
            }
        }
        try {
            Event s = automata.get_automaton_at(id)->getEvent(item->row());
            s.setLabel(item->text());
            automata.get_automaton_at(id)->getEventList()->replace(item->row(),s);
        } catch (SetterException &e) {
            QMessageBox::information(this, tr("Error"),
            e.getMsg());
            item->setText(automata.get_automaton_at(id)->getEvent(item->row()).getLabel());
            return;
        }
        break;
    case 1:
        automata.get_automaton_at(id)->getEvent(item->row()).setObservable(item->checkState()==Qt::Checked?true:false);
        break;
    case 2:
        automata.get_automaton_at(id)->getEvent(item->row()).setControlable(item->checkState()==Qt::Checked?true:false);
        break;
    }*/
}
