#include <sdbus-c++/sdbus-c++.h>

#include <iostream>

#include "../com.system.permissions/include/permission-proxy-glue.h"
#include "include/time-server-glue.h"


/*
 * Клиентское приложение должно вызывать этот прокси для получения текущего системного времени.
 * Если клиент не имеет разрешения, оно должно вызвать метод RequestPermission, передавая ему путь к клиентскому приложению.
 * Приложение TimeService должно проверять разрешения клиента перед получением текущего системного времени.
 * Если разрешения отсутствуют, приложение должно выбросить исключение с сообщением об ошибке.
*/
class TimeService : public sdbus::AdaptorInterfaces<com::system::time_adaptor>
{
   public:
    TimeService(sdbus::IConnection& connection, sdbus::ObjectPath objectPath)
        : AdaptorInterfaces(connection, std::move(objectPath)),
          connection_(connection),
          permissionProxy_(sdbus::createProxy(
              connection, sdbus::ServiceName("com.system.permissions"),
              sdbus::ObjectPath("/com/system/permissions")))
    {
        registerAdaptor();
    }

    ~TimeService() { unregisterAdaptor(); }

   protected:
   // Метод для получения системного времени
    uint32_t GetSystemTime() override
    {
        std::string clientExecPath = getClientExecPath();
        if (!checkClientPermission(clientExecPath))
        {
            // Вызов метода сервиса permissions
            permissionProxy_->callMethod("RequestPermission") 
                .onInterface("com.system.permissions")
                .withArguments(0, clientExecPath);
            throw std::runtime_error(
                "UnauthorizedAccess: нет "
                "разрешения");
        }
        return getCurrentTime();
    }

   private:
    std::unique_ptr<sdbus::IProxy> permissionProxy_;
    sdbus::IConnection& connection_;

    // Метод для получения пути до исполняемого файла клиента
    std::string getClientExecPath()
    {
        std::string clientBusName =
            getObject().getCurrentlyProcessedMessage().getSender();

        sdbus::ServiceName dest{"org.freedesktop.DBus"};
        sdbus::ObjectPath objectPath{"/org/freedesktop/DBus"};
        auto dbusProxy = sdbus::createProxy(dest, objectPath);
        uint32_t clientPID;
        dbusProxy->callMethod("GetConnectionUnixProcessID")
            .onInterface("org.freedesktop.DBus")
            .withArguments(clientBusName)
            .storeResultsTo(clientPID);

        std::string cmd =
            "ps -p " + std::to_string(clientPID) + " -o cmd --no-headers";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe)
            throw std::runtime_error(
                "Ошибка получения пути к "
                "исполняемому файлу");

        char buffer[128];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);

        result.erase(std::remove(result.begin(), result.end(), '\n'),
                     result.end());
        return result;
    }

    // Метод для проверки разрешений клиента
    bool checkClientPermission(const std::string& clientName)
    {
        try
        {
            bool hasPermission;
            permissionProxy_
                ->callMethod(
                    "CheckApplicationHasPermissio"
                    "n")
                .onInterface("com.system.permissions")
                .withArguments(0, clientName)
                .storeResultsTo(hasPermission);
            return hasPermission;
        }
        catch (const sdbus::Error& e)
        {
            std::cerr << "Ошибка при проверке "
                         "разрешений: "
                      << e.what() << std::endl;
            return false;
        }
    }

    uint32_t getCurrentTime() { return static_cast<uint64_t>(time(nullptr)); }
};

int main()
{
    sdbus::ServiceName serviceName{"com.system.time"};
    auto connection = sdbus::createSessionBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/com/system/time"};

    TimeService timeService(*connection, objectPath);

    connection->enterEventLoop();

    return 0;
}
