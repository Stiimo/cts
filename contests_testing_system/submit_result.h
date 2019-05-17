#ifndef RESULT_H
#define RESULT_H

#include <QString>
#include <QStringList>
#include <QVariantMap>

struct SubmitResult {
    int run_id = 0;
    int score = 0;
    int count = 0;
    QString problem_id = "";
    QString status = "N/A";
    QStringList tests;

    inline QVariantMap toVariantMap() const {
        QVariantMap map;
        map["run_id"] = QVariant(run_id);
        map["score"] = QVariant(score);
        map["count"] = QVariant(count);
        map["problem_id"] = QVariant(problem_id);
        map["status"] = QVariant(status);
        map["tests"] = QVariant(tests);
        return map;
    }

    inline static SubmitResult fromQVariantMap(QVariantMap map) {
        SubmitResult result;
        result.run_id = map["run_id"].toInt();
        result.score = map["score"].toInt();
        result.count = map["count"].toInt();
        result.problem_id = map["problem_id"].toString();
        result.status = map["status"].toString();
        result.tests = map["tests"].toStringList();
        return result;
    }
};

#endif // RESULT_H
