#ifndef BACKENDSERVICE_H
#define BACKENDSERVICE_H

#include <QSqlDatabase>
#include <QString>
#include <QVector>

struct DashboardMetrics
{
    int totalProducts = 0;
    int lowStockProducts = 0;
    int totalSales = 0;
    int totalUsers = 0;
    double revenue = 0.0;
};

struct ProductRecord
{
    int id = 0;
    QString name;
    double price = 0.0;
    int stock = 0;
};

struct SaleRecord
{
    int invoiceNo = 0;
    QString productName;
    int quantity = 0;
    double total = 0.0;
    QString createdAt;
};

struct UserRecord
{
    int id = 0;
    QString username;
    QString role;
};

class BackendService
{
public:
    BackendService();

    bool loadMetrics(DashboardMetrics* metrics, QString* errorMessage = nullptr);
    QVector<ProductRecord> loadProducts(QString* errorMessage = nullptr);
    QVector<ProductRecord> loadLowStockProducts(QString* errorMessage = nullptr);
    QVector<SaleRecord> loadRecentSales(int limit, QString* errorMessage = nullptr);
    QVector<UserRecord> loadUsers(QString* errorMessage = nullptr);

    bool addProduct(
        int id,
        const QString& name,
        double price,
        int stock,
        QString* errorMessage = nullptr
    );
    bool updateStock(int id, int newStock, QString* errorMessage = nullptr);
    bool deleteProduct(int id, QString* errorMessage = nullptr);
    bool sellProduct(int id, int quantity, QString* errorMessage = nullptr);
    bool addUser(
        const QString& username,
        const QString& password,
        const QString& role,
        QString* errorMessage = nullptr
    );
    bool removeUser(const QString& username, QString* errorMessage = nullptr);

private:
    QString connectionName_;
    QSqlDatabase db_;

    bool ensureOpen(QString* errorMessage = nullptr);
    static QString envOrDefault(const char* name, const QString& fallback);
    static int envPortOrDefault(const char* name, int fallback);
};

#endif // BACKENDSERVICE_H
