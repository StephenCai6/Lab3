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

// 病人获取数据
bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit); // 数据保存方式，OnManualSubmit、OnRowChange
    patientTabModel->setSort(patientTabModel->fieldIndex("ID"),Qt::AscendingOrder); // 排序
    if(!patientTabModel->select())
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

// 增
int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),QModelIndex());
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    // curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));
    patientTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();
}

// 查
bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

// 删
void IDatabase::deleteCurrentPatient(int rowIndex) {
    qDebug() << "Deleting row at index:" << rowIndex;
    // 直接使用传入的 rowIndex 删除对应的行
    if (patientTabModel->removeRow(rowIndex)) {
        if (patientTabModel->submitAll()) {
            qDebug() << "Row deleted successfully";
            patientTabModel->select(); // 重新加载数据
        } else {
            qDebug() << "Failed to delete row";
        }
    } else {
        qDebug() << "Failed to remove row from model";
    }
}

// 提交
bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

// 撤销
void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

// 用户注册
QString IDatabase::userRegister(int id, QString fullname, QString username, QString password)
{
    QSqlQuery query;

    // 检查用户名是否已被使用
    query.prepare("SELECT username FROM user WHERE username = :USER");
    query.bindValue(":USER", username);
    if (!query.exec()) {
        return "Database query execution failed!";
    }

    qDebug() << query.lastQuery() << query.first();

    // 如果查到已有用户，返回提示信息
    if (query.first() && query.value("username").isValid()) {
        return "Username has been used!";
    } else {
        // 插入新用户
        query.prepare("INSERT INTO user (id, fullname, username, password) VALUES (:ID, :FULLNAME, :USERNAME, :PASSWORD)");
        query.bindValue(":ID", id);
        query.bindValue(":FULLNAME", fullname);
        query.bindValue(":USERNAME", username);
        query.bindValue(":PASSWORD", password); // 实际应用中，密码应该加密存储

        if (query.exec()) {
            return "User registered successfully!";
        } else {
            return "Failed to register user: " + query.lastError().text();
        }
    }
}

// 用户登录
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
            return "wrongPassword";
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

// 导入数据
bool IDatabase::importPatientsFromCSV(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8); // 设置编码为 UTF-8
    QString line = in.readLine(); // 读取表头并忽略

    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    while (!in.atEnd()) {
        line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() != 10) { // 确保数据行有10个字段
            qDebug() << "Incorrect number of fields in line:" << line;
            continue;
        }

        QSqlRecord newRecord = patientTabModel->record();
        newRecord.setValue("ID", fields[0]); // ID 是第一个字段
        newRecord.setValue("ID_CARD", fields[1]);
        newRecord.setValue("NAME", fields[2]);
        newRecord.setValue("SEX", fields[3]);
        newRecord.setValue("DOB", fields[4]);
        newRecord.setValue("HEIGHT", fields[5].toInt());
        newRecord.setValue("WEIGHT", fields[6].toInt());
        newRecord.setValue("MOBILEPHONE", fields[7]);
        newRecord.setValue("AGE", fields[8].toInt());
        newRecord.setValue("CREATEDTIMESTAMP", fields[9]);

        patientTabModel->insertRow(patientTabModel->rowCount());
        patientTabModel->setRecord(patientTabModel->rowCount() - 1, newRecord);
    }

    file.close();

    // 提交所有更改到数据库
    if (!patientTabModel->submitAll()) {
        qDebug() << "Failed to submit changes to the database:" << patientTabModel->lastError();
        return false;
    }

    return true;
}

// 导出数据
void IDatabase::exportPatientsToCSV(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << filePath;
        return;
    }

    QTextStream out(&file);
    // 写入表头
    QStringList headers;
    for (int i = 0; i < patientTabModel->columnCount(); ++i) {
        headers << patientTabModel->headerData(i, Qt::Horizontal).toString();
    }
    out << headers.join(",") << "\n";

    // 遍历数据库中的患者数据并写入文件
    for (int i = 0; i < patientTabModel->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < patientTabModel->columnCount(); ++j) {
            QString fieldValue = patientTabModel->data(patientTabModel->index(i, j)).toString();
            // 检查字段值是否包含逗号或换行符，如果包含，则添加双引号
            if (fieldValue.contains(',') || fieldValue.contains('\n')) {
                row << "\"" << fieldValue << "\"";
            } else {
                row << fieldValue;
            }
        }
        out << row.join(",") << "\n";
    }

    file.close();
    qDebug() << "Patients exported successfully to" << filePath;
}

// 医生获取数据
bool IDatabase::initDoctorModel()
{
    doctorTabModel = new QSqlTableModel(this,database);
    doctorTabModel->setTable("doctor");
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("ID"), Qt::AscendingOrder);
    if(!(doctorTabModel->select()))
        return false;

    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    int lastRow = doctorTabModel->rowCount() - 1;
    QModelIndex lastIndex = doctorTabModel->index(lastRow, 0);
    int lastId = doctorTabModel->data(lastIndex, Qt::EditRole).toInt();
    doctorTabModel->insertRow(doctorTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = doctorTabModel->index(doctorTabModel->rowCount() - 1, 1);
    int curRecNo = curIndex.row();
    QSqlRecord curRec = doctorTabModel->record(curRecNo);
    curRec.setValue("ID", QString::number(lastId + 1));
    doctorTabModel->setRecord(curRecNo, curRec);
    return curIndex.row();
}

bool IDatabase::searchDoctor(QString filter)
{
    doctorTabModel->setFilter(filter);
    return doctorTabModel->select();
}

void IDatabase::deleteCurrentDoctor()
{
    QModelIndex curIdex = theDoctorSelection->currentIndex();
    doctorTabModel->removeRow(curIdex.row());
    doctorTabModel->submitAll();
    doctorTabModel->select();
}

bool IDatabase::submitDoctorEdit()
{
    return doctorTabModel->submitAll();
}

void IDatabase::reverDoctorEdit()
{
    return doctorTabModel->revertAll();
}

bool IDatabase::importDoctorsFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8); // 设置编码为 UTF-8
    QString line = in.readLine(); // 读取表头并忽略

    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    while (!in.atEnd()) {
        line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() != 6) { // 确保数据行有6个字段
            qDebug() << "Incorrect number of fields in line:" << line;
            continue;
        }

        QSqlRecord newRecord = doctorTabModel->record();
        newRecord.setValue("ID", fields[0]); // ID 是第一个字段
        newRecord.setValue("NAME", fields[1]);
        newRecord.setValue("SEX", fields[2]);
        newRecord.setValue("AGE", fields[3].toInt());
        newRecord.setValue("DEPARTMENT_ID", fields[4]);
        newRecord.setValue("EMPLOYEENO", fields[5]);

        doctorTabModel->insertRow(doctorTabModel->rowCount());
        doctorTabModel->setRecord(doctorTabModel->rowCount() - 1, newRecord);
    }

    file.close();

    // 提交所有更改到数据库
    if (!doctorTabModel->submitAll()) {
        qDebug() << "Failed to submit changes to the database:" << doctorTabModel->lastError();
        return false;
    }

    return true;
}

void IDatabase::exportDoctorsToCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << filePath;
        return;
    }

    QTextStream out(&file);
    // 写入表头
    QStringList headers;
    for (int i = 0; i < doctorTabModel->columnCount(); ++i) {
        headers << doctorTabModel->headerData(i, Qt::Horizontal).toString();
    }
    out << headers.join(",") << "\n";

    // 遍历数据库中的医生数据并写入文件
    for (int i = 0; i < doctorTabModel->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < doctorTabModel->columnCount(); ++j) {
            QString fieldValue = doctorTabModel->data(doctorTabModel->index(i, j)).toString();
            // 检查字段值是否包含逗号或换行符，如果包含，则添加双引号
            if (fieldValue.contains(',') || fieldValue.contains('\n')) {
                row << "\"" << fieldValue << "\"";
            } else {
                row << fieldValue;
            }
        }
        out << row.join(",") << "\n";
    }

    file.close();
    qDebug() << "Patients exported successfully to" << filePath;
}
