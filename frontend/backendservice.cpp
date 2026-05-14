#include "backendservice.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

namespace {
constexpr int kDefaultPort = 5433;

QString trimValue(const QString& value)
{
    const QString trimmed = value.trimmed();
    return trimmed;
}
}

QString BackendService::envOrDefault(const char* name, const QString& fallback)
{
    const QString value = QString::fromUtf8(qgetenv(name)).trimmed();
    return value.isEmpty() ? fallback : value;
}

int BackendService::envPortOrDefault(const char* name, int fallback)
{
    bool ok = false;
    const int value = qEnvironmentVariableIntValue(name, &ok);
    return ok && value > 0 ? value : fallback;
}

BackendService::BackendService()
    : connectionName_("pos_frontend_connection")
{
    if (QSqlDatabase::contains(connectionName_)) {
        db_ = QSqlDatabase::database(connectionName_);
    } else {
        db_ = QSqlDatabase::addDatabase("QPSQL", connectionName_);
    }

    db_.setHostName(envOrDefault("POS_DB_HOST", ""));
    db_.setPort(envPortOrDefault("POS_DB_PORT", kDefaultPort));
    db_.setDatabaseName(envOrDefault("POS_DB_NAME", "pos_system"));
    db_.setUserName(envOrDefault("POS_DB_USER", "pos_user"));
    db_.setPassword(envOrDefault("POS_DB_PASSWORD", "possystem"));
}

bool BackendService::ensureOpen(QString* errorMessage)
{
    if (db_.isOpen() || db_.open()) {
        return true;
    }

    if (errorMessage != nullptr) {
        *errorMessage = db_.lastError().text();
    }
    return false;
}

bool BackendService::loadMetrics(DashboardMetrics* metrics, QString* errorMessage)
{
    if (metrics == nullptr) {
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    QSqlQuery query(db_);

    if (!query.exec("SELECT COUNT(*) FROM products")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }
    if (query.next()) metrics->totalProducts = query.value(0).toInt();

    if (!query.exec("SELECT COUNT(*) FROM products WHERE stock < 5")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }
    if (query.next()) metrics->lowStockProducts = query.value(0).toInt();

    if (!query.exec("SELECT COUNT(*) FROM sales")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }
    if (query.next()) metrics->totalSales = query.value(0).toInt();

    if (!query.exec("SELECT COUNT(*) FROM users")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }
    if (query.next()) metrics->totalUsers = query.value(0).toInt();

    if (!query.exec("SELECT COALESCE(SUM(total), 0) FROM sales")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }
    if (query.next()) metrics->revenue = query.value(0).toDouble();

    return true;
}

QVector<ProductRecord> BackendService::loadProducts(QString* errorMessage)
{
    QVector<ProductRecord> items;
    if (!ensureOpen(errorMessage)) {
        return items;
    }

    QSqlQuery query(db_);
    if (!query.exec("SELECT id, name, price, stock FROM products ORDER BY id ASC")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return items;
    }

    while (query.next()) {
        ProductRecord item;
        item.id = query.value(0).toInt();
        item.name = query.value(1).toString();
        item.price = query.value(2).toDouble();
        item.stock = query.value(3).toInt();
        items.push_back(item);
    }

    return items;
}

QVector<ProductRecord> BackendService::loadLowStockProducts(QString* errorMessage)
{
    QVector<ProductRecord> items;
    if (!ensureOpen(errorMessage)) {
        return items;
    }

    QSqlQuery query(db_);
    if (!query.exec("SELECT id, name, price, stock FROM products WHERE stock < 5 ORDER BY stock ASC, id ASC")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return items;
    }

    while (query.next()) {
        ProductRecord item;
        item.id = query.value(0).toInt();
        item.name = query.value(1).toString();
        item.price = query.value(2).toDouble();
        item.stock = query.value(3).toInt();
        items.push_back(item);
    }

    return items;
}

QVector<SaleRecord> BackendService::loadRecentSales(int limit, QString* errorMessage)
{
    QVector<SaleRecord> items;
    if (!ensureOpen(errorMessage)) {
        return items;
    }

    const int safeLimit = limit > 0 ? limit : 10;
    QSqlQuery query(db_);
    const QString sql =
        QStringLiteral(
            "SELECT invoice_no, product_name, quantity, total, created_at "
            "FROM sales ORDER BY created_at DESC, invoice_no DESC LIMIT %1"
        ).arg(safeLimit);

    if (!query.exec(sql)) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return items;
    }

    while (query.next()) {
        SaleRecord item;
        item.invoiceNo = query.value(0).toInt();
        item.productName = query.value(1).toString();
        item.quantity = query.value(2).toInt();
        item.total = query.value(3).toDouble();
        item.createdAt = query.value(4).toString();
        items.push_back(item);
    }

    return items;
}

QVector<UserRecord> BackendService::loadUsers(QString* errorMessage)
{
    QVector<UserRecord> items;
    if (!ensureOpen(errorMessage)) {
        return items;
    }

    QSqlQuery query(db_);
    if (!query.exec("SELECT id, username, role FROM users ORDER BY id ASC")) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return items;
    }

    while (query.next()) {
        UserRecord item;
        item.id = query.value(0).toInt();
        item.username = query.value(1).toString();
        item.role = query.value(2).toString();
        items.push_back(item);
    }

    return items;
}

bool BackendService::addProduct(
    int id,
    const QString& name,
    double price,
    int stock,
    QString* errorMessage
)
{
    const QString cleanName = trimValue(name);
    if (id <= 0 || cleanName.isEmpty() || price < 0.0 || stock < 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "Invalid product details.";
        }
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    QSqlQuery query(db_);
    query.prepare(
        "INSERT INTO products(id, name, price, stock) "
        "VALUES(:id, :name, :price, :stock)"
    );
    query.bindValue(":id", id);
    query.bindValue(":name", cleanName);
    query.bindValue(":price", price);
    query.bindValue(":stock", stock);

    if (!query.exec()) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }

    return true;
}

bool BackendService::updateStock(int id, int newStock, QString* errorMessage)
{
    if (id <= 0 || newStock < 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "Invalid stock update.";
        }
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE products SET stock = :stock WHERE id = :id");
    query.bindValue(":stock", newStock);
    query.bindValue(":id", id);

    if (!query.exec()) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "Product not found.";
        }
        return false;
    }

    return true;
}

bool BackendService::deleteProduct(int id, QString* errorMessage)
{
    if (id <= 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "Invalid product id.";
        }
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("DELETE FROM products WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "Product not found.";
        }
        return false;
    }

    return true;
}

bool BackendService::sellProduct(int id, int quantity, QString* errorMessage)
{
    if (id <= 0 || quantity <= 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "Invalid sale details.";
        }
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    if (!db_.transaction()) {
        if (errorMessage != nullptr) {
            *errorMessage = db_.lastError().text();
        }
        return false;
    }

    QSqlQuery query(db_);
    query.prepare(
        "SELECT name, stock, price FROM products WHERE id = :id"
    );
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        db_.rollback();
        if (errorMessage != nullptr) {
            *errorMessage = query.lastError().text().isEmpty()
                ? "Product not found."
                : query.lastError().text();
        }
        return false;
    }

    const QString name = query.value(0).toString();
    const int stock = query.value(1).toInt();
    const double price = query.value(2).toDouble();

    if (quantity > stock) {
        db_.rollback();
        if (errorMessage != nullptr) {
            *errorMessage = "Not enough stock.";
        }
        return false;
    }

    QSqlQuery updateQuery(db_);
    updateQuery.prepare("UPDATE products SET stock = :stock WHERE id = :id");
    updateQuery.bindValue(":stock", stock - quantity);
    updateQuery.bindValue(":id", id);

    if (!updateQuery.exec()) {
        db_.rollback();
        if (errorMessage != nullptr) *errorMessage = updateQuery.lastError().text();
        return false;
    }

    QSqlQuery saleQuery(db_);
    saleQuery.prepare(
        "INSERT INTO sales(product_id, product_name, quantity, total) "
        "VALUES(:product_id, :product_name, :quantity, :total)"
    );
    saleQuery.bindValue(":product_id", id);
    saleQuery.bindValue(":product_name", name);
    saleQuery.bindValue(":quantity", quantity);
    saleQuery.bindValue(":total", price * quantity);

    if (!saleQuery.exec()) {
        db_.rollback();
        if (errorMessage != nullptr) *errorMessage = saleQuery.lastError().text();
        return false;
    }

    if (!db_.commit()) {
        if (errorMessage != nullptr) {
            *errorMessage = db_.lastError().text();
        }
        return false;
    }

    return true;
}

bool BackendService::addUser(
    const QString& username,
    const QString& password,
    const QString& role,
    QString* errorMessage
)
{
    const QString cleanUsername = trimValue(username);
    const QString cleanPassword = trimValue(password);
    const QString cleanRole = trimValue(role);

    if (cleanUsername.isEmpty() || cleanPassword.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Username and password are required.";
        }
        return false;
    }

    if (cleanRole != "admin" && cleanRole != "cashier") {
        if (errorMessage != nullptr) {
            *errorMessage = "Role must be admin or cashier.";
        }
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    QSqlQuery query(db_);
    query.prepare(
        "INSERT INTO users(username, password, role) "
        "VALUES(:username, :password, :role)"
    );
    query.bindValue(":username", cleanUsername);
    query.bindValue(":password", cleanPassword);
    query.bindValue(":role", cleanRole);

    if (!query.exec()) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }

    return true;
}

bool BackendService::removeUser(const QString& username, QString* errorMessage)
{
    const QString cleanUsername = trimValue(username);
    if (cleanUsername.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Username is required.";
        }
        return false;
    }

    if (!ensureOpen(errorMessage)) {
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("DELETE FROM users WHERE username = :username");
    query.bindValue(":username", cleanUsername);

    if (!query.exec()) {
        if (errorMessage != nullptr) *errorMessage = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        if (errorMessage != nullptr) {
            *errorMessage = "User not found.";
        }
        return false;
    }

    return true;
}
