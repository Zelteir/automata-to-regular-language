#include "type_choice.hpp"
#include "ui_type_choice.h"
#include <QFileDialog>

Type_choice::Type_choice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::type_choice)
{
    ui->setupUi(this);
}

Type_choice::~Type_choice()
{
    delete ui;
}

/*TO DO*/
void Type_choice::selection(){
    Automata automata;
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open automaton file"), "", tr("file (*);;All Files (*)"));
    if (automata.fromSupremica(file_name))
    {

    }
}

/*TO DO*/
void Type_choice::on_buttonBox_accepted()
{

}

/*TO DO*/
void Type_choice::on_buttonBox_rejected()
{

}
