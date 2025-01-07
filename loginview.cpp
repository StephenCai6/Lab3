#include "loginview.h"
#include "ui_loginview.h"
#include "idatabase.h"
#include <QMessageBox>


LoginView::LoginView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginView)
{
    ui->setupUi(this);
}

LoginView::~LoginView()
{
    delete ui;
}

// 登录
void LoginView::on_btSignIn_clicked()
{
    QString status = IDatabase::getInstance().userLogin(ui->inputUserName->text(), ui->inputUserPassword->text());

    if(status == "loginOK"){
        // 清除错误信息
        ui->statusLabel->clear();
        ui->statusLabel->setStyleSheet(""); // 清除样式
        emit loginSuccuss();
    }
    else if(status == "wrongPassword"){
        // 设置文本为红色
        ui->statusLabel->setStyleSheet("color: red;");
        ui->statusLabel->setText("Wrong Password");
    }
    else if(status == "wrongUsername"){
        // 设置文本为红色
        ui->statusLabel->setStyleSheet("color: red;");
        ui->statusLabel->setText("Wrong Username");
    }
}

// 注册
void LoginView::on_btSignUp_clicked()
{
    emit goRegisterView();
}

// 退出
void LoginView::on_btExit_clicked()
{
    QApplication::quit();
}

