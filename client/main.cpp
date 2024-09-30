#include <iostream>
#include <sdbus-c++/sdbus-c++.h>
#include <chrono>
#include <iomanip>

void printTime(uimt64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm* timeInfo = std::localtime(&time);
    std::cout << "Текущее время: " << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S") << std::endl;

}

uint64_t getSystemTime() {
    auto connection = sdbus::createSessionBusConnection();
    auto timeProxy = sdbus::createProxy(*connection, "com.system.time", "/com/system/time");

    uint64_t systemTime;
    try {
        timeProxy->callMethod("GetSystemTime")
                 .onInterface("com.system.time")
                 .storeResultsTo(systemTime);
    } catch (const sdbus::Error& error) {
        throw std::runtime_error("Ошибка сервиса com.system.time: " + std::string(error.what()));
    }
    return systemTime;
}

int main() {
    try {
        uint64_t systemTime = getSystemTime();
        printTime(systemTime);
    } catch (const std::runtime_error& error) {
        std::cerr << "Произошла ошибка: " << error.what() << std::endl;
    }

    return 0;
}