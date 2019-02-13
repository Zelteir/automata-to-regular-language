#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "type_choice.hpp"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    /*
     * TO DO
     * Other things to enable
    */
    this->fill_automaton_list();
    this->fill_interface();
}

/*
 * Fill the interface with the information contained in the current automaton
*/
void MainWindow::fill_interface()
{
    QString string;
    int id = ui->Automatons_list->currentRow();
    for(State s : automata.get_automaton_at(id).getStateList())
        ui->States_list->addItem(s.getName());
    ui->Events_list->setRowCount(automata.get_automaton_at(id).getEventList().length());
    for(int i = 0; i< automata.get_automaton_at(id).getEventList().length();i++)
    {
        ui->Events_list->setItem(i,0,new QTableWidgetItem(automata.get_automaton_at(id).getEventList()[i].getLabel()));
        if(!automata.get_automaton_at(id).getEventList()[i].getObservable())
            ui->Events_list->setItem(i,1,new QTableWidgetItem("X"));
        if(!automata.get_automaton_at(id).getEventList()[i].getControlable())
            ui->Events_list->setItem(i,2,new QTableWidgetItem("X"));
    }
    for(Transition t : automata.get_automaton_at(id).getTransitionList())
    {
        string = "";
        string.append(automata.get_automaton_at(id).getEventList()[t.getEvent()].getLabel())
                .append(": ")
                .append(automata.get_automaton_at(id).getStateList()[t.getSource()].getName())
                .append(" -> ")
                .append(automata.get_automaton_at(id).getStateList()[t.getDest()].getName());
        ui->Transitions_list->addItem(string);
    }
}

/*
 * Fill the list of automaton wontained inside the automata
*/
void MainWindow::fill_automaton_list()
{
    for(Automaton tmp : this->automata.get_automatons())
        ui->Automatons_list->addItem(tmp.getName());
    ui->Automatons_list->setCurrentRow(0);
}

void MainWindow::on_Automatons_list_itemSelectionChanged()
{
    fill_interface();
}
