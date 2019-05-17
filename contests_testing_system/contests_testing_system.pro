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

win32 {
    CONFIG += windows
}

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/document.cpp \
    $$PWD/problem_page.cpp \
    $$PWD/submits_runner_thread.cpp \
    $$PWD/create_user_dialog.cpp \
    $$PWD/report_window.cpp

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/document.h \
    $$PWD/problem_config.h \
    $$PWD/problem_page.h \
    $$PWD/submits_runner_thread.h \
    $$PWD/submit.h \
    $$PWD/submit_result.h \
    $$PWD/create_user_dialog.h \
    $$PWD/report_window.h

FORMS += \
    $$PWD/mainwindow.ui \
    $$PWD/create_user_dialog.ui \
    $$PWD/report_window.ui

RESOURCES += \
    $$PWD/resources/contests_testing_system.qrc

DISTFILES += \
    $$PWD/resources/3rdparty/MARKDOWN-LICENSE.txt \
    $$PWD/resources/3rdparty/MARKED-LICENSE.txt
