#ifndef PATIENTVIEW_H
#define PATIENTVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include "idatabase.h"
#include "pageproxymodel.h"
#include "SortProxyModel.h"

namespace Ui {
class PatientView;
}

class PatientView : public QWidget
{
    Q_OBJECT

public:
    explicit PatientView(QWidget *parent = nullptr);
    ~PatientView();

private slots:
    void on_btAdd_clicked();

    void on_btSearch_clicked();

    void on_btDelete_clicked();

    void on_btEdit_clicked();

    void on_headerSectionClicked(int logicalIndex);

    void on_btSort_clicked();

    void on_lastPageBt_clicked();

    void on_nextPageBt_clicked();

    void adjustPaginationButtons();

    void on_btExport_clicked();

    void on_btImport_clicked();

    void updateModel();  // 更新模型

signals:
    void goPatientEditView(int idx);

private:
    Ui::PatientView *ui;
    PageProxyModel *proxyModel; // 使用 PageProxyModel 类型
    SortProxyModel *sortProxyModel;
    int selectedColumn = -1; // 记录用户选择的列，-1 表示未选择
    int lastSortedColumn = -1; // 记录用户上一次选择的列，-1 表示未选择
    int currentPage = 1; // 当前页码
    int recordsPerPage = 10; // 每页记录数
    int totalRecords = 0; // 总记录数
    int totalPage = 1; // 总页数
};

#endif // PATIENTVIEW_H
