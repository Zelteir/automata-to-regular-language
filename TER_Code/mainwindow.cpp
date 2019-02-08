#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "type_choice.hpp"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*TO DO*/
void MainWindow::on_actionOpen_Import_triggered()
{
    /*Type_choice window_choice;
    window_choice.show();*/
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open automaton file"), "", tr("XML file (*.xml);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    if (automata.fromSupremica(file_name))
    {
        this->activate_interface();
    }
}

/*TO DO*/
void MainWindow::activate_interface()
{
    ui->Events_list->setEnabled(true);
    ui->States_list->setEnabled(true);
    ui->Automatons_list->setEnabled(true);
    ui->Transitions_list->setEnabled(true);
    /*Other things to enable*/
}
