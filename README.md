# D-Bus Permission Manager

## Установка и сборка

### Предварительные требования

1. Убедитесь, что у вас установлены следующие зависимости:
   - CMake
   - D-Bus
   - [sdbus-c++](https://github.com/Kistler-Group/sdbus-cpp)
   - g++
   - git
  
### Шаги по сборке

1. Клонируйте репозиторий:
```bash
git clone https://github.com/sSoooooF/DBus_Permission_Manager
cd DBus_Permission_Manager
```

2. Сборка сервиса com.system.permission:
```bash
cd services/com.system.permissions
mkdir build
cd build
cmake ..
make
```

3. Сборка сервиса com.system.time:
```bash
cd ../../com.system.time
mkdir build
cd build
cmake ..
make
```

4. Сборка клиентского приложения:
```bash
cd ../../client
mkdir build
cd build
cmake ..
make
```

#### Запуск сервисов

1. В первом терминале запустите сервис com.system.permission
```bash
cd services/com.system.permissions/build
./permissions_server
```

2. Во втором терминале запустите сервис времени:

```bash
cd ../com.system.time/build
./time_server
```

3. В третьем терминале запустите клиентское приложение:
```bash
cd ../../client/build
./client
```
