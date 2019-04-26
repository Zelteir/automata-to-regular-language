#include "delete_transition_dialog.hpp"
#include "ui_delete_transition_dialog.h"

Delete_transition_dialog::Delete_transition_dialog(QMap<int, Event> eventList, QMap<int, State> stateList, QMap<int, Transition> transitionList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Delete_transition_dialog),
    transitionList(transitionList)
{
    ui->setupUi(this);
    int pos;
    for(Transition t : transitionList)
    {
        pos = ui->Transitions_list->rowCount();
        ui->Transitions_list->insertRow(pos);

        ui->Transitions_list->setItem(pos,0, new QTableWidgetItem(QString::number(t.getId())));
        ui->Transitions_list->setItem(pos,1, new QTableWidgetItem(stateList.find(t.getSource()).value().getName()));
        ui->Transitions_list->setItem(pos,2, new QTableWidgetItem(stateList.find(t.getDest()).value().getName()));
        ui->Transitions_list->setItem(pos,3, new QTableWidgetItem(eventList.find(t.getEvent()).value().getLabel()));
        ui->Transitions_list->setItem(pos,4,new Table_Widget_Checkbox_Item(""));
        ui->Transitions_list->item(pos,4)->setFlags(ui->Transitions_list->item(pos,4)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
        ui->Transitions_list->item(pos,4)->setCheckState(Qt::Unchecked);
    }
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch );
    ui->Transitions_list->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch );
    ui->Transitions_list->hideColumn(0);
}

Delete_transition_dialog::~Delete_transition_dialog()
{
    delete ui;
}

void Delete_transition_dialog::on_buttonBox_accepted()
{
    QList<int> deleteList;
    for(int i = 0; i < ui->Transitions_list->rowCount(); i++)
    {
        if(ui->Transitions_list->item(i,4)->checkState())
            deleteList.append(i);
    }
    emit(delete_transition(deleteList));
    emit(accept());
}

void Delete_transition_dialog::on_Transitions_list_cellClicked(int row, int column)
{
    bool state;
    if(column == 4)
    {
        state = ui->Transitions_list->item(row,column)->checkState();
        ui->Transitions_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
    }
}
