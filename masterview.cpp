#include "masterview.h"
#include "ui_masterview.h"
#include <QDebug>
#include "idatabase.h"

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);

    this->setWindowTitle(" "); // 设置窗口标题

    goLoginView();

    IDatabase::getInstance();
    // this->setWindowFlags(Qt::FramelessWindowHint); // 顶部框
    // this->setWindowFlags(Qt::Tool);// 设置窗口为工具窗口，这将去除标题栏和图标
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goRegisterView()
{
    registerView = new RegisterView(this);
    pushWidgetToStackView(registerView);

    connect(registerView, &RegisterView::goPreviousView,this,&MasterView::goPreviousView);
}

void MasterView::goLoginView()
{
    qDebug() << "goLoginView";
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccuss()), this, SLOT(goWelcomView()));
    connect(loginView, &LoginView::goRegisterView,this,&MasterView::goRegisterView);
}

void MasterView::goWelcomView()
{
    qDebug() << "goWelcomeView";
    welcomeView = new WelcomeView(this);
    pushWidgetToStackView(welcomeView);

    connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
    connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
    connect(welcomeView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
}

void MasterView::goDoctorView()
{
    qDebug() << "goDoctorView";
    doctorView = new DoctorView(this);
    pushWidgetToStackView(doctorView);

    connect(doctorView, &DoctorView::goDoctorEditView, this, &MasterView::goDoctorEditView);
}

void MasterView::goDoctorEditView(int index)
{
    qDebug() << "goDoctorEditView";
    doctorEditView = new DoctorEditView(this,index);
    pushWidgetToStackView(doctorEditView);

    connect(doctorEditView,SIGNAL(goPreviousView()),this,SLOT(goPreviousView()));
}

void MasterView::goDepartmentView()
{
    qDebug() << "goDepartmentView";
    departmentView = new DepartmentView(this);
    pushWidgetToStackView(departmentView);
}

void MasterView::goPatientView()
{
    qDebug() << "goPatientView";
    patientView = new PatientView(this);
    pushWidgetToStackView(patientView);

    connect(patientView, SIGNAL(goPatientEditView(int)), this, SLOT(goPatientEditView(int)));
}

void MasterView::goPatientEditView(int rowNo)
{
    qDebug() << "goPatientEditView";
    patientEditView = new PatientEditView(this, rowNo);
    pushWidgetToStackView(patientEditView);

    connect(patientEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goPreviousView()
{
    int count = ui->stackedWidget->count();

    if(count > 1){
        ui->stackedWidget->setCurrentIndex(count -2);
        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        QWidget *widget = ui->stackedWidget->widget(count - 1);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count - 1); // 总是显示最新加入的view
    ui->labelTitle->setText(widget->windowTitle());
}

void MasterView::on_btBack_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int count = ui->stackedWidget->count();
    if (count > 1)
        ui->btBack->setEnabled(true);
    else
        ui->btBack->setEnabled(false);

    QString title = ui->stackedWidget->currentWidget()->windowTitle();

    if(title == "欢迎")
        ui->btBack->setEnabled(false);
    if(title == "登录" or title == "注册")
        ui->btLogout->setEnabled(false);

    ui->btLogout->setEnabled(true);
}


void MasterView::on_btLogout_clicked()
{
    goPreviousView();
}

