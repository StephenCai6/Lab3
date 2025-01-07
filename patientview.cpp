#include "patientview.h"
#include "ui_patientview.h"
#include "idatabase.h"
#include "pageproxymodel.h"
#include "SortProxyModel.h"
#include <QFileDialog>

PatientView::PatientView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PatientView)
    , recordsPerPage(10) // 设置每页显示10条记录
    , currentPage(1)
    , selectedColumn(-1)
    , lastSortedColumn(-1)
{
    ui->setupUi(this);

    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initPatientModel()) {
        // 创建排序代理模型
        sortProxyModel = new SortProxyModel(this);
        sortProxyModel->setSourceModel(iDatabase.patientTabModel);
        sortProxyModel->setDynamicSortFilter(true); // 启用动态排序
        sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive); // 设置排序不区分大小写

        // 创建分页代理模型
        // proxyModel = new QSortFilterProxyModel(this);
        proxyModel = new PageProxyModel(this);
        proxyModel->setSourceModel(sortProxyModel);
        proxyModel->setDynamicSortFilter(true);
        // proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setPageSize(recordsPerPage); // 设置每页大小
        proxyModel->setCurrentPage(currentPage); // 设置当前页码

        // ui->tableView->setModel(iDatabase.patientTabModel);
        ui->tableView->setModel(proxyModel); // 设置代理模型为tableView的模型
        ui->tableView->setSelectionModel(iDatabase.thePatientSelection);
        // ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 选择整行
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 表格不允许编辑
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 行宽
        ui->tableView->verticalHeader()->setVisible(false); // 隐藏QTableView的默认行号显示

        totalRecords = iDatabase.patientTabModel->rowCount();
        totalPage = (totalRecords + recordsPerPage - 1) / recordsPerPage; // 计算总页数
        ui->currentPage->setText(QString::number(currentPage));
        ui->totalPage->setText(QString::number(totalPage));
        // proxyModel->sort(0, Qt::AscendingOrder);// 默认按照ID排序

        // 获取表头
        QHeaderView *header = ui->tableView->horizontalHeader();
        header->setSectionsClickable(true); // 使表头可点击

        // 连接表头点击信号到槽函数
        connect(header, &QHeaderView::sectionClicked, this, &PatientView::on_headerSectionClicked);

        adjustPaginationButtons(); // 初始化上下切页按钮状态
    }
}

PatientView::~PatientView()
{
    delete ui;
    delete proxyModel; // 删除代理模型
}

// 增
void PatientView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewPatient();
    emit goPatientEditView(currow);
}

// 查
void PatientView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchPatient(filter);
}

// 删
void PatientView::on_btDelete_clicked() {
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (currentIndex.isValid()) {
        QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
        IDatabase::getInstance().deleteCurrentPatient(sourceIndex.row());
    }
}

// 改
void PatientView::on_btEdit_clicked()
{
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (currentIndex.isValid()) {
        QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
        emit goPatientEditView(sourceIndex.row());
    }
}

// 选中列表头
void PatientView::on_headerSectionClicked(int logicalIndex)
{
    // 记录用户选择的列
    selectedColumn = logicalIndex;
}

// 排序
void PatientView::on_btSort_clicked() {
    if (selectedColumn != -1) {
        if (sortProxyModel->sortColumn() == lastSortedColumn) {
            Qt::SortOrder order = sortProxyModel->sortOrder() == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
            sortProxyModel->sort(selectedColumn, order);
        } else {
            sortProxyModel->sort(selectedColumn, Qt::AscendingOrder);
        }
        lastSortedColumn = selectedColumn;
        selectedColumn = -1;
        qDebug() << "已重新排序";
        proxyModel->invalidate(); // 重新应用过滤器以显示当前页的数据
    } else {
        qDebug() << "未选择列，不进行排序";
    }
}

// 上一页
void PatientView::on_lastPageBt_clicked() {
    qDebug() << "Current page:" << currentPage << "Total pages:" << totalPage;
    // ui->currentPage->setText(QString::number(currentPage));
    if (currentPage > 1) {
        currentPage--;
        proxyModel->setCurrentPage(currentPage);
        proxyModel->invalidate();
        ui->currentPage->setText(QString::number(currentPage));
    }
}

// 下一页
void PatientView::on_nextPageBt_clicked() {
    qDebug() << "Current page:" << currentPage << "Total pages:" << totalPage;
    // ui->currentPage->setText(QString::number(currentPage));
    if (currentPage < totalPage) {
        currentPage++;
        proxyModel->setCurrentPage(currentPage);
        proxyModel->invalidate();
        ui->currentPage->setText(QString::number(currentPage));
    }
}

// 页数范围
void PatientView::adjustPaginationButtons() {
    ui->lastPageBt->setEnabled(currentPage > 0);
    ui->nextPageBt->setEnabled(currentPage < totalPage);
}

// 导入
void PatientView::on_btImport_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Import Patients", "", "CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        if (IDatabase::getInstance().importPatientsFromCSV(filePath)) {
            qDebug() << "Import successful";
            updateModel(); // 更新模型和分页信息
        } else {
            qDebug() << "Import failed";
        }
    }
}

// 导出
void PatientView::on_btExport_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Export Patients", "", "CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        IDatabase::getInstance().exportPatientsToCSV(filePath);
    }
}

// 更新页码
void PatientView::updateModel() {
    // 刷新模型
    IDatabase::getInstance().patientTabModel->select();

    // 更新总记录数
    totalRecords = IDatabase::getInstance().patientTabModel->rowCount();

    // 更新分页信息
    totalPage = (totalRecords + recordsPerPage - 1) / recordsPerPage;
    currentPage = qMin(currentPage, totalPage);  // 确保当前页码不超过总页数
    // ui->currentPage->setText(QString::number(currentPage));
    ui->currentPage->setText(QString::number(1)); // 重置为第一页
    ui->totalPage->setText(QString::number(totalPage));

    // 刷新视图
    ui->tableView->reset();
}
