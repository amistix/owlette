#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <vector>

#include "storage/AppStorageManager.h"

#include "i2pd/DaemonAndroid.h"

namespace i2p
{
    class I2pdManager 
    {
    public:
        void start();
        void restart();
        void shutdown();


    private:

        void initialize();
        android::DaemonAndroid* daemon = nullptr;

        bool isRunning = false;
        std::thread* daemonThread = nullptr;
    };
}