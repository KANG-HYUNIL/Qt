#include "mainwindow.h"
#include "ui_mainwindow.h"

//생성자 호출,
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) //QMainWindow 생성자 호출(상속받기에 부모 클래스 호출)

    //MainWindow의 멤버 변수 ui를 초기화, Ui::MainWindow 생성 후 ui에 할당
    //이를 통해 ui 변수는 Ui::MainWindow를 가리키게 됨
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
