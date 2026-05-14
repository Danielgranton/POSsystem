#ifndef AUTHDATABASE_H
#define AUTHDATABASE_H

#include <QString>
#include <QSqlDatabase>

class AuthDatabase
{
public:
    AuthDatabase();

    QString login(
        const QString& username,
        const QString& password,
        QString* errorMessage = nullptr
    );

private:
    QString connectionName_;
    QSqlDatabase db_;

    static QString envOrDefault(const char* name, const char* fallback);
    static int envPortOrDefault(const char* name, int fallback);
};

#endif // AUTHDATABASE_H
