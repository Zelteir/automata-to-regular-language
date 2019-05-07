#include "help_dialog.hpp"
#include "ui_help_dialog.h"

Help_dialog::Help_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help_dialog)
{
    ui->setupUi(this);
}

Help_dialog::~Help_dialog()
{
    delete ui;
}
