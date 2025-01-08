#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "registerview.h"
#include "loginview.h"
#include "welcomeview.h"
#include "doctorview.h"
#include "doctoreditview.h"
#include "departmentview.h"
#include "patientview.h"
#include "patienteditview.h"
#include "recordview.h"
#include "recordeditview.h"
#include "medicineview.h"
#include "medicineeditview.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();

public slots:
    void goRegisterView();
    void goLoginView();
    void goWelcomView();
    void goPreviousView();

    void goDoctorView();
    void goDoctorEditView(int index);
    void goDepartmentView();
    void goPatientView();
    void goPatientEditView(int rowNo);
    void goRecordView();
    void goRecordEditView(int rowNo);
    void goMedicineView();
    void goMedicineEditView(int rowNo);

private slots:
    void on_btBack_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_btLogout_clicked();

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;

    WelcomeView *welcomeView;
    RegisterView* registerView;
    LoginView *loginView;

    DoctorView *doctorView;
    DoctorEditView *doctorEditView;

    PatientView *patientView;
    PatientEditView *patientEditView;

    DepartmentView *departmentView;

    RecordView *recordView;
    RecordEditView *recordEditView;

    MedicineView *medicineView;
    MedicineEditView *medicineEditView;
};
#endif // MASTERVIEW_H
