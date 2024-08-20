#include "mainwindow.h" //헤더 파일 포함

#include <QApplication> //GUI 어플리케이션에 필수적인 QApplication 포함(환경, 이벤트 관리 등)

//헤더 파일은 클래스의 전체 구조를 정의하는 파일
//실행 파일(cpp)는 선언된 함수, 클래스에 대한 구체적 구현을 표시하는 파일
//클래스의 구조 정의를 한 파일에서 전부 해도 되나, 재사용성이 높은 클래스는 정의외 구체적 구현을 분리하는 것이 효율적

//main 함수, argc는 인수의 개수, argv는 인수의 문자열 배열
int main(int argc, char *argv[])
{
    QApplication a(argc, argv); //QApplication 객체 a 생성, 프로그램 실행에 필수적
    MainWindow w; //MainWindow 객체 w 생성, MainWindow 클래스의 생성자 호출
    w.show(); //윈도우 표시
    qDebug() << "Qt"; //
    return a.exec(); //어플리케이션 종료 시 return
}
