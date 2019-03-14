#include "create_transition_dialog.hpp"
#include "ui_create_transition_dialog.h"
#include <QDebug>

Create_transition_dialog::Create_transition_dialog(QList<Event> eventList, QList<State> stateList, QList<Transition> transitionList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_transition_dialog),
    eventList(eventList),
    stateList(stateList),
    transitionList(transitionList)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_transition_dialog_accept_clicked()));
    for(Event e : eventList)
        this->eventNameList.append(e.getLabel());
    for(State s : stateList)
        this->stateNameList.append(s.getName());
    stateCompleter = new QCompleter(stateNameList,this);
    eventCompleter = new QCompleter(eventNameList,this);
    eventCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    stateCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->edit_source->setCompleter(stateCompleter);
    ui->edit_dest->setCompleter(stateCompleter);
    ui->edit_event->setCompleter(eventCompleter);
}

Create_transition_dialog::~Create_transition_dialog()
{
    delete stateCompleter;
    delete eventCompleter;
    delete ui;
}

void Create_transition_dialog::Create_transition_dialog_accept_clicked()
{
    int idSource;
    int idDest;
    int idEvent;
    if(ui->edit_source->text().isEmpty() || ui->edit_dest->text().isEmpty() || ui->edit_event->text().isEmpty())
    {
        ui->label_alert->setText(QString("All fields must be filled."));
        return;
    }
    if(!stateNameList.contains(ui->edit_source->text(), Qt::CaseInsensitive))
    {
        ui->label_alert->setText(QString("Source state does not exist."));
        return;
    }
    if(!stateNameList.contains(ui->edit_dest->text(), Qt::CaseInsensitive))
    {
        ui->label_alert->setText(QString("Destination state does not exist."));
        return;
    }
    if(!eventNameList.contains(ui->edit_event->text(), Qt::CaseInsensitive))
    {
        ui->label_alert->setText(QString("Event does not exist."));
        return;
    }

    /*check if transition already exist*/

    /*create transition*/
    emit accept();
}
