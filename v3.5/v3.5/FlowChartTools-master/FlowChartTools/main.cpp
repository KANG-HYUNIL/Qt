﻿#include "mainwindow.h"
#include <QApplication>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.hide();

    return a.exec();
}