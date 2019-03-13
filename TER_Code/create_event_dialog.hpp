#ifndef CREATE_EVENT_DIALOG_HPP
#define CREATE_EVENT_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"

namespace Ui {
class Create_event_dialog;
}

class Create_event_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Create_event_dialog(QList<Event> eventList, QList<State> stateList, QWidget *parent = nullptr);
    ~Create_event_dialog();

private slots:

    void Create_event_dialog_accept_clicked();

signals:
    void creation_event(Event);

private:
    Ui::Create_event_dialog *ui;
    QList<Event> eventList;
    QList<State> stateList;
};

#endif // CREATE_EVENT_DIALOG_HPP
