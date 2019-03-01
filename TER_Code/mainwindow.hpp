#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTableWidgetItem>
#include "automata.hpp"
#include "translator.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_Import_triggered();

    void on_Automatons_list_itemSelectionChanged();

    void on_Generate_Button_clicked();

    void on_actionClose_triggered();

    void on_actionSave_as_triggered();

    void on_States_list_itemChanged(QTableWidgetItem *item);

    void on_Events_list_itemChanged(QTableWidgetItem *item);

    void on_Transitions_list_itemChanged(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    Automata automata;
    Translator translator;
    void toggle_interface(bool b);
    void fill_interface();
    void fill_automaton_list();
    void clear_interface();
    void clear_automaton_list();
};

#endif // MAINWINDOW_HPP
