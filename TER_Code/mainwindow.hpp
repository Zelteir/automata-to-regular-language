#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QListWidget>
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QActionGroup>
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

    void on_Automatons_list_itemChanged(QListWidgetItem *item);

    void on_States_list_itemChanged(QTableWidgetItem *item);

    void on_Events_list_itemChanged(QTableWidgetItem *item);

    void on_Transitions_list_itemChanged(QTableWidgetItem *item);

    void on_actionStateCreate_triggered();

    void createState_finished(State);

    void on_actionEventCreate_triggered();

    void createEvent_finished(Event e);

    void on_actionTransitionCreate_triggered();

    void createTransition_finished(Transition t);

    void on_actionAutomatonCreate_triggered();

    void createAutomaton_finished(Automaton a);

    void on_actionSaveAutomaton_triggered();

    void on_actionSaveRL_triggered();

    void on_actionGenerate_all_languages_triggered();

private:
    Ui::MainWindow *ui;
    Automata automata;
    Translator translator;
    int currentAutomaton;
    QActionGroup *method;
    void toggle_interface(bool b);
    void fill_interface();
    void fill_automaton_list();
    void clear_interface();
    void clear_automaton_list();
    void add_state_to_list(State s);
    void add_event_to_list(Event e);
    void add_transition_to_list(Transition t);
    void generateLanguage(Automaton *a);
};

class TableWidgetCheckboxItem: public QTableWidgetItem
{
    public:
    TableWidgetCheckboxItem(const QString &text, int type = Type);
    bool operator< (const QTableWidgetItem &other) const
    {
        return (this->checkState() > other.checkState());
    }
};
#endif // MAINWINDOW_HPP
