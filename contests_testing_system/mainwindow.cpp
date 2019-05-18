#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "problem_page.h"
#include "create_user_dialog.h"
#include "report_window.h"

#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebChannel>
#include <QtAlgorithms>
#include <QFileDialog>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        thread_(parent) {
    ui->setupUi(this);

    ProblemPage *page = new ProblemPage(this);
    ui->problem_text->setPage(page);
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &content);
    page->setWebChannel(channel);
    ui->problem_text->setUrl(QUrl("qrc:/index.html"));
//    QAction *todo = ui->menu_bar->addAction("TODO");
    QAction *quit = ui->menu_bar->addAction("Quit");
    QAction *dump = ui->menu_admin->addAction("Download results");
    QAction *reset = ui->menu_admin->addAction("Reset user");

//    connect(todo, &QAction::triggered, this, &MainWindow::show_todo_list);
    connect(quit, &QAction::triggered, this, [this](bool){ close(); });
    connect(dump, &QAction::triggered, this, &MainWindow::dump_results);
    connect(reset, &QAction::triggered, this, &MainWindow::show_todo_list);
    connect(ui->problems_box, QOverload<QString const&>::of(&QComboBox::currentIndexChanged), this, &MainWindow::load_problem);
    connect(ui->submit_button, &QAbstractButton::released, this, &MainWindow::submit_solution);
    connect(&thread_, &SubmitsRunnerThread::submit_added, this, &MainWindow::add_new_submit);
    connect(&thread_, &SubmitsRunnerThread::run_finished, this, &MainWindow::update_submit);
    connect(ui->open_button, &QAbstractButton::released, this, &MainWindow::open_source);

    read_contest_settings();
    if (!centralWidget()->isEnabled()) {
        return;
    }
    ui->problems_box->setCurrentIndex(0);
    load_problem(ui->problems_box->currentText());
    thread_.start();

    #ifdef WIN32
        QDir user_dir = QDir::currentPath();
    #else
        QDir user_dir = QDir::home();
    #endif
    QString sep = QDir::separator();
    user_dir.mkpath(".cts" + sep + "user" + sep + contest_id_);
    user_dir.cd(".cts" + sep + "user" + sep + contest_id_);
    user_file_.setFileName(user_dir.filePath("config.json"));
    QJsonParseError error;
    QJsonObject json_config;
    if (user_file_.exists()) {
        user_file_.open(QIODevice::ReadOnly | QIODevice::Text);
        json_config = QJsonDocument::fromJson(user_file_.readAll(), &error).object();
        user_file_.close();
    }
    if (!user_file_.exists() || error.error != QJsonParseError::NoError) {
        CreateUserDialog dialog;
        dialog.exec();
        if (dialog.result() == QDialog::Accepted) {
            ui->student_label->setText(dialog.get_name());
        } else {
            centralWidget()->setEnabled(false);
            return;
        }
        user_file_.open(QIODevice::WriteOnly | QIODevice::Text);
        user_file_.write(("{\n    \"name\": " + ui->student_label->text() + "\n}\n").toStdString().c_str());
        user_file_.close();
    } else {
        ui->student_label->setText(json_config["name"].toString());
    }
    if (json_config.contains("results")) {
        for (auto json_result : json_config["results"].toArray()) {
            SubmitResult result = SubmitResult::fromQVariantMap(json_result.toVariant().toMap());
            int pos = static_cast<int>(result.problem_id.toStdString()[0] - 'A');
            SubmitResult *tmp = results_[pos];
            tmp->run_id = result.run_id;
            tmp->score = result.score;
            tmp->count = result.count;
            tmp->problem_id = result.problem_id;
            tmp->status = result.status;
            tmp->tests = result.tests;
            ui->results_table->item(pos, 1)->setText(QString::number((tmp->score)));
            if (tmp->status == "OK") {
                ui->results_table->item(pos, 0)->setBackgroundColor(Qt::green);
                ui->results_table->item(pos, 1)->setBackgroundColor(Qt::green);
            } else if (tmp->status == "Incomplete solution") {
                ui->results_table->item(pos, 0)->setBackgroundColor(Qt::red);
                ui->results_table->item(pos, 1)->setBackgroundColor(Qt::red);
            }
        }
    }
    if (json_config.contains("submits")) {
        for (auto json_submit : json_config["submits"].toArray()) {
            SubmitResult submit = SubmitResult::fromQVariantMap(json_submit.toVariant().toMap());
            SubmitResult *tmp = new SubmitResult(submit);
            ui->submits_table->insertRow(0);
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(tmp->run_id));
            ui->submits_table->setItem(0, 0, item);
            item = new QTableWidgetItem(tmp->problem_id);
            ui->submits_table->setItem(0, 1, item);
            item = new QTableWidgetItem(tmp->status);
            ui->submits_table->setItem(0, 2, item);
            item = new QTableWidgetItem(QString::number(tmp->score));
            ui->submits_table->setItem(0, 3, item);
            QPushButton *button = new QPushButton("Show");
            ui->submits_table->setCellWidget(0, 4, button);
            connect(button, &QAbstractButton::released, this, [this, tmp](){ show_report(tmp); });
            submits_.append(tmp);
            ++total_submits_;
        }
    }
}

MainWindow::~MainWindow() {
    thread_.requestInterruption();
    thread_.wake();
    thread_.wait();
    if (centralWidget()->isEnabled()) {
        save_results();
        for (SubmitResult * submit : submits_) {
            delete submit;
        }
        for (SubmitResult * result : results_) {
            delete result;
        }
    }
    delete ui;
}

void MainWindow::read_contest_settings() {
    QString sep = QDir::separator();
    QFile file(".contest" + sep + "contest_config.json");
    if (!file.exists()) {
        QMessageBox::critical(this, "Load contest", "Contest config file was not found.");
        centralWidget()->setEnabled(false);
        return;
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError error;
    QJsonDocument config = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();
    if (error.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Load contest", "Config file has been corrupted\nError: " + error.errorString());
        centralWidget()->setEnabled(false);
        return;
    }
    QJsonObject json_conf = config.object();
    contest_id_ = json_conf["contest_id"].toString();
    ui->contest_label->setText(json_conf["contest_name"].toString());
    QJsonArray problems = json_conf["problems"].toArray();
    ui->results_table->setRowCount(problems.size());
    int i = 0;
    for (auto problem : problems){
        ui->problems_box->addItem(problem.toString());
        SubmitResult *result = new SubmitResult();
        result->problem_id = problem.toString();
        results_.push_back(result);
        QTableWidgetItem *item = new QTableWidgetItem(result->problem_id);
        ui->results_table->setItem(i, 0, item);
        item = new QTableWidgetItem("0");
        ui->results_table->setItem(i, 1, item);
        ++i;
    }
}

void MainWindow::load_problem(QString const& problem_id) {
    QString sep = QDir::separator();
    QJsonParseError error;
    problem_ = ProblemConfig::fromJson(".contest" + sep + contest_id_ + sep, problem_id, error);
    if (error.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Load problem", "Config file has been corrupted\nError: " + error.errorString());
        ui->problems_box->removeItem(ui->problems_box->currentIndex());
        return;
    }
    ui->problem_header->item(0, 0)->setText(problem_.long_name);
    ui->problem_header->item(0, 1)->setText(QString::number(problem_.score));
    ui->problem_header->item(0, 2)->setText(QString::number(problem_.time_limit));
    ui->problem_header->item(0, 3)->setText(QString::number(problem_.memory_limit));
    ui->problem_header->item(0, 4)->setText(problem_.checker);

    QFile statement(problem_.dir + sep + "statement.md");
    statement.open(QIODevice::ReadOnly | QIODevice::Text);
    content.setText(QString::fromUtf8(statement.readAll()));
}

void MainWindow::submit_solution() {
    QString sep = QDir::separator();
    Submit *submit = new Submit();
    submit->id = total_submits_++;
    submit->config.id = ui->problems_box->currentText();
    submit->config.dir = ".contest" + sep + contest_id_ + sep + submit->config.id;
    submit->config.score = ui->problem_header->item(0, 1)->text().toInt();
    submit->config.time_limit = ui->problem_header->item(0, 2)->text().toInt();
    submit->config.memory_limit = ui->problem_header->item(0, 3)->text().toInt();
    submit->config.checker = ui->problem_header->item(0, 4)->text();
    if (ui->file_path->text().isEmpty()) {
        submit->source = ui->source_code->toPlainText().toUtf8();
    } else {
        QFile file(ui->file_path->text());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        submit->source = file.readAll();
        file.close();
    }
    ui->file_path->setText("");
    ui->source_code->setPlainText("");
    thread_.add_submit(submit);
}

void MainWindow::add_new_submit(Submit *submit) {
    QMutexLocker locker(&mutex_);
    ui->submits_table->insertRow(0);
    QTableWidgetItem *item = new QTableWidgetItem(QString::number(submit->id));
    ui->submits_table->setItem(0, 0, item);
    item = new QTableWidgetItem(submit->config.id);
    ui->submits_table->setItem(0, 1, item);
    item = new QTableWidgetItem("N/A");
    ui->submits_table->setItem(0, 2, item);
    item = new QTableWidgetItem("N/A");
    ui->submits_table->setItem(0, 3, item);
    item = new QTableWidgetItem("N/A");
    ui->submits_table->setItem(0, 4, item);
}

void MainWindow::update_submit(SubmitResult *result) {
    QMutexLocker locker(&mutex_);
    int pos = static_cast<int>(result->problem_id.toStdString()[0] - 'A');
    SubmitResult *tmp = results_[pos];
    if (tmp->status != "OK") {
        tmp->status = result->status;
    }
    int score = qMax(result->score - tmp->count++, 0);
    tmp->score = qMax(score, tmp->score);
    int row = ui->submits_table->rowCount() - result->run_id - 1;
    ui->submits_table->item(row, 2)->setText(result->status);
    ui->submits_table->item(row, 3)->setText(QString::number(score));
    QTableWidgetItem *item = ui->submits_table->item(row, 4);
    delete item;
    QPushButton *button = new QPushButton("Show");
    ui->submits_table->setCellWidget(row, 4, button);
    connect(button, &QAbstractButton::released, this, [this, result](){ show_report(result); });
    ui->results_table->item(pos, 1)->setText(QString::number((tmp->score)));
    if (tmp->status == "OK") {
        ui->results_table->item(pos, 0)->setBackgroundColor(Qt::green);
        ui->results_table->item(pos, 1)->setBackgroundColor(Qt::green);
    } else if (tmp->status == "Incomplete solution") {
        ui->results_table->item(pos, 0)->setBackgroundColor(Qt::red);
        ui->results_table->item(pos, 1)->setBackgroundColor(Qt::red);
    }
    submits_.append(result);
    save_results();
}

void MainWindow::open_source() {
    ui->file_path->setText(QFileDialog::getOpenFileName(this, "Open file", "~", "Python script (*.py);;All files (*)"));
}

void MainWindow::save_results() {
    QJsonObject json_config;
    json_config["name"] = QJsonValue(ui->student_label->text());
    QVariantList json_submits;
    for (SubmitResult * submit : submits_) {
        json_submits.append(QVariant::fromValue(submit->toVariantMap()));
    }
    QJsonObject obj = json_submits[0].toJsonObject();
    json_config["submits"] = QJsonArray::fromVariantList(json_submits);
    QVariantList json_results;
    for (SubmitResult * result : results_) {
        json_results.append(QVariant::fromValue(result->toVariantMap()));
    }
    json_config["results"] = QJsonArray::fromVariantList(json_results);
    QJsonDocument document(json_config);
    user_file_.open(QIODevice::WriteOnly | QIODevice::Text);
    user_file_.write(document.toJson());
    user_file_.close();
}

void MainWindow::show_todo_list(bool) {
    qDebug()<< "\n"
            << "Delete this and qdebug\n"
            << "Add protocol\n"
            << "Add admin functions\n"
            << "Prepare contest\n"
            << "Add report buttons on load\n"
            ;
}

void MainWindow::show_report(SubmitResult *result) {
    ReportWindow report(this, result);
    report.exec();
}

void MainWindow::dump_results(bool) {
    QString sep = QDir::separator();
    QDir dump_dir("report");
    if (dump_dir.exists()) {
        dump_dir.removeRecursively();
    }
    QDir::current().mkdir("report");
    if (!user_file_.copy("report" + sep + "config.json")) {
        QMessageBox::critical(this, "Download resuts", "Cannot copy some files.");
        return;
    }
    QDir runs(".runs");
    QStringList files = runs.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++) {
        QString srcName = ".runs" + sep + files[i];
        QString destName = "report" + sep + files[i];
        bool success = QFile::copy(srcName, destName);
        if (!success) {
            QMessageBox::critical(this, "Download resuts", "Cannot copy some files.");
            return;
        }
    }
    QMessageBox::information(this, "Download resuts", "Finished.");
}
