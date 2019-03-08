#ifndef CREATE_STATE_DIALOG_HPP
#define CREATE_STATE_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"

namespace Ui {
class Create_state_dialog;
}

class Create_state_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Create_state_dialog(QList<Event> eventList, QList<State> stateList, QWidget *parent = nullptr);
    ~Create_state_dialog();

private slots:

    void Create_state_dialog_accept_clicked();

signals:
    void creation(State);

private:
    Ui::Create_state_dialog *ui;
    QList<Event> eventList;
    QList<State> stateList;
};

#endif // CREATE_STATE_DIALOG_HPP
