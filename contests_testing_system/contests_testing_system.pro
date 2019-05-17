#-------------------------------------------------
#
# Project created by QtCreator 2019-04-06T20:36:22
#
#-------------------------------------------------

QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = contests_testing_system
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    document.cpp \
    problem_page.cpp \
    submits_runner_thread.cpp \
    create_user_dialog.cpp \
    report_window.cpp

HEADERS += \
        mainwindow.h \
    document.h \
    problem_config.h \
    problem_page.h \
    submits_runner_thread.h \
    submit.h \
    submit_result.h \
    create_user_dialog.h \
    report_window.h

FORMS += \
        mainwindow.ui \
    create_user_dialog.ui \
    report_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/contests_testing_system.qrc

DISTFILES += \
    resources/3rdparty/MARKDOWN-LICENSE.txt \
    resources/3rdparty/MARKED-LICENSE.txt
