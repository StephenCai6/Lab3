#include "doctorview.h"
#include "ui_doctorview.h"
#include "idatabase.h"
#include <QFileDialog>

DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
    , selectedColumn(-1)
    , lastSortedColumn(-1)
{
    ui->setupUi(this);

    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initDoctorModel()){
        ui->tableView->setModel(iDatabase.doctorTabModel);
        ui->tableView->setSelectionModel(iDatabase.theDoctorSelection);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 表格不允许编辑
        ui->tableView->verticalHeader()->setVisible(false); // 隐藏QTableView的默认行号显示

        // 获取表头
        QHeaderView *header = ui->tableView->horizontalHeader();
        header->setSectionsClickable(true); // 使表头可点击

        // 连接表头点击信号到槽函数
        connect(header, &QHeaderView::sectionClicked, this, &DoctorView::on_headerSectionClicked);

        // 设置每一格的宽度
        for (int i = 0; i < ui->tableView->model()->columnCount(); ++i) {
            if (i == 4 || i == 5) { // 假设第二列和第四列的索引为 4 和 5
                ui->tableView->setColumnWidth(i, 150); // 设置第二列和第四列的宽度为 150 像素
            } else {
                ui->tableView->setColumnWidth(i, 80); // 其他列的宽度设置为 80 像素
            }
        }
    }
}

DoctorView::~DoctorView(){
    delete ui;
}

void DoctorView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchDoctor(filter);
}


void DoctorView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewDoctor();
    emit goDoctorEditView(currow);
}


void DoctorView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentDoctor();
}


void DoctorView::on_btEdit_clicked()
{
    qDebug() << "编辑医生信息";
    QModelIndex curIndex = IDatabase::getInstance().theDoctorSelection->currentIndex();
    qDebug() << curIndex;
    if (curIndex.isValid()) {
        emit goDoctorEditView(curIndex.row());
    }
}

// 选中列表头
void DoctorView::on_headerSectionClicked(int logicalIndex)
{
    // 记录用户选择的列
    selectedColumn = logicalIndex;
     qDebug() << selectedColumn;
}

// 排序
void DoctorView::on_btSort_clicked() {
    if (selectedColumn != -1) {
        // 直接在QSqlTableModel上调用sort方法进行升序排序
        IDatabase::getInstance().doctorTabModel->sort(selectedColumn, Qt::AscendingOrder);
        selectedColumn = -1;
        qDebug() << "已重新排序";
    } else {
        qDebug() << "未选择列，不进行排序";
    }
}

// 导入
void DoctorView::on_btImport_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Import Patients", "", "CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        if (IDatabase::getInstance().importDoctorsFromCSV(filePath)) {
            qDebug() << "Import successful";
        } else {
            qDebug() << "Import failed";
        }
    }
}

// 导出
void DoctorView::on_btExport_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Export Patients", "", "CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        IDatabase::getInstance().exportDoctorsToCSV(filePath);
    }
}
