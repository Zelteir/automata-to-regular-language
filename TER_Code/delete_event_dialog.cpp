#include "delete_event_dialog.hpp"
#include "ui_delete_event_dialog.h"

Delete_event_dialog::Delete_event_dialog(QMap<int, Event> eventList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Delete_event_dialog),
    eventList(eventList)
{
    ui->setupUi(this);
    int pos;
    for(Event e : eventList)    //fill interface with events
    {
        pos = ui->event_list->rowCount();
        ui->event_list->insertRow(pos);

        ui->event_list->setItem(pos,0, new QTableWidgetItem(QString::number(e.getId())));
        ui->event_list->setItem(pos,1, new QTableWidgetItem(e.getLabel()));
        ui->event_list->setItem(pos,2,new Table_Widget_Checkbox_Item(""));
        ui->event_list->item(pos,2)->setFlags(ui->event_list->item(pos,2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
        ui->event_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->event_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->event_list->hideColumn(0);
}

Delete_event_dialog::~Delete_event_dialog()
{
    delete ui;
}

/*
 * Detect selection of whole cell instead of just checkbox
*/
void Delete_event_dialog::on_event_list_cellClicked(int row, int column)
{
    bool state;
    if(column == 2)
    {
        state = ui->event_list->item(row,column)->checkState();
        ui->event_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
    }
}

/*
 * Recieve signal when accept is clicked. Create list of all selected automata and send it to MainWindow via signal
*/
void Delete_event_dialog::on_buttonBox_accepted()
{
    QList<int> deleteList;
    for(int i = 0; i < ui->event_list->rowCount(); i++)
    {
        if(ui->event_list->item(i,2)->checkState()==Qt::Checked)
            deleteList.append(ui->event_list->item(i,0)->text().toInt());
    }
    emit(delete_event(deleteList));
    emit(accept());
}
