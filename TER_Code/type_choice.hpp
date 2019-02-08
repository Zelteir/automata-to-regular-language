#ifndef TYPE_CHOICE_HPP
#define TYPE_CHOICE_HPP

#include <QDialog>

namespace Ui {
class type_choice;
}

class type_choice : public QDialog
{
    Q_OBJECT

public:
    explicit type_choice(QWidget *parent = nullptr);
    ~type_choice();

private:
    Ui::type_choice *ui;
};

#endif // TYPE_CHOICE_HPP
