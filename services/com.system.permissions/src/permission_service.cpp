#include "permission_service.h"
#include <iostream>
#include <sdbus-c++/sdbus-c++.h>

PermissionService::PermissionService(std::string& databasePath) {
    if (sqlite3_open(databasePath.c_str(), &database) != SQLITE_OK) {
        throw std::runtime_error("Unable to open database");
    }
    initDatabase();
}

PermissionService::~PermissionService() {
    sqlite3_close(database);
}

void PermissionService::initDatabase() {
    std::string sql = "CREATE TABLE IF NOT EXISTS permissions ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "application TEXT NOT NULL,"
                      "permission_code INTEGER NOT NULL);";
    char* errMsg = nullptr;
    if (sqlite3_exec(database, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        throw std::runtime_error(errMsg);
    }
}

void PermissionService::RequesPermission(int permissionEnumCode) {
    std::string clientExecPath = getClientExecPath(sdbus::getUniqueName());

    sqlite3_stmt* stmt;

    const char* sql = "INSERT INTO permissions (application, permission_code) VALUES (?, ?);";
    
    sqlite3_bind_text(stmt, 1, clientExecPath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, permissionEnumCode);
    sqlite3_finalize(stmt);

    std::cout << "Permission requested by application" << clientExecPath << std::endl;
}

bool PermissionService::CheckApplicationHasPermission(const std::string& applicationExecPath, int permissionEnumCode) {
    const char* sql = "SELECT 1 FROM permissions WHERE application = ? AND permission_code = ?;";
    sqlite3_stmt* stmt;
    
    sqlite3_bind_text(stmt, 1, applicationExecPath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, permissionEnumCode);

    bool hasPermission = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return hasPermission;
}


std::string PermissionService::getClientExecPath(const std::string& dbusName) {
    auto connection = sdbus::createSystemBusConnection();
    auto dbusProxy = sdbus::createProxy(*connection, "org.freedesktop.DBus", "/org/freedesktop/DBus");
    
    uint32_t pid;
    dbusProxy->callMethod("GetConnectionUnixProcessID")
        .onInterface("org.freedesktop.DBus")
        .withArguments(dbusName)
        .storeResultsTo(pid);

    std::array<char, 128> buffer;
    std::string result;
    std::string command = "ps -fp " + std::to_string(pid) + " --no-headers -o cmd";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Ошибка при выполнении команды ps");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (result.empty()) {
        throw std::runtime_error("Не удалось получить путь до исполняемого файла для PID: " + std::to_string(pid));
    }

    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    return result;
}
