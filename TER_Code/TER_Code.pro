#-------------------------------------------------
#
# Project created by QtCreator 2019-02-08T10:55:48
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TER_Code
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

CONFIG += c++14

SOURCES += \
    create_automaton_dialog.cpp \
    delete_automaton_dialog.cpp \
    delete_event_dialog.cpp \
    delete_state_dialog.cpp \
    delete_transition_dialog.cpp \
    help_dialog.cpp \
        main.cpp \
        mainwindow.cpp \
    automata.cpp \
    automaton.cpp \
    table_widget_checkbox_item.cpp \
    type_choice.cpp \
    translator.cpp \
    create_state_dialog.cpp \
    create_event_dialog.cpp \
    create_transition_dialog.cpp

HEADERS += \
    create_automaton_dialog.hpp \
    delete_automaton_dialog.hpp \
    delete_event_dialog.hpp \
    delete_state_dialog.hpp \
    delete_transition_dialog.hpp \
    help_dialog.hpp \
        mainwindow.hpp \
    automata.hpp \
    automaton.hpp \
    table_widget_checkbox_item.hpp \
    type_choice.hpp \
    translator.hpp \
    create_state_dialog.hpp \
    create_event_dialog.hpp \
    create_transition_dialog.hpp

FORMS += \
    create_automaton_dialog.ui \
    delete_automaton_dialog.ui \
    delete_event_dialog.ui \
    delete_state_dialog.ui \
    delete_transition_dialog.ui \
    help_dialog.ui \
        mainwindow.ui \
    type_choice.ui \
    create_state_dialog.ui \
    create_event_dialog.ui \
    create_transition_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
