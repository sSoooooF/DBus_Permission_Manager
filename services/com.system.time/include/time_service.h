#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <string>
#include <sdbus-c++/sdbus-c++.h>

class TimeService {
public:
    uint64_t GetSystemTime();

private:
    std::string getClientExecPath(const std::string& dbusName);
    bool checkPermission(const std::string& execPath);
};

#endif // TIME_SERVICE_H