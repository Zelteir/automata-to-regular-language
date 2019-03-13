#ifndef CREATE_TRANSITION_DIALOG_HPP
#define CREATE_TRANSITION_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"

namespace Ui {
class Create_transition_dialog;
}

class Create_transition_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Create_transition_dialog(QList<Event> eventList, QList<State> stateList, QWidget *parent = nullptr);
    ~Create_transition_dialog();

private slots:

    void Create_transition_dialog_accept_clicked();

signals:
    void creation_transition(Transition);

private:
    Ui::Create_transition_dialog *ui;
    QList<Event> eventList;
    QList<State> stateList;
};

#endif // CREATE_TRANSITION_DIALOG_HPP
