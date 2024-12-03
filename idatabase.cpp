#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE"); // 添加 SQL LITE数据库驱动
    QString aFile = "D:/QT/QtProject/Lab3/lab3.db";
    database.setDatabaseName(aFile); // 设置数据库名称

    if(!database.open()){
        qDebug() << "failed to open database";
    } else
        qDebug() << "open database is ok";
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit); // 数据保存方式，OnManualSubmit、OnRowChange
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder); // 排序
    if(!patientTabModel->select())
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),QModelIndex());
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));
    patientTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex();
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QSqlQuery query;
    query.prepare("select username,password from user where username = :USER");
    query.bindValue(":USER",userName);
    query.exec();
    if(query.first() && query.value("username").isValid()){
        QString passwd = query.value("password").toString();
        if(passwd == password){
            qDebug() << "loginOK";
            return "loginOK";
        } else {
            qDebug() << "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }
    query.first();
}

IDatabase::IDatabase(QObject *parent): QObject{parent}
{
    ininDatabase();
}
