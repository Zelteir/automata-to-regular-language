#ifndef DELETE_AUTOMATON_DIALOG_HPP
#define DELETE_AUTOMATON_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"
#include "table_widget_checkbox_item.hpp"

namespace Ui {
class Delete_automaton_dialog;
}

class Delete_automaton_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Delete_automaton_dialog(QMap<int, Automaton>, QWidget *parent = nullptr);
    ~Delete_automaton_dialog();

private slots:
    void on_automaton_list_cellClicked(int row, int column);

    void on_buttonBox_accepted();

signals:
    void delete_automaton(QList<int>);

private:
    Ui::Delete_automaton_dialog *ui;
    QMap<int, Automaton> automatonList;
};

#endif // DELETE_AUTOMATON_DIALOG_HPP
