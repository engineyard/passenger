#include "tut.h"
#include "support/Support.h"

#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "ApplicationPool/Pool.h"
#include "ApplicationPool/Server.h"
#include "ApplicationPool/Client.h"
#include "Utils.h"

using namespace Passenger;
using namespace std;
using namespace boost;
using namespace Test;

namespace tut {
	struct ApplicationPool_Server_PoolTest {
		ApplicationPool::AccountsDatabasePtr accountsDatabase;
		shared_ptr<ApplicationPool::Pool> realPool;
		shared_ptr<ApplicationPool::Server> server;
		shared_ptr<ApplicationPool::Client> pool, pool2;
		shared_ptr<oxt::thread> serverThread;
		string socketFilename;
		
		ApplicationPool_Server_PoolTest() {
			socketFilename = getPassengerTempDir() + "/master/pool_server.sock";
			accountsDatabase = ptr(new ApplicationPool::AccountsDatabase());
			accountsDatabase->add("test", "12345", false);
			
			realPool = ptr(new ApplicationPool::Pool("../bin/passenger-spawn-server"));
			server   = ptr(new ApplicationPool::Server(socketFilename, accountsDatabase, realPool));
			pool     = ptr(new ApplicationPool::Client(socketFilename, "test", "12345"));
			pool2    = ptr(new ApplicationPool::Client(socketFilename, "test", "12345"));
			serverThread = ptr(new oxt::thread(
				boost::bind(&ApplicationPool::Server::mainLoop, server.get())
			));
		}
		
		~ApplicationPool_Server_PoolTest() {
			if (serverThread != NULL) {
				serverThread->interrupt_and_join();
			}
		}
		
		ApplicationPool::Ptr newPoolConnection() {
			return ptr(new ApplicationPool::Client(socketFilename, "test", "12345"));
		}
	};
	
	DEFINE_TEST_GROUP(ApplicationPool_Server_PoolTest);
	
	#define USE_TEMPLATE
	#include "ApplicationPool_PoolTestCases.cpp"
}
