#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "problem_config.h"
#include "document.h"
#include "submits_runner_thread.h"

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void read_contest_settings();
    void save_results();
    void show_report(SubmitResult *result);

    Ui::MainWindow *ui;
    QString contest_id_ = "";
    ProblemConfig problem_;
    Document content;
    SubmitsRunnerThread thread_;
    int total_submits_ = 0;
    QMutex mutex_;
    QVector<SubmitResult*> results_;
    QVector<SubmitResult*> submits_;
    QFile user_file_;

private slots:
    void load_problem(QString const& problem_id);
    void submit_solution();
    void add_new_submit(Submit *submit);
    void update_submit(SubmitResult *result);
    void open_source();
    void show_todo_list(bool);
};

#endif // MAINWINDOW_H
