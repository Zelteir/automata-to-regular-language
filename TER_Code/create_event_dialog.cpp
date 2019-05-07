#include "create_event_dialog.hpp"
#include "ui_create_event_dialog.h"

Create_event_dialog::Create_event_dialog(int idEvent, QMap<int, Event> eventList, QMap<int, State> stateList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_event_dialog),
    eventList(eventList),
    stateList(stateList),
    idEvent(idEvent)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_event_dialog_accept_clicked()));
}

Create_event_dialog::~Create_event_dialog()
{
    delete ui;
}

/*
 * Recieve signal when accept is clicked. Check values and send created event to MainWindow via a signal
*/
void Create_event_dialog::Create_event_dialog_accept_clicked()
{
    if(ui->edit_name->text().isEmpty())
    {
        ui->label_alert->setText(QString("Event must have a name."));
        return;
    }
    for(Event tmpEvent : eventList)
    {
        if(ui->edit_name->text() == tmpEvent.getLabel())
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
    Event e(idEvent, ui->edit_name->text(),!ui->checkBox_Unobservable->isChecked(),!ui->checkBox_Uncontrolable->isChecked());
    emit creation_event(e);
    emit accept();
}
