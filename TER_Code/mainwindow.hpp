#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "automata.hpp"

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

private:
    Ui::MainWindow *ui;
    Automata automata;
    void activate_interface();
    void fill_interface();
    void fill_automaton_list();
    void clear_interface();
    void clear_automaton_list();
};

#endif // MAINWINDOW_HPP
