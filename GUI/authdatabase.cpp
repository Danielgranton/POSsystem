#include "authdatabase.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

namespace {
constexpr int kDefaultPort = 5433;
}

QString AuthDatabase::envOrDefault(const char* name, const char* fallback)
{
    const QString value = QString::fromUtf8(qgetenv(name)).trimmed();
    return value.isEmpty() ? QString::fromUtf8(fallback) : value;
}

int AuthDatabase::envPortOrDefault(const char* name, int fallback)
{
    bool ok = false;
    const int value = qEnvironmentVariableIntValue(name, &ok);
    return ok && value > 0 ? value : fallback;
}

AuthDatabase::AuthDatabase()
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

QString AuthDatabase::login(
    const QString& username,
    const QString& password,
    QString* errorMessage
)
{
    if (!db_.isOpen() && !db_.open()) {
        if (errorMessage != nullptr) {
            *errorMessage = db_.lastError().text();
        }
        return {};
    }

    QSqlQuery query(db_);
    query.prepare(
        "SELECT role "
        "FROM users "
        "WHERE username = :username AND password = :password"
    );
    query.bindValue(":username", username.trimmed());
    query.bindValue(":password", password);

    if (!query.exec()) {
        if (errorMessage != nullptr) {
            *errorMessage = query.lastError().text();
        }
        return {};
    }

    if (!query.next()) {
        return {};
    }

    return query.value(0).toString();
}
