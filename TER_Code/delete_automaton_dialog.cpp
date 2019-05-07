#include "delete_automaton_dialog.hpp"
#include "ui_delete_automaton_dialog.h"

Delete_automaton_dialog::Delete_automaton_dialog(QMap<int, Automaton> automatonList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Delete_automaton_dialog),
    automatonList(automatonList)
{
    ui->setupUi(this);
    int pos;

    for(Automaton a : automatonList)    //fill interface with automata
    {
        pos = ui->automaton_list->rowCount();
        ui->automaton_list->insertRow(pos);

        ui->automaton_list->setItem(pos,0, new QTableWidgetItem(QString::number(a.getId())));
        ui->automaton_list->setItem(pos,1, new QTableWidgetItem(a.getName()));
        ui->automaton_list->setItem(pos,2,new Table_Widget_Checkbox_Item(""));
        ui->automaton_list->item(pos,2)->setFlags(ui->automaton_list->item(pos,2)->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable));
        ui->automaton_list->item(pos,2)->setCheckState(Qt::Unchecked);
    }
    ui->automaton_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch );
    ui->automaton_list->hideColumn(0);
}

Delete_automaton_dialog::~Delete_automaton_dialog()
{
    delete ui;
}

/*
 * Detect selection of whole cell instead of just checkbox
*/
void Delete_automaton_dialog::on_automaton_list_cellClicked(int row, int column)
{
    bool state;
    if(column == 2)
    {
        state = ui->automaton_list->item(row,column)->checkState();
        ui->automaton_list->item(row,column)->setCheckState((state)?Qt::Unchecked:Qt::Checked);
    }
}

/*
 * Recieve signal when accept is clicked. Create list of all selected automata and send it to MainWindow via signal
*/
void Delete_automaton_dialog::on_buttonBox_accepted()
{
    QList<int> deleteList;
    for(int i = 0; i < ui->automaton_list->rowCount(); i++)
    {
        if(ui->automaton_list->item(i,2)->checkState()==Qt::Checked)
            deleteList.append(ui->automaton_list->item(i,0)->text().toInt());
    }
    emit(delete_automaton(deleteList));
    emit(accept());
}

