#include "verifyutilities.h"
#include <QRegularExpression>
VerifyUtilities::VerifyUtilities()
{

}
//暂时没有写正则表达式验证是否有效的语句


// QRegularExpressionMatch match = invalidCharPattern.match(newUserName);

// if (!match.hasMatch()) {
//     ui->textEdit->appendPlainText("**Error: User name cannot contain special characters like ';' or ':'.");
//     return;
// }


bool VerifyUtilities::IsValidUsername(QString username){
    //不能含有、标点符号、或者是纯数字, 或者长度小于四位
    QRegularExpression rx_("[A-Za-z]");//判断是否含有字母
    QRegularExpression rx("^[A-Za-Z0-9]$");//判断是否有其他符号
    if(!rx_.match(username).hasMatch()|| rx.match(username).hasMatch()|| username.length()<4) return false;
    return true;
}

bool VerifyUtilities::IsValidPassword(QString password){
    //长度要够 不能含有1234 必须含有大小写字母
    if(password.length()<6) return false;//长度至少为6位
    QRegularExpression rx("(1234)");
    if(rx.match(password).hasMatch()) return false;//含有1234
    QRegularExpression rx1("[A-Z]");
    QRegularExpression rx2("[a-z]");
    if(!rx1.match(password).hasMatch()|| !rx2.match(password).hasMatch()) return false;//含有大小写字母
    return true;
}
