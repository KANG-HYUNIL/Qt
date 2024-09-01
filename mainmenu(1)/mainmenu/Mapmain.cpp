#include "BitMap.h"
#include <QApplication>
#include <QFile>

resolve_xml XML;

int mapmain(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString qss;
    QFile qssFile(":/qrc/qss/myQss.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
      qss = QLatin1String(qssFile.readAll());
      a.setStyleSheet(qss);
      qssFile.close();
    }

    XML.imortant();
    BitMap w;
    w.show();

    return a.exec();

}
