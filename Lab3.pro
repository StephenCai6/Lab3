QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    departmentview.cpp \
    doctoreditview.cpp \
    doctorview.cpp \
    idatabase.cpp \
    loginview.cpp \
    main.cpp \
    masterview.cpp \
    medicineeditview.cpp \
    medicineview.cpp \
    pageproxymodel.cpp \
    patienteditview.cpp \
    patientview.cpp \
    recordeditview.cpp \
    recordview.cpp \
    registerview.cpp \
    sortproxymodel.cpp \
    welcomeview.cpp

HEADERS += \
    RowNumberProxyModel.h \
    departmentview.h \
    doctoreditview.h \
    doctorview.h \
    idatabase.h \
    loginview.h \
    masterview.h \
    medicineeditview.h \
    medicineview.h \
    pageproxymodel.h \
    patienteditview.h \
    patientview.h \
    recordeditview.h \
    recordview.h \
    registerview.h \
    sortproxymodel.h \
    welcomeview.h

FORMS += \
    departmentview.ui \
    doctoreditview.ui \
    doctorview.ui \
    loginview.ui \
    masterview.ui \
    medicineeditview.ui \
    medicineview.ui \
    patienteditview.ui \
    patientview.ui \
    recordeditview.ui \
    recordview.ui \
    registerview.ui \
    welcomeview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    lab3.qrc

DISTFILES +=
