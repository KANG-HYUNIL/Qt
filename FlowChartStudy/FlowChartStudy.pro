QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chart_label.cpp \
    chart_line.cpp \
    chart_parent.cpp \
    chart_plaintext.cpp \
    flowchart.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    chart_label.h \
    chart_line.h \
    chart_parent.h \
    chart_plaintext.h \
    enums.h \
    flowchart.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
