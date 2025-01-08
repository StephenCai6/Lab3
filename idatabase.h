#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QFile>
#include <QTextStream>

class IDatabase : public QObject
{
    Q_OBJECT

public:
    static IDatabase &getInstance()
    {
        static IDatabase instance;
        return instance;
    }

    QString userRegister(int ID, QString fullname, QString username, QString password);
    QString userLogin(QString userName, QString password);

private:
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const&) = delete;
    void operator = (IDatabase const&) = delete;

    QSqlDatabase database;

    void ininDatabase();

signals:

public:
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    void deleteCurrentPatient(int rowIndex);
    bool submitPatientEdit();
    void revertPatientEdit();
    void exportPatientsToCSV(const QString &filePath); // 导出
    bool importPatientsFromCSV(const QString &filePath); // 导入

    QSqlTableModel *patientTabModel; // 数据模型
    QItemSelectionModel *thePatientSelection; // 选择模型

    bool initDoctorModel();
    int addNewDoctor();
    bool searchDoctor(QString filter);
    void deleteCurrentDoctor();
    bool submitDoctorEdit();  //提交医生编辑
    void revertDoctorEdit();    //撤销当前的医生编辑操作
    void exportDoctorsToCSV(const QString &filePath); // 导出
    bool importDoctorsFromCSV(const QString &filePath); // 导入

    QSqlTableModel *doctorTabModel;  //表示医生数据的模型
    QItemSelectionModel *theDoctorSelection;  //用于在医生表格中选择医生的行

    bool initRecordModel();
    int addNewRecord();
    bool searchRecord(QString filter);
    void deleteCurrentRecord();
    bool submitRecordEdit();  //提交医生编辑
    void revertRecordEdit();    //撤销当前的医生编辑操作
    void exportRecordsToCSV(const QString &filePath); // 导出
    bool importRecordsFromCSV(const QString &filePath); // 导入

    QSqlTableModel *recordTabModel;  //表示医生数据的模型
    QItemSelectionModel *theRecordSelection;  //用于在医生表格中选择医生的行

};

#endif // IDATABASE_H
