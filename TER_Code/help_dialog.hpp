#ifndef HELP_DIALOG_HPP
#define HELP_DIALOG_HPP

#include <QDialog>

namespace Ui {
class Help_dialog;
}

class Help_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Help_dialog(QWidget *parent = nullptr);
    ~Help_dialog();

private:
    Ui::Help_dialog *ui;
};

#endif // HELP_DIALOG_HPP
