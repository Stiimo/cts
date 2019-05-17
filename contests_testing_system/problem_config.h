#ifndef PROBLEM_CONFIG_H
#define PROBLEM_CONFIG_H

#include <QString>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

struct ProblemConfig {
    QString id = "";
    QString dir = "";
    QString long_name = "";
    QString checker = "";
    int score = 0;
    int time_limit = 0;
    int memory_limit = 0;

    inline static ProblemConfig fromJson(QString const& path, QString const& id, QJsonParseError & error) {
        ProblemConfig problem;
        problem.id = id;
        problem.dir = path + id;
        QFile file(problem.dir + "/header.json");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QJsonDocument config = QJsonDocument::fromJson(file.readAll(), &error);
        file.close();
        if (error.error != QJsonParseError::NoError) {
            return problem;
        }
        QJsonObject json_conf = config.object();
        problem.long_name = json_conf["long_name"].toString();
        problem.checker = json_conf["checker"].toString();
        problem.score = json_conf["score"].toInt();
        problem.time_limit = json_conf["time_limit"].toInt();
        problem.memory_limit = json_conf["memory_limit"].toInt();
        return problem;
    }
};

#endif // PROBLEM_CONFIG_H
