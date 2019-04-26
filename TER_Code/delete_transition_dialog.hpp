#ifndef DELETE_TRANSITION_DIALOG_HPP
#define DELETE_TRANSITION_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"
#include "table_widget_checkbox_item.hpp"

namespace Ui {
class Delete_transition_dialog;
}

class Delete_transition_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Delete_transition_dialog(QMap<int, Event> eventList, QMap<int, State> stateList, QMap<int, Transition> transitionList, QWidget *parent = nullptr);
    ~Delete_transition_dialog();

private slots:
    void on_buttonBox_accepted();

    void on_Transitions_list_cellClicked(int row, int column);

signals:
    void delete_transition(QList<int>);

private:
    Ui::Delete_transition_dialog *ui;
    QMap<int, Transition> transitionList;
};

#endif // DELETE_TRANSITION_DIALOG_HPP
