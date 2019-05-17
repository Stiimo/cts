#ifndef SUBMIT_H
#define SUBMIT_H

#include "problem_config.h"

#include <QByteArray>

struct Submit {
    int id;
    QByteArray source;
    ProblemConfig config;
};

#endif // SUBMIT_H
