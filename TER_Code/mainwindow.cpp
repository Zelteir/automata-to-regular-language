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

/*TO DO*/
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

/*TO DO*/
void MainWindow::activate_interface()
{
    ui->Events_list->setEnabled(true);
    ui->States_list->setEnabled(true);
    ui->Automatons_list->setEnabled(true);
    ui->Transitions_list->setEnabled(true);
    /*Other things to enable*/
    this->fill_automaton_list();
    this->fill_interface();
}

void MainWindow::fill_interface()
{
    QString string;
    int id = ui->Automatons_list->currentRow();
    for(State s : automata.get_automaton_at(id).getStateList())
        ui->States_list->addItem(s.getName());
    for(Event e : automata.get_automaton_at(id).getEventList())
        ui->Events_list->addItem(e.getLabel());
    for(Transition t : automata.get_automaton_at(id).getTransitionList())
    {
        string = "";
        string += automata.get_automaton_at(id).getEventList()[t.getEvent()].getLabel();
        string += ": ";
        string += automata.get_automaton_at(id).getStateList()[t.getSource()].getName();
        string += " -> ";
        string += automata.get_automaton_at(id).getStateList()[t.getDest()].getName();
        ui->Transitions_list->addItem(string);
    }
}

void MainWindow::fill_automaton_list()
{
    for(Automaton tmp : this->automata.get_automatons())
        ui->Automatons_list->addItem(tmp.getName());
    ui->Automatons_list->setCurrentRow(0);
}
