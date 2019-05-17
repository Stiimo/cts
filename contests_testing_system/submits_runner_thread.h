#ifndef SUBMITS_RUNNER_THREAD_H
#define SUBMITS_RUNNER_THREAD_H

#include "submit.h"
#include "submit_result.h"

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QQueue>

class SubmitsRunnerThread : public QThread {
    Q_OBJECT

public:
    explicit SubmitsRunnerThread(QObject *parent = nullptr) : QThread(parent) {}
    ~SubmitsRunnerThread() override;

    void add_submit(Submit *submit);
    void wake();

signals:
    void submit_added(Submit *result);
    void run_finished(SubmitResult *result);

protected:
    virtual void run() override;

private:
    void run_submit(Submit *submit);
    bool check(QString & verdict, QString input, QString output, QString answer, QString checker);

    QWaitCondition waiter_;
    QMutex mutex_;
    QQueue<Submit*> submits_;
};

#endif // SUBMITS_RUNNER_THREAD_H
