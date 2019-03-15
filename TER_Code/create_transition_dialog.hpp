#ifndef CREATE_TRANSITION_DIALOG_HPP
#define CREATE_TRANSITION_DIALOG_HPP

#include <QDialog>
#include <QCompleter>
#include "automaton.hpp"

namespace Ui {
class Create_transition_dialog;
}

class Create_transition_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Create_transition_dialog(QList<Event> eventList, QList<State> stateList, QList<Transition> transitionList, QWidget *parent = nullptr);
    ~Create_transition_dialog();

private slots:

    void Create_transition_dialog_accept_clicked();

signals:
    void creation_transition(Transition);

private:
    Ui::Create_transition_dialog *ui;
    QList<Event> eventList;
    QList<State> stateList;
    QList<Transition> transitionList;
    QStringList eventNameList;
    QStringList stateNameList;
    QCompleter *eventCompleter;
    QCompleter *stateCompleter;
};

#endif // CREATE_TRANSITION_DIALOG_HPP
