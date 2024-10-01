#include <sdbus-c++/sdbus-c++.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string formatTime(uint32_t timeInSeconds)
{
    std::time_t currTime = static_cast<std::time_t>(timeInSeconds);
    std::tm* localTime = std::localtime(&currTime);

    std::ostringstream oss;
    oss << std::put_time(localTime,
                         "%Y-%m-%d %H:%M:%S");  // Формат:
                                                // ГГГГ-ММ-ДД
                                                // ЧЧ:ММ:СС
    return oss.str();
}

int main()
{
    // Создание соединения
    auto connection = sdbus::createSessionBusConnection();

    // Создание прокси для TimeService
    sdbus::ServiceName serviceName{"com.system.time"};
    sdbus::ObjectPath objectPath{"/com/system/time"};
    auto timeProxy = sdbus::createProxy(*connection, serviceName, objectPath);

    uint32_t systemTimeInSeconds;

    // Получение системного времени и форматирование его
    try
    {
        timeProxy->callMethod("GetSystemTime")
            .onInterface("com.system.time")
            .storeResultsTo(systemTimeInSeconds);

        std::string formattedTime = formatTime(systemTimeInSeconds);

        std::cout << "System Time: " << formattedTime << std::endl;
    }
    catch (const sdbus::Error& e)
    {
        std::cerr << "DBus ошибка: " << e.what() << std::endl;
        std::cout << "Повторный запрос..." << std::endl;

        timeProxy->callMethod("GetSystemTime")
            .onInterface("com.system.time")
            .storeResultsTo(systemTimeInSeconds);

        std::string formattedTime = formatTime(systemTimeInSeconds);

        std::cout << "System Time: " << formattedTime << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
