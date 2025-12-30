#include "i2pd/I2pdManager.h"
#include "FS.h"
#include "./DestinationClient.cpp"

namespace i2p
{
    void I2pdManager::initialize()
    {
        if (daemon) return;
        daemon = new android::DaemonAndroid();

        std::string dataDir = "";
        do 
        {
            dataDir = storage::getAppStoragePath();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        while (dataDir.empty());

        i2p::fs::DetectDataDir(dataDir, false);
        int numAttempts = 0;
        do
        {
            if (i2p::fs::Exists (i2p::fs::DataDirPath("assets.ready"))) break; // assets ready
            numAttempts++;
            std::this_thread::sleep_for (std::chrono::seconds(1)); // otherwise wait for 1 more second
        }
        while (numAttempts <= 10);

        daemon->setDataDir(dataDir);
        daemon->init(1, nullptr);
    }

    void I2pdManager::start()
    {
        if (daemonThread && daemonThread->joinable()) return;
        if (isRunning) return;
        isRunning = true;

        daemonThread = new std::thread([this]() {
            this->initialize();
            this->daemon->start();
            startDestinationClient();
            // connectToDestination("");
        });
    }

    void I2pdManager::shutdown()
    {
        if (daemon && daemon->isRunning)
        {
            daemon->stop();
        }

        if (daemonThread)
        {
            if (daemonThread->joinable())
            {
                daemonThread->join();
            }
            delete daemonThread;
            daemonThread = nullptr;
        }
    }

    void I2pdManager::restart()
    {
        shutdown();
        start();
    }
}