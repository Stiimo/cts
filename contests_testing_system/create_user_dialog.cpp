#include "create_user_dialog.h"
#include "ui_create_user_dialog.h"

CreateUserDialog::CreateUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateUserDialog)
{
    ui->setupUi(this);
}

CreateUserDialog::~CreateUserDialog()
{
    delete ui;
}

QString CreateUserDialog::get_name() {
    return ui->user_name->text();
}
