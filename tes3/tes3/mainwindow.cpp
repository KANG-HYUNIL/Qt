#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->redradio, SIGNAL(clicked()), this, SLOT(on_set_color()));



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_clearTxt_clicked()
{
    // Clear the text in the textEdit widget
    ui->plainTextEdit->clear();
}


void MainWindow::on_blodcheckBox_clicked(bool checked)
{
    // Set the font weight of the text in the textEdit widget
    QFont font = ui->plainTextEdit->font();
    font.setBold(checked);
    ui->plainTextEdit->setFont(font);


}

void MainWindow::on_set_color()
{
   // if redradio is true, set the plainTextEdit color to red
    //if blueraido is true, set the plainTextEdit color to blue
    //if greenradio is true, set the plainTextEdit color to green

    if(ui->redradio->isChecked()){
        ui->plainTextEdit->setStyleSheet("color: red");
    }
    else if(ui->blueradio->isChecked()){
        ui->plainTextEdit->setStyleSheet("color: blue");
    }
    else{
        ui->plainTextEdit->setStyleSheet("color: black");
    }


}

