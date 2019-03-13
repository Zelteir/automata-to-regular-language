#include "create_event_dialog.hpp"
#include "ui_create_event_dialog.h"

Create_event_dialog::Create_event_dialog(QList<Event> eventList, QList<State> stateList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_event_dialog)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_event_dialog_accept_clicked()));
    this->eventList = eventList;
    this->stateList = stateList;
}

Create_event_dialog::~Create_event_dialog()
{
    delete ui;
}

void Create_event_dialog::Create_event_dialog_accept_clicked()
{
    if(ui->edit_name->text().isEmpty())
    {
        ui->label_alert->setText(QString("Event must have a name."));
        return;
    }
    for(int i = 0; i < eventList.size();i++)
    {
        if(ui->edit_name->text() == eventList[i].getLabel())
        {
            ui->label_alert->setText(QString("Name already in use."));
            return;
        }
    }
    for(int i = 0; i < stateList.size();i++)
    {
        if(ui->edit_name->text() == stateList[i].getName())
        {
            ui->label_alert->setText(QString("Name already in use."));
            return;
        }
    }
    Event e(ui->edit_name->text(),!ui->checkBox_Unobservable->isChecked(),!ui->checkBox_Uncontrolable->isChecked());
    emit creation_event(e);
    emit accept();
}