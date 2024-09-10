#include "dialog.h"
#include "ui_dialog.h"
#include "mainwindow.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("流程图编辑器");
    //MainWindow* mainwindow = new MainWindow();
    connect(ui->createNew,SIGNAL(clicked()),this,SLOT(createNew()));
    connect(ui->OpenExisting,SIGNAL(clicked()),this,SLOT(openFile()));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::createNew(){
    this->hide();
    emit create();
}

void Dialog::openFile(){
    this->hide();
    emit openExisting();
}
