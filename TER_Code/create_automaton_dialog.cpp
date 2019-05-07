#include "create_automaton_dialog.hpp"
#include "ui_create_automaton_dialog.h"

Create_automaton_dialog::Create_automaton_dialog(int idAutomaton, QMap<int, Automaton> automatonList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_automaton_dialog),
    automatonList(automatonList),
    idAutomaton(idAutomaton)
{
    ui->setupUi(this);
}

Create_automaton_dialog::~Create_automaton_dialog()
{
    delete ui;
}


/*
 * Recieve signal when accept is clicked. Check values and send created automaton to MainWindow via a signal
*/
void Create_automaton_dialog::on_buttonBox_accepted()
{
    if(ui->nameEdit->text().isEmpty())
    {
        ui->label_alert->setText(QString("Automaton must have a name."));
        return;
    }
    for(Automaton a : automatonList)
    {
        if(a.getName() == ui->nameEdit->text())
        {
            ui->label_alert->setText(QString("Name already in use."));
            return;
        }
    }
    Automaton a(idAutomaton, ui->nameEdit->text());
    emit creation_automaton(a);
    emit accept();
}
