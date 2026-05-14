#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QMainWindow>

#include "backendservice.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTableWidget;
class QPushButton;
QT_END_NAMESPACE

class AdminDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminDashboard(QWidget *parent = nullptr);

private:
    void reloadData();
    void showError(const QString& message);
    void openAddProductDialog();
    void openSellProductDialog();
    void openUpdateStockDialog();
    void openAddUserDialog();
    void openRemoveUserDialog();

    BackendService backend_;

    QLabel* totalProductsValue_ = nullptr;
    QLabel* lowStockValue_ = nullptr;
    QLabel* salesValue_ = nullptr;
    QLabel* usersValue_ = nullptr;
    QLabel* revenueValue_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    QTableWidget* productsTable_ = nullptr;
    QTableWidget* salesTable_ = nullptr;
    QTableWidget* usersTable_ = nullptr;
};

#endif
