#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "type_choice.hpp"
#include <QFileDialog>
#include <QDebug>

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
        this->activate_interface();
    }
}

/*
 * Enable the interface to user
*/
void MainWindow::activate_interface()
{
    ui->Events_list->setEnabled(true);
    ui->States_list->setEnabled(true);
    ui->Automatons_list->setEnabled(true);
    ui->Transitions_list->setEnabled(true);
    ui->Generate_Button->setEnabled(true);
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
    clear_interface();
    QString string;
    int i;
    int id = ui->Automatons_list->currentRow();
    ui->States_list->setRowCount(automata.get_automaton_at(id).getStateList().length());
    /*fill states table with name, initial and accepted information*/
    for(i = 0; i< automata.get_automaton_at(id).getStateList().length();i++)
    {
        ui->States_list->setItem(i,0,new QTableWidgetItem(automata.get_automaton_at(id).getStateList()[i].getName()));
        if(automata.get_automaton_at(id).getStateList()[i].getInitial())
        {
            ui->States_list->setItem(i,1,new QTableWidgetItem(""));
            ui->States_list->item(i,1)->setCheckState(Qt::Checked);
        }
        else {
            ui->States_list->setItem(i,1,new QTableWidgetItem(""));
            ui->States_list->item(i,1)->setCheckState(Qt::Unchecked);
        }
        if(automata.get_automaton_at(id).getStateList()[i].getAccepting())
        {
            ui->States_list->setItem(i,2,new QTableWidgetItem(""));
            ui->States_list->item(i,2)->setCheckState(Qt::Checked);
        }
        else {
            ui->States_list->setItem(i,2,new QTableWidgetItem(""));
            ui->States_list->item(i,2)->setCheckState(Qt::Unchecked);
        }
    }
    ui->Events_list->setRowCount(automata.get_automaton_at(id).getEventList().length());
    /*fill events table with name, observable and controlable information*/
    for(i = 0; i< automata.get_automaton_at(id).getEventList().length();i++)
    {
        ui->Events_list->setItem(i,0,new QTableWidgetItem(automata.get_automaton_at(id).getEventList()[i].getLabel()));
        if(!automata.get_automaton_at(id).getEventList()[i].getObservable())
        {
            ui->Events_list->setItem(i,1,new QTableWidgetItem(""));
            ui->Events_list->item(i,1)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->Events_list->setItem(i,1,new QTableWidgetItem(""));
            ui->Events_list->item(i,1)->setCheckState(Qt::Unchecked);
        }
        if(!automata.get_automaton_at(id).getEventList()[i].getControlable())
        {
            ui->Events_list->setItem(i,2,new QTableWidgetItem(""));
            ui->Events_list->item(i,2)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->Events_list->setItem(i,2,new QTableWidgetItem(""));
            ui->Events_list->item(i,2)->setCheckState(Qt::Unchecked);
        }
    }
    ui->Transitions_list->setRowCount(automata.get_automaton_at(id).getTransitionList().length());
    /*fill transition table with origin, destination and event information*/
    for(i = 0; i< automata.get_automaton_at(id).getTransitionList().length();i++)
    {
        ui->Transitions_list->setItem(i,0, new QTableWidgetItem(*ui->States_list->item(automata.get_automaton_at(id).getTransitionList()[i].getSource(),0)));
        ui->Transitions_list->setItem(i,1, new QTableWidgetItem(*ui->States_list->item(automata.get_automaton_at(id).getTransitionList()[i].getDest(),0)));
        ui->Transitions_list->setItem(i,2, new QTableWidgetItem(*ui->Events_list->item(automata.get_automaton_at(id).getTransitionList()[i].getEvent(),0)));
    }
}

/*
 * Fill the list of automaton wontained inside the automata
*/
void MainWindow::fill_automaton_list()
{
    clear_automaton_list();
    for(Automaton tmp : this->automata.get_automatons())
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
    translator.brzozowskiMethod(automata.get_automaton_at(ui->Automatons_list->currentRow()));
    ui->Generated_Regular_Language->setPlainText(translator.getRegex());
}
