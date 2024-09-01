QT       += core gui multimedia multimediawidgets network
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatdialog.cpp \
    client.cpp \
    home.cpp \
    main.cpp \
    mainwindow.cpp \
    Clock.cpp \
    ImageBrowser.cpp \
    imagewidget.cpp \
    musicplayer.cpp \
    receivefiledialog.cpp \
    registerdialog.cpp \
    sendfiledialog.cpp \
    userinfo.cpp \
    videoplayer.cpp \
    BitMap.cpp \
    wea_ther.cpp \


HEADERS += \
    chatdialog.h \
    client.h \
    home.h \
    mainwindow.h \
    Clock.h \
    ImageBrowser.h \
    imagewidget.h \
    musicplayer.h \
    receivefiledialog.h \
    registerdialog.h \
    sendfiledialog.h \
    userinfo.h \
    videoplayer.h \
    BitMap.h \
    wea_ther.h \



FORMS += \
    chatdialog.ui \
    client.ui \
    home.ui \
    mainwindow.ui \
    Clock.ui \
    ImageBrowser.ui \
    musicplayer.ui \
    receivefiledialog.ui \
    registerdialog.ui \
    sendfiledialog.ui \
    videoplayer.ui \
    BitMap.ui \
    wea_ther.ui \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS += -fpermissive

RESOURCES += \
    Client.qrc \
    image.qrc \
    qss.qrc \
    icon.qrc \
    weathericon.qrc \

