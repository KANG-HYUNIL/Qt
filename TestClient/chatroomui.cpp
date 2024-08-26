#include "chatroomui.h"
#include "ui_chatroomui.h"

chatroomui::chatroomui(const QString &roomId, const QString &roomName, const QStringList &userNames, QWidget *parent = nullptr)
    : QWidget(parent), roomId(roomId), roomName(roomName), userNames(userNames)
{


    QVBoxLayout *layout = new QVBoxLayout(this);

    QString text;

    QLabel *roomLabel = new QLabel;
    text = "Room : " + roomName;
    if (text.length() > 15) {
        text = text.left(12) + "...";
    }
    roomLabel->setText(text);
    roomLabel->setFixedWidth(250);
    layout->addWidget(roomLabel);

    // QTabWidget *roomWidget = new QTabWidget;
    // roomWidget->addTab(new QWidget, "Room : " + roomName);
    // roomWidget->setFixedWidth(250);
    // roomWidget->setElideMode(Qt::ElideRight);
    // layout->addWidget(roomWidget);

    QLabel *userLabel = new QLabel;
    text = "Users : " + userNames.join(", ");
    if (text.length() > 15) {
        text = text.left(12) + "...";
    }
    userLabel->setText(text);
    userLabel->setFixedWidth(250);
    layout->addWidget(roomLabel);


    // QTabWidget *userWidget = new QTabWidget;
    // userWidget->addTab(new QWidget, "Users : " + userNames.join(", "));
    // userWidget->setFixedWidth(250);
    // userWidget->setElideMode(Qt::ElideRight);
    // layout->addWidget(userWidget);

    setLayout(layout);
}
QString chatroomui::getRoomId() const {
    return roomId;
}

chatroomui::~chatroomui()
{
    delete ui;
}
