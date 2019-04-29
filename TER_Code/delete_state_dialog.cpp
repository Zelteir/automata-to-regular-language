#include "delete_state_dialog.hpp"
#include "ui_delete_state_dialog.h"

Delete_state_dialog::Delete_state_dialog(QMap<int, State> stateList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Delete_state_dialog),
    stateList(stateList)
{
    ui->setupUi(this);
    int pos;
    for(State s : stateList)
    {
        pos = ui->state_list->rowCount();
        ui->state_list->insertRow(pos);

        ui->state_list->setItem(pos,0, new QTableWidgetItem(QString::number(s.getId())));
        ui->state_list->setItem(pos,1, new QTableWidgetItem(s.getName()));
        ui->state_list->setItem(pos,2,new Table_Widget_Checkbox_Item(""));
        ui->state_list->item(pos,2)->setFlags(ui->state_list->item(pos,2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
        ui->state_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->state_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->state_list->hideColumn(0);
}

Delete_state_dialog::~Delete_state_dialog()
{
    delete ui;
}

void Delete_state_dialog::on_state_list_cellClicked(int row, int column)
{
    bool state;
    if(column == 2)
    {
        state = ui->state_list->item(row,column)->checkState();
        ui->state_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
    }
}

void Delete_state_dialog::on_buttonBox_accepted()
{
    QList<int> deleteList;
    for(int i = 0; i < ui->state_list->rowCount(); i++)
    {
        if(ui->state_list->item(i,2)->checkState()==Qt::Checked)
            deleteList.append(ui->state_list->item(i,0)->text().toInt());
    }
    emit(delete_state(deleteList));
    emit(accept());
}
