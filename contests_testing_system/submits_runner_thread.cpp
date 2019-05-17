#include "submits_runner_thread.h"

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QDebug>

void SubmitsRunnerThread::run() {
    while (!isInterruptionRequested()) {
        if (submits_.isEmpty()) {
            mutex_.lock();
            waiter_.wait(&mutex_);
            mutex_.unlock();
        } else {
            Submit *submit = submits_.dequeue();
            run_submit(submit);
            delete submit;
        }
    }
}

SubmitsRunnerThread::~SubmitsRunnerThread() {
    for (auto item : submits_) {
        delete item;
    }
    submits_.clear();
}

void SubmitsRunnerThread::add_submit(Submit *submit) {
    submits_.enqueue(submit);
    emit submit_added(submit);
    waiter_.wakeOne();
}

void SubmitsRunnerThread::wake() {
    waiter_.wakeOne();
}

void SubmitsRunnerThread::run_submit(Submit *submit) {
    QString sep = QDir::separator();
    QString file_path = ".runs" + sep + QString::number(submit->id) + "_" + submit->config.id + ".py";
    QFile source(file_path);
    source.open(QIODevice::WriteOnly | QIODevice::Text);
    source.write(submit->source);
    source.setPermissions(source.permissions() | QFileDevice::ExeUser);
    source.close();
    QDir dir(submit->config.dir + sep + ".tests");
    QStringList tests = dir.entryList(QStringList() << "*.i", QDir::Files, QDir::Name);
    QStringList answers = dir.entryList(QStringList() << "*.a", QDir::Files, QDir::Name);
    SubmitResult *result = new SubmitResult();
    result->problem_id = submit->config.id;
    result->run_id = submit->id;
    for (int i = 0; i < tests.size(); ++i) {
        QObject thread_parent;
        QProcess process(&thread_parent);
        #ifdef WIN32
            process.setProgram("python/python.exe");
            process.setArguments(QStringList() << file_path);
        #else
            process.setProgram("/bin/sh");
            process.setArguments(QStringList() << "-c" << "ulimit -v " + QString::number(submit->config.memory_limit * 1024) +
                               "; timeout " + QString::number(submit->config.time_limit) + " python3 " + file_path);
        #endif
        process.setStandardInputFile(dir.path() + sep + tests[i]);
        process.setStandardOutputFile("./.tmp" + sep + "output.txt");
        process.setWorkingDirectory(".");
        process.start();
        process.waitForStarted(-1);
        process.waitForFinished(-1);
        int code = process.exitCode();
        switch (code) {
            case 1: {
                QString error = QString::fromUtf8(process.readAllStandardError());
                if (error.contains("MemoryError")) {
                    result->tests << "Memory Limit";
                } else {
                    result->tests << "Runtime Error";
                }
                result->status = "Incomplete solution";
                break;
            }
            case 124: {
                result->tests << "Time Limit";
                result->status = "Incomplete solution";
                break;
            }
            case 0: {
                QString verdict;
                if (check(verdict, dir.path() + sep + tests[i], ".tmp" + sep + "output.txt", dir.path() + sep + answers[i],
                          submit->config.checker)) {
                   ++result->score;
                } else {
                    result->status = "Incomplete solution";
                }
                result->tests << verdict;
                break;
            }
            default:
                throw std::runtime_error("Unknown exit error: " + std::to_string(code));
        }
    }
    if (result->status == "N/A") {
        result->status = "OK";
        result->score = submit->config.score;
    }
    emit run_finished(result);
}

bool SubmitsRunnerThread::check(QString & verdict, QString /*input*/, QString output, QString answer, QString checker) {
    QString sep = QDir::separator();
    QObject thread_parent;
    QProcess process(&thread_parent);
    if (checker != "custom") {
        #ifdef WIN32
            process.setProgram(".checkers" + sep + checker + ".exe");
        #else
            process.setProgram(".checkers" + sep + checker);
        #endif
    } else {}
    process.setArguments(QStringList() << output << answer);
    process.setWorkingDirectory(".");
    process.start();
    process.waitForStarted(-1);
    process.waitForFinished(-1);
    int code = process.exitCode();
    switch (code) {
        case 0:
            verdict = "OK";
            break;
        case 1:
            verdict = "Wrong Answer";
            break;
        case 2:
            verdict = "Presentation Error";
            break;
        default:
            throw std::runtime_error("Unknown checker result: " + std::to_string(code));
    }
    return code == 0;
}
