#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>

#include <iostream>
#include <string>

#include "include/permission-server-glue.h"

class PermissionService : public sdbus::AdaptorInterfaces<com::system::permissions_adaptor>
{
   public:
    PermissionService(sdbus::IConnection& connection, sdbus::ObjectPath objectPath)
        : AdaptorInterfaces(connection, std::move(objectPath))
    {
        registerAdaptor();
        openDatabase();
        createTable();
    }

    ~PermissionService()
    {
        sqlite3_close(db);
        unregisterAdaptor();
    }

   protected:
    void RequestPermission(const int32_t& permissionEnumCode,
                           const std::string& applicationExecPath) override
    {
        savePermission(applicationExecPath, permissionEnumCode);
    }

    bool CheckApplicationHasPermission(const int32_t& permissionEnumCode,
                                       const std::string& applicationExecPath) override
    {
        return checkPermissionInDatabase(applicationExecPath, permissionEnumCode);
    }

   private:
    sqlite3* db;

    void openDatabase()
    {
        if (sqlite3_open("permissions.db", &db))
        {
            std::cerr << "Ошибка открытия базы данных." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void createTable()
    {
        const char* sql =
            "CREATE TABLE IF NOT EXISTS Permissions ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "ExecPath TEXT NOT NULL, "
            "PermissionCode INTEGER NOT NULL);";

        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::cerr << "Ошибка создания таблицы: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            exit(EXIT_FAILURE);
        }
    }

    void savePermission(const std::string& execPath, int32_t permissionCode)
    {
        std::string sql = "INSERT INTO Permissions (ExecPath, PermissionCode) VALUES (?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            std::cerr << "Ошибка подготовки SQL запроса." << std::endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, execPath.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, permissionCode);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            std::cerr << "Ошибка выполнения SQL запроса." << std::endl;
            sqlite3_finalize(stmt);
            return;
        }

        sqlite3_finalize(stmt);
    }

    bool checkPermissionInDatabase(const std::string& execPath, int32_t permissionCode)
    {
        std::string sql =
            "SELECT COUNT(*) FROM Permissions WHERE ExecPath = ? AND PermissionCode = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            std::cerr << "Ошибка подготовки SQL запроса." << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, execPath.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, permissionCode);

        bool hasPermission = false;
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            hasPermission = sqlite3_column_int(stmt, 0) > 0;
        }

        sqlite3_finalize(stmt);
        return hasPermission;
    }
};

int main()
{
    sdbus::ServiceName serviceName{"com.system.permissions"};
    auto connection = sdbus::createSessionBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/com/system/permissions"};

    PermissionService permissionService(*connection, std::move(objectPath));

    connection->enterEventLoop();
    return 0;
}