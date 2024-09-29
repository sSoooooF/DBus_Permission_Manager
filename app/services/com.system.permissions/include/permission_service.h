#ifdef PERMISSION_SERVICE_H
#define PERMISSION_SERVICE_H

#include <iostream>
#include <string>
#include <sdbus-c++/sdbus-c++.h>
#include "sqlite3.h"

enum Permissions {
    SystemTime = 0
};

class PermissionService {
public:
    PermissionService(std::string& databasePath);
    ~PermissionService();

    void RequestPermission(Permissions permissionEnumCode);
    bool CheckApplicationHasPermission(const std::string& applicationExecPath, Permissions permissionEnumCode);

private:
    std::string getClientExecPath(const std::string& dbusName);
    void initDatabase();

    sqlite3* database;
}
