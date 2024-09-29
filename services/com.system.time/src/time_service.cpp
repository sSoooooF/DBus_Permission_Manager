#include "time_service.h"
#include <chrono>
#include <iostream>

uint64_t TimeService::GetSystemTime() {
    std::string clientExecPath = getClientExecPath(sdbus::getUniqueName());

    if (!checkPermission(clientExecPath)) {
        throw sdbus::Error("com.system.permissions.UnauthorizedAccess", "Доступ запрещён: отсутствует разрешение SystemTime");
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return static_cast<uint64_t>(timestamp);
}

std::string TimeService::getClientExecPath(const std::string& dbusName) {
    std::array<char, 128> buffer;
    std::string result;
    std::string command = "ps -fp $(dbus-send --session --print-reply --dest=org.freedesktop.DBus / org.freedesktop.DBus.GetConnectionUnixProcessID string:" + dbusName + " | awk '/uint32/ {print $2}') --no-headers -o cmd";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Ошибка при выполнении команды ps");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (result.empty()) {
        throw std::runtime_error("Не удалось получить путь до исполняемого файла для DBus клиента");
    }

    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    return result;
}

bool TimeService::checkPermission(const std::string& execPath) {
    auto connection = sdbus::createSessionBusConnection();
    auto permissionsProxy = sdbus::createProxy(*connection, "com.system.permissions", "/com/system/permissions");

    bool hasPermission;
    permissionsProxy->callMethod("CheckApplicationHasPermission")
                    .onInterface("com.system.permissions")
                    .withArguments(execPath, 0)
                    .storeResultsTo(hasPermission);

    return hasPermission;
}