#ifndef PROBLEMPAGE_H
#define PROBLEMPAGE_H

#include <QWebEnginePage>

class ProblemPage : public QWebEnginePage {
    Q_OBJECT

public:
    explicit ProblemPage(QObject *parent = nullptr) : QWebEnginePage(parent) {}

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // PROBLEMPAGE_H
