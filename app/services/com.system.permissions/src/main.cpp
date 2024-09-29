#include "permission_service.h"
#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

int main() {
    std::string databasePath = "permissions.db";
    PermissionService service(databasePath);

    auto connection = sdbus::createSessionBusConnection();
    auto object = sdbus::createObject(*connection, "/com/system/permissions");

    object->registerMethod("RequestPermissions")
    .onInterface("com.system.permissions")
    .implementedAs([&service](int permissionCode) {
        try {
            service.RequesPermission(permissionCode);
        } catch (const std::exception& ex) {
            throw sdbus::Error("com.system.permissions.Error", ex.what())
        }
    });

    object->registerMethod("CheckApplicationHasPermission")
    .onInterface("com.system.permissions")
    .implementedAs([&service](const std::string& applicationExecPath, int permissionCode) -> bool {
        try {
            return service.CheckApplicationHasPermission(applicationExecPath, permissionCode);
        } catch (const std::exception& ex) {
            throw sdbus::Error("com.system.permissions.Error", ex.what())
        }
    });

    object->finishRegistration();
    connection->enterEventLoop();

    return 0;
}