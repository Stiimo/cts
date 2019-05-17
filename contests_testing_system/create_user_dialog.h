#ifndef CREATE_USER_DIALOG_H
#define CREATE_USER_DIALOG_H

#include <QDialog>

namespace Ui {
class CreateUserDialog;
}

class CreateUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateUserDialog(QWidget *parent = nullptr);
    ~CreateUserDialog();

    QString get_name();

private:
    Ui::CreateUserDialog *ui;
};

#endif // CREATE_USER_DIALOG_H
