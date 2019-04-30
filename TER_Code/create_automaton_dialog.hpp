#ifndef CREATE_AUTOMATON_DIALOG_HPP
#define CREATE_AUTOMATON_DIALOG_HPP

#include <QDialog>
#include "automata.hpp"

namespace Ui {
class Create_automaton_dialog;
}

class Create_automaton_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Create_automaton_dialog(int, QMap<int, Automaton> , QWidget *parent = nullptr);
    ~Create_automaton_dialog();

private slots:
    void on_buttonBox_accepted();

signals:
    void creation_automaton(Automaton);

private:
    Ui::Create_automaton_dialog *ui;
    QMap<int, Automaton> automatonList;
    int idAutomaton;
};

#endif // CREATE_AUTOMATON_DIALOG_HPP
