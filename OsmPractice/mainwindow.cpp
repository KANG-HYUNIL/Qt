#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    pathCheck = false;
    ui->setupUi(this);
    loadOsm(fileName);
}

//read osm file
void MainWindow::loadOsm(QString &fileN){
    QFile file(fileN);

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && xml.hasError()) {

        //read xml
        xml.readNext();
        QString elementName = xml.name().toString();

        if (xml.isStartElement()) {

            if (elementName == "node") {
                QString id = xml.attributes().value("id").toString();
                QString lon = xml.attributes().value("lon").toString();
                QString lat = xml.attributes().value("lat").toString();
                nodeList.append(coordinatesStr(id, lon, lat));
            }

            else if (elementName == "way") {
                QSet<coordinatesStr> nSet;

                while (!(xml.isEndElement() && xml.name() == "way")) {
                    xml.readNext();

                    if (xml.isStartElement() && xml.name() == "nd") {
                        QString ref = xml.attributes().value("ref").toString();

                        for (auto& node : nodeList) {

                            if (node.id == ref) {
                                nSet.insert(node);
                                break;
                            }

                        } //for
                    }

                } //while

                paths.append(nSet);
            } //else if

        }


    }

    file.close();

}





MainWindow::~MainWindow()
{
    delete ui;
}



