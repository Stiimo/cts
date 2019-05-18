#include "report_window.h"
#include "ui_report_window.h"

#include <QAbstractButton>

ReportWindow::ReportWindow(QWidget *parent, SubmitResult *result) :
    QDialog(parent),
    ui(new Ui::ReportWindow)
{
    ui->setupUi(this);
    setWindowTitle("Submit #" + QString::number(result->run_id - 1) + " result");
    ui->tests_table->setRowCount(result->tests.size());
    for (int i = 0; i < result->tests.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem(result->tests[i]);
        ui->tests_table->setItem(i, 0, item);
    }
    connect(ui->ok_button, &QAbstractButton::released, this, &QDialog::accept);
}

ReportWindow::~ReportWindow()
{
    delete ui;
}
