#ifndef TYPE_CHOICE_HPP
#define TYPE_CHOICE_HPP

#include <QDialog>
#include "automata.hpp"

namespace Ui {
class type_choice;
}

class Type_choice : public QDialog
{
    Q_OBJECT

public:
    explicit Type_choice(QWidget *parent = nullptr);
    ~Type_choice();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::type_choice *ui;
    void selection();
};

#endif // TYPE_CHOICE_HPP
