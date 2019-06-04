#include "delete_transition_dialog.hpp"
#include "ui_delete_transition_dialog.h"

Delete_transition_dialog::Delete_transition_dialog(QMap<int, Event> eventList, QMap<int, State> stateList, QMap<int, Transition> transitionList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Delete_transition_dialog),
    transitionList(transitionList)
{
    ui->setupUi(this);
    int pos;
    for(Transition t : transitionList)  //fill interface with trnsitions
    {
        pos = ui->transitions_list->rowCount();
        ui->transitions_list->insertRow(pos);

        ui->transitions_list->setItem(pos,0, new QTableWidgetItem(QString::number(t.getId())));
        ui->transitions_list->setItem(pos,1, new QTableWidgetItem(stateList.find(t.getSource()).value().getName()));
        ui->transitions_list->setItem(pos,2, new QTableWidgetItem(stateList.find(t.getDest()).value().getName()));
        ui->transitions_list->setItem(pos,3, new QTableWidgetItem(eventList.find(t.getEvent()).value().getName()));
        ui->transitions_list->setItem(pos,4,new Table_Widget_Checkbox_Item(""));
        ui->transitions_list->item(pos,4)->setFlags(ui->transitions_list->item(pos,4)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
        ui->transitions_list->item(pos,4)->setCheckState(Qt::Unchecked);
    }
    ui->transitions_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->transitions_list->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch );
    ui->transitions_list->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch );
    ui->transitions_list->hideColumn(0);
}

Delete_transition_dialog::~Delete_transition_dialog()
{
    delete ui;
}

/*
 * Detect selection of whole cell instead of just checkbox
*/
void Delete_transition_dialog::on_transitions_list_cellClicked(int row, int column)
{
    bool state;
    if(column == 4)
    {
        state = ui->transitions_list->item(row,column)->checkState();
        ui->transitions_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
    }
}

/*
 * Recieve signal when accept is clicked. Create list of all selected transitions and send it to MainWindow via signal
*/
void Delete_transition_dialog::on_buttonBox_accepted()
{
    QList<int> deleteList;
    for(int i = 0; i < ui->transitions_list->rowCount(); i++)
    {
        if(ui->transitions_list->item(i,4)->checkState())
            deleteList.append(ui->transitions_list->item(i,0)->text().toInt());
    }
    emit(delete_transition(deleteList));
    emit(accept());
}
