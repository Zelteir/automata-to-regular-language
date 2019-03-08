#include "create_state_dialog.hpp"
#include "ui_create_state_dialog.h"

Create_state_dialog::Create_state_dialog(QList<Event> eventList, QList<State> stateList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_state_dialog)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_state_dialog_accept_clicked()));
    this->eventList = eventList;
    this->stateList = stateList;
}

Create_state_dialog::~Create_state_dialog()
{
    delete ui;
}

/*TO DO*/
void Create_state_dialog::Create_state_dialog_accept_clicked()
{
    if(ui->edit_name->text().isEmpty())
    {
        ui->label_alert->setText(QString("State must have a name."));
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
    State s(ui->edit_name->text(),ui->checkBox_initial->isChecked(),ui->checkBox_accepting->isChecked());
    emit creation(s);
    emit accept();
}
