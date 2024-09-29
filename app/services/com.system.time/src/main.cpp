#include "TimeService.h"
#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

int main() {
    TimeService timeService;

    auto connection = sdbus::createSessionBusConnection();
    auto object = sdbus::createObject(*connection, "/com/system/time");

    object->registerMethod("GetSystemTime")
        .onInterface("com.system.time")
        .implementedAs([&timeService]() -> uint64_t {
            try {
                return timeService.GetSystemTime();
            } catch (const sdbus::Error& ex) {
                throw;  
            } catch (const std::exception& ex) {
                throw sdbus::Error("com.system.time.Error", ex.what());
            }
        });

    object->finishRegistration();
    connection->enterEventLoop();

    return 0;
}
