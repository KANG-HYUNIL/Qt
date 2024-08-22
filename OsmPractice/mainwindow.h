#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //void drawMap();
    void loadOsm(QString &fileN);

private:
    Ui::MainWindow *ui;

    QString fileName = ":/data/map (3)(1).osm";

    struct coordinatesStr
    {
        QString id;
        QString lon;
        QString lat;


        coordinatesStr() {}

        coordinatesStr(const QString &id, QString &lon, QString &lat)
            : id(id), lon(lon), lat(lat) {}

        bool operator==(const coordinatesStr &other) const {
            return id == other.id;
        }
    };

    QList<coordinatesStr> nodeList;
    QSet<coordinatesStr> nodeSet;
    QList<QSet<coordinatesStr>> paths;
    bool pathCheck;

    friend uint qHash(coordinatesStr &key, uint seed = 0) {
        return qHash(key.id, seed);
    }

};
#endif // MAINWINDOW_H
