#ifndef REPORT_WINDOW_H
#define REPORT_WINDOW_H

#include "submit_result.h"

#include <QDialog>

namespace Ui {
class ReportWindow;
}

class ReportWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ReportWindow(QWidget *parent, SubmitResult *result);
    ~ReportWindow();

private:
    Ui::ReportWindow *ui;
};

#endif // REPORT_WINDOW_H
