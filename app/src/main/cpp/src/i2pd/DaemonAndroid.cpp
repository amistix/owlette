/*
* Copyright (c) 2013-2022, The PurpleI2P Project
*
* This file is part of Purple i2pd project and licensed under BSD3
*
* See full license text in LICENSE file at top of project tree
*/


#include "i2pd/DaemonAndroid.h"
#include "Daemon.h"
#include "I18N.h"
#include "Config.h"
#include "RouterContext.h"
#include "ClientContext.h"
#include "Transports.h"
#include "Tunnel.h"
#include "FS.h"

namespace i2p
{
namespace android
{
	std::string dataDir = "";

	DaemonAndroid::DaemonAndroid (){}

	DaemonAndroid::~DaemonAndroid (){}

	bool DaemonAndroid::init(int argc, char* argv[])
	{
		return Daemon.init(argc, argv);
	}

	void DaemonAndroid::start()
	{
		Daemon.start();
		isRunning = true;
	}

	void DaemonAndroid::stop()
	{
		Daemon.stop();
	}

	void DaemonAndroid::restart()
	{
		stop();
		start();
	}

	void DaemonAndroid::reloadTunnelsConfigs()
	{
		i2p::client::context.ReloadConfig();
	}

	int DaemonAndroid::getTransitTunnelsCount()
	{
		return i2p::tunnel::tunnels.CountTransitTunnels();
	}

	std::string DaemonAndroid::getWebConsAddr()
	{
		std::string httpAddr; i2p::config::GetOption("http.address", httpAddr);
		uint16_t    httpPort; i2p::config::GetOption("http.port", httpPort);
		std::string result = "http://" + httpAddr + ":" + std::to_string(httpPort) + "/";
		return result;
	}

	void DaemonAndroid::setLanguage(std::string language)
	{
		i2p::i18n::SetLanguage(language);
	}

	void DaemonAndroid::setAcceptsTunnels(bool state)
	{
		i2p::context.SetAcceptsTunnels (state);
	}

	void DaemonAndroid::setDataDir(std::string path)
	{
		Daemon.setDataDir(path);
	}



	// static DaemonAndroid daemon;
	// static char* argv[1]={strdup("tmp")};
	// /**
	//  * returns error details if failed
	//  * returns "ok" if daemon initialized and started okay
	//  */
	// std::string start(/*int argc, char* argv[]*/)
	// {
	// 	try
	// 	{
	// 		{
	// 			// make sure assets are ready before proceed
	// 			i2p::fs::DetectDataDir(dataDir, false);
	// 			int numAttempts = 0;
	// 			do
	// 			{
	// 				if (i2p::fs::Exists (i2p::fs::DataDirPath("assets.ready"))) break; // assets ready
	// 				numAttempts++;
	// 				std::this_thread::sleep_for (std::chrono::seconds(1)); // otherwise wait for 1 more second
	// 			}
	// 			while (numAttempts <= 10); // 10 seconds max

	// 			// Set application directory
	// 			daemon.setDataDir(dataDir);

	// 			bool daemonInitSuccess = daemon.init(1, argv);
	// 			if(!daemonInitSuccess)
	// 			{
	// 				return "Daemon init failed";
	// 			}

	// 			// Set webconsole language from application
	// 			i2p::i18n::SetLanguage(language);

	// 			daemon.start();
	// 		}
	// 	}
	// 	catch (boost::exception& ex)
	// 	{
	// 		std::stringstream ss;
	// 		ss << boost::diagnostic_information(ex);
	// 		return ss.str();
	// 	}
	// 	catch (std::exception& ex)
	// 	{
	// 		std::stringstream ss;
	// 		ss << ex.what();
	// 		return ss.str();
	// 	}
	// 	catch(...)
	// 	{
	// 		return "unknown exception";
	// 	}
	// 	return "ok";
	// }

	// void stop()
	// {
	// 	daemon.stop();
	// }

	// void SetDataDir(std::string jdataDir)
	// {
	// 	dataDir = jdataDir;
	// }

	// std::string GetDataDir(void)
	// {
	// 	return dataDir;
	// }

	// void SetLanguage(std::string jlanguage)
	// {
	// 	language = jlanguage;
	// }
}
}