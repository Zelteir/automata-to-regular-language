#include "create_state_dialog.hpp"
#include "ui_create_state_dialog.h"

Create_state_dialog::Create_state_dialog(int idState, QMap<int, Event> eventList, QMap<int, State> stateList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_state_dialog),
    eventList(eventList),
    stateList(stateList),
    idState(idState)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_state_dialog_accept_clicked()));
}

Create_state_dialog::~Create_state_dialog()
{
    delete ui;
}

/*
 * Recieve signal when accept is clicked. Check values and send created state to MainWindow via a signal
*/
void Create_state_dialog::Create_state_dialog_accept_clicked()
{
    if(ui->edit_name->text().isEmpty())
    {
        ui->label_alert->setText(QString("State must have a name."));
        return;
    }
    for(Event tmpEvent : eventList)
    {
        if(ui->edit_name->text() == tmpEvent.getName())
        {
            ui->label_alert->setText(QString("Name already in use."));
            return;
        }
    }
    for(State tmpState : stateList)
    {
        if(ui->edit_name->text() == tmpState.getName())
        {
            ui->label_alert->setText(QString("Name already in use."));
            return;
        }
    }
    State s(idState, ui->edit_name->text(),ui->checkBox_initial->isChecked(),ui->checkBox_accepting->isChecked());
    emit creation_state(s);
    emit accept();
}
