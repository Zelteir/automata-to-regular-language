#include "create_transition_dialog.hpp"
#include "ui_create_transition_dialog.h"
#include <QDebug>

Create_transition_dialog::Create_transition_dialog(int idTransition, QMap<int, Event> eventList, QMap<int, State> stateList, QMap<int, Transition> transitionList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_transition_dialog),
    eventList(eventList),
    stateList(stateList),
    transitionList(transitionList),
    idTransition(idTransition)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Create_transition_dialog_accept_clicked()));
    for(Event e : eventList)
        this->eventNameList.append(e.getLabel());
    for(State s : stateList)
        this->stateNameList.append(s.getName());
    stateCompleter = new QCompleter(stateNameList,this);    //autocompleter for states
    eventCompleter = new QCompleter(eventNameList,this);    //autocompleter for events
    eventCompleter->setCaseSensitivity(Qt::CaseSensitive);
    stateCompleter->setCaseSensitivity(Qt::CaseSensitive);
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

/*
 * Recieve signal when accept is clicked. Check values and send created transitions to MainWindow via a signal
*/
void Create_transition_dialog::Create_transition_dialog_accept_clicked()
{
    State idSource;
    State idDest;
    Event idEvent;
    if(ui->edit_source->text().isEmpty() || ui->edit_dest->text().isEmpty() || ui->edit_event->text().isEmpty())
    {
        ui->label_alert->setText(QString("All fields must be filled."));
        return;
    }
    if(!stateNameList.contains(ui->edit_source->text(), Qt::CaseSensitive))
    {
        ui->label_alert->setText(QString("Source state does not exist."));
        return;
    }
    if(!stateNameList.contains(ui->edit_dest->text(), Qt::CaseSensitive))
    {
        ui->label_alert->setText(QString("Destination state does not exist."));
        return;
    }
    if(!eventNameList.contains(ui->edit_event->text(), Qt::CaseSensitive))
    {
        ui->label_alert->setText(QString("Event does not exist."));
        return;
    }

    /*check if transition already exist*/
    Transition t;
    for(Event tmpEvent : eventList) //find event ID
    {
        if(ui->edit_event->text() == tmpEvent.getLabel())
        {
            idEvent = tmpEvent;
            t.setEvent(idEvent.getId());
            break;
        }
    }
    for(State tmpState : stateList) //find destination state ID
    {
        if(ui->edit_dest->text() == tmpState.getName())
        {
            idDest = tmpState;
            t.setDest(idDest.getId());
            break;
        }
    }
    for(State tmpState : stateList) //find source state ID
    {
        if(ui->edit_source->text() == tmpState.getName())
        {
            idSource = tmpState;
            t.setSource(idSource.getId());
            break;
        }
    }
    for(Transition transition : transitionList)
    {
        if(t == transition)
        {
            ui->label_alert->setText(QString("Transition already exist."));
            return;
        }
    }
    /*create transition*/
    emit creation_transition(Transition(idTransition, idSource.getId(), idDest.getId(), idEvent.getId()));
    emit accept();
}
