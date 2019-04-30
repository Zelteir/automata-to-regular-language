#ifndef DELETE_EVENT_DIALOG_HPP
#define DELETE_EVENT_DIALOG_HPP

#include <QDialog>
#include "automaton.hpp"
#include "table_widget_checkbox_item.hpp"

namespace Ui {
class Delete_event_dialog;
}

class Delete_event_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Delete_event_dialog(QMap<int, Event> eventList, QWidget *parent = nullptr);
    ~Delete_event_dialog();

private slots:
    void on_event_list_cellClicked(int row, int column);

    void on_buttonBox_accepted();

signals:
    void delete_event(QList<int>);

private:
    Ui::Delete_event_dialog *ui;
    QMap<int, Event> eventList;
};

#endif // DELETE_EVENT_DIALOG_HPP
