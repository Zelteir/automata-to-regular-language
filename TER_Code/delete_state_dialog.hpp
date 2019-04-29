#ifndef DELETE_STATE_DIALOG_HPP
#define DELETE_STATE_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"
#include "table_widget_checkbox_item.hpp"

namespace Ui {
class Delete_state_dialog;
}

class Delete_state_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Delete_state_dialog(QMap<int, State> stateList, QWidget *parent = nullptr);
    ~Delete_state_dialog();

private slots:
    void on_state_list_cellClicked(int row, int column);

    void on_buttonBox_accepted();

signals:
    void delete_state(QList<int>);

private:
    Ui::Delete_state_dialog *ui;
    QMap<int, State> stateList;
};

#endif // DELETE_STATE_DIALOG_HPP
