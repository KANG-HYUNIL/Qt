#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_clearTxt_clicked();

    void on_blodcheckBox_clicked(bool checked);

    //declare set color slot function

    void on_set_color();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
