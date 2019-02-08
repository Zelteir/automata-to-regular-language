#include "type_choice.hpp"
#include "ui_type_choice.h"

type_choice::type_choice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::type_choice)
{
    ui->setupUi(this);
}

type_choice::~type_choice()
{
    delete ui;
}
