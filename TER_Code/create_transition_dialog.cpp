#include "create_transition_dialog.hpp"
#include "ui_create_transition_dialog.h"

Create_transition_dialog::Create_transition_dialog(QList<Event> eventList, QList<State> stateList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_transition_dialog)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_transition_dialog_accept_clicked()));
    this->eventList = eventList;
    this->stateList = stateList;
    /*TO DO completer*/
}

Create_transition_dialog::~Create_transition_dialog()
{
    delete ui;
}

void Create_transition_dialog::Create_transition_dialog_accept_clicked()
{

}
