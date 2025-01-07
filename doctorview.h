#ifndef DOCTORVIEW_H
#define DOCTORVIEW_H

#include <QWidget>
#include <QSortFilterProxyModel>

namespace Ui {
class DoctorView;
}

class DoctorView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorView(QWidget *parent = nullptr);
    ~DoctorView();

private:
    Ui::DoctorView *ui;
    QSortFilterProxyModel *proxyModel;
    int selectedColumn = -1; // 记录用户选择的列，-1 表示未选择
    int lastSortedColumn = -1; // 记录用户上一次选择的列，-1 表示未选择

signals:
    void goDoctorEditView(int index);

private slots:
    void on_btSearch_clicked();
    void on_btAdd_clicked();
    void on_btDelete_clicked();
    void on_btEdit_clicked();
    void on_headerSectionClicked(int logicalIndex);
    void on_btSort_clicked();
    void on_btExport_clicked();
    void on_btImport_clicked();
};

#endif // DOCTORVIEW_H
