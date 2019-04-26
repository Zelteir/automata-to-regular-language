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
    explicit Create_state_dialog(int idState, QMap<int, Event> eventList, QMap<int, State> stateList, QWidget *parent = nullptr);
    ~Create_state_dialog();

private slots:
    void Create_state_dialog_accept_clicked();

signals:
    void creation_state(State);

private:
    Ui::Create_state_dialog *ui;
    QMap<int, Event> eventList;
    QMap<int, State> stateList;
    int idState;
};

#endif // CREATE_STATE_DIALOG_HPP
