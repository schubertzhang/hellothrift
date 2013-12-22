// Copyright 2013, Baidu Inc.
// Author: Zhang Guangqiang <zhangguangqiang@baidu.com>
//

#include <string>
#include <stdio.h>

#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include "thrift/transport/TSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TJSONProtocol.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/concurrency/PlatformThreadFactory.h"
#include "thrift/server/TSimpleServer.h"
#include "thrift/server/TThreadPoolServer.h"
#include "thrift/server/TNonblockingServer.h"
#include "thrift/TProcessor.h"

#include "hello_types.h"
#include "HelloService.h"

using std::string;

using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace baidu::longscheduler::sample;

// Handler to process client calls
class HelloServiceHandler : virtual public HelloServiceIf {
public:
    HelloServiceHandler() {
        // Your initialization goes here
    }

    void helloEcho(std::string& _return, const std::string& message, const HelloStruct& body) {
        printPidTid();
        printf("\n\n<><> HelloService Server receive helloEcho <><>\n");
        printf("message: %s\n", message.c_str());
        printStruct(body);
        _return = "Hello Thrift, it works!";
    }

    void hello(const std::string& message, const HelloStruct& body) {
        printPidTid();
        printf("\n\n>>>> HelloService Server receive hello >>>> \n");
        printf("message: %s\n", message.c_str());
        printStruct(body);
    }

private:
    void printStruct(const HelloStruct& body) {
        printf("a = %d\n", body.a);
        printf("b = %d\n", body.b);
        printf("c = %d\n", body.c);
        printf("d = %d\n", body.d);
        printf("e = %ld\n", body.e);
        printf("f = %f\n", body.f);
        if (body.__isset.g) {
            printf("g = %s\n", body.g.c_str());
        }
        if (body.__isset.h) {
            printf("h = %s\n", body.h.c_str());
        }
    }

    void printPidTid() {
        printf("\n ********* PID = %d, TID = %lu ********\n", getpid(), pthread_self());
    }
};

// Event handler to track the connection and call events
class HelloServerEventHandler : virtual public TServerEventHandler {
    // Called before the server begins.
    void preServe() {
        printf("PID = %d, TID = %lu, server start!\n", getpid(), pthread_self());
    }

    /**
     * Called when a new client has connected and is about to being processing.
     */
    virtual void* createContext(boost::shared_ptr<TProtocol> input,
                                boost::shared_ptr<TProtocol> output) {
        printf("PID = %d, TID = %lu, server accept a new connection!\n",
                getpid(), pthread_self());

        (void)input;
        (void)output;
        return NULL;
    }

    /**
     * Called when a client has finished request-handling to delete server
     * context.
     */
    virtual void deleteContext(void* serverContext,
                               boost::shared_ptr<TProtocol>input,
                               boost::shared_ptr<TProtocol>output) {
        printf("PID = %d, TID = %lu, server exit connection!\n",
                getpid(), pthread_self());

        (void)serverContext;
        (void)input;
        (void)output;
    }

    /**
     * Called when a client is about to call the processor.
     */
    virtual void processContext(void* serverContext,
                                boost::shared_ptr<TTransport> transport) {
        printf("PID = %d, TID = %lu, server is ready to get a next call!\n",
                getpid(), pthread_self());

        (void)serverContext;
        (void)transport;
  }
};


const int kServerPort = 9090;

int main(int argc, char** argv) {

    string transportType = "framed";
    if (argc > 1) {
        transportType = argv[1];
        boost::to_lower(transportType);
    }

    string protocolType = "compact";
    if (argc > 2) {
        protocolType = argv[2];
        boost::to_lower(protocolType);
    }

    string serverType = "nonblocking";
    if (argc > 3) {
        serverType = argv[3];
        boost::to_lower(serverType);
    }

    int serverPort = kServerPort;
    if (argc >= 4) {
        serverPort = boost::lexical_cast<int>(argv[4]);
    }

    printf("Server arguments: transportType = %s\n", transportType.c_str());
    printf("Server arguments:  protocolType = %s\n", protocolType.c_str());
    printf("Server arguments:    serverType = %s\n", serverType.c_str());
    printf("Server arguments:    serverPort = %d\n", serverPort);

    // Client Connected Transport (created after accept)
    // Pay attention, nonblocking server not use this transportFactory,
    // it always use TFramedTransportFactory internal.
    boost::shared_ptr<TTransportFactory> transportFactory;
    if (transportType == "framed") {
        transportFactory = boost::shared_ptr<TTransportFactory>(new TFramedTransportFactory());
    } else if (transportType == "buffered") {
        transportFactory = boost::shared_ptr<TTransportFactory>(new TBufferedTransportFactory());
    } else {
        transportFactory = boost::shared_ptr<TTransportFactory>(new TTransportFactory());
    }

    // Protocol
    boost::shared_ptr<TProtocolFactory> protocolFactory;
    if (protocolType == "json") {
        printf("server protocol TJSONProtocol\n");
        protocolFactory = boost::shared_ptr<TProtocolFactory>(new TJSONProtocolFactory());
    } else if (protocolType == "compact") {
        printf("server protocol TCompactProtocol\n");
        protocolFactory = boost::shared_ptr<TProtocolFactory>(new TCompactProtocolFactory());
    } else {
        printf("server protocol TBinaryProtocol\n");
        protocolFactory =  boost::shared_ptr<TProtocolFactory>(new TBinaryProtocolFactory());
    }

    // Processor with Handler (handler implements interface methods)
    boost::shared_ptr<HelloServiceIf> handler(new HelloServiceHandler());
    boost::shared_ptr<TProcessor> processor(new HelloServiceProcessor(handler));
    boost::shared_ptr<TProcessorFactory> processorFactory(new TSingletonProcessorFactory(processor));

    // Server
    boost::shared_ptr<TServer> server;

    if (serverType == "nonblocking") {
        /*
         * This is a non-blocking server in C++ for high performance that
         * operates a set of IO threads (by default only one). It assumes that
         * all incoming requests are framed with a 4 byte length indicator and
         * writes out responses using the same framing.
         *
         * It does not use the TServerTransport framework, but rather has socket
         * operations hardcoded for use with select.
         *
         */

        /*
         * If you are writing an application that will serve a lot of connection
         * (like php front end calling thrift service), you'd better use TNonblockingServer.
         * TNonblockingServer can accept a lot of connections while throttling the processor
         * threads using a pool.
         * TNonblockingServer with a thread pool is the c++ alternative of the JAVA THsHaServer;
         * TNonblockingServer withOUT a thread pool is the c++ alternative of the JAVA
         * TNonblockingServer; refer to: http://wiki.apache.org/thrift/ThriftUsageC%2B%2B
         */
        printf("Server type: TNonblockingServer\n");
        printf("TNonblockingServer use TFramedTransport, and client must use it too!\n");

        // using thread pool with maximum 2 threads to handle incoming requests
        boost::shared_ptr<ThreadManager> threadManager =
                ThreadManager::newSimpleThreadManager(2, 0);
        boost::shared_ptr<PlatformThreadFactory> threadFactory =
                boost::shared_ptr<PlatformThreadFactory>(new PosixThreadFactory());
        threadManager->threadFactory(threadFactory);
        threadManager->start();

        // here we do not use transportFactory

        boost::shared_ptr<TNonblockingServer> nonblockingServer(
                                                    new TNonblockingServer(processorFactory,
                                                                           protocolFactory,
                                                                           serverPort,
                                                                           threadManager));

        // There are many options to tuning TNonblockingServer, such as following.
        nonblockingServer->setNumIOThreads(2);
        nonblockingServer->setUseHighPriorityIOThreads(true);
        nonblockingServer->setMaxConnections(10000);

        server = nonblockingServer;

    } else if (serverType == "threadpool") {
        // ThreadPool Server
        // one task for each connect, and one thread in the pool will process the task later.
        printf("Server type: TThreadPoolServer\n");
        printf("Each thread for each connect, and block new connect over pool size!\n");

        // Server Transport (for listen and accept client)
        boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(serverPort));

        boost::shared_ptr<PlatformThreadFactory> threadFactory(new PlatformThreadFactory());
        boost::shared_ptr<ThreadManager> threadManager =
                ThreadManager::newSimpleThreadManager(2, 0);
        threadManager->threadFactory(threadFactory);
        threadManager->start();

        server = boost::shared_ptr<TServer>(new TThreadPoolServer(processorFactory,
                                                                  serverTransport,
                                                                  transportFactory,
                                                                  protocolFactory,
                                                                  threadManager));

    } else {
        // This is the most basic simple server. It is single-threaded and runs a
        // continuous loop of accepting a single connection, processing requests on
        // that connection until it closes, and then repeating.
        // This server can only server one client connection.
        printf("Server type: TSimpleServer.\n");
        printf("Simple one thread server, only server one connect at a time. Just for test.\n");

        // Server Transport (for listen and accept client)
        boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(serverPort));

        server = boost::shared_ptr<TServer>(new TSimpleServer(processorFactory,
                                                              serverTransport,
                                                              transportFactory,
                                                              protocolFactory));
    }

    // event handler is called when the server begin/accept a connect/process a call/exit
    boost::shared_ptr<HelloServerEventHandler> serverEventHandler(new HelloServerEventHandler());
    server->setServerEventHandler(serverEventHandler);

    printf("Start Server ... \n");
    server->serve();
    printf("Exit Server ... \n");

    return 0;
}

