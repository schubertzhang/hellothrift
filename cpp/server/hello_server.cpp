#include <stdio.h>
#include <string>

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/shared_ptr.hpp"

#include "gflags/gflags.h"

#include "thrift/TProcessor.h"
#include "thrift/concurrency/PlatformThreadFactory.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/protocol/TJSONProtocol.h"
#include "thrift/server/TNonblockingServer.h"
#include "thrift/server/TSimpleServer.h"
#include "thrift/server/TThreadPoolServer.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/transport/TSocket.h"

#include "hello_types.h"
#include "HelloService.h"

using std::string;

using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace sample;

// Default server parameters
DEFINE_string(transport_type, "framed", "type of TTransport");
DEFINE_string(protocol_type, "compact", "type of TProtocol");
DEFINE_string(server_type, "nonblocking", "type of TServer");
DEFINE_int32(server_port, 9090, "listen port of server");

// Handler to process client calls
// User usually focus on this class
class HelloServiceHandler : virtual public HelloServiceIf {
public:
    HelloServiceHandler() {
        // Your initialization goes here
    }

    void HelloEcho(std::string& _return, const std::string& message, const HelloStruct& body) {
        PrintPidTid();
        printf("\n<><> HelloService Server receive helloEcho <><>\n");
        printf("message: %s\n", message.c_str());
        PrintStruct(body);
        _return = "Hello Thrift, it works!";
    }

    void Hello(const std::string& message, const HelloStruct& body) {
        PrintPidTid();
        printf("\n>>>> HelloService Server receive hello >>>> \n");
        printf("message: %s\n", message.c_str());
        PrintStruct(body);
    }

private:
    void PrintStruct(const HelloStruct& body) {
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

    void PrintPidTid() {
        printf("\n ********* PID = %d, TID = %lu ********\n", getpid(), pthread_self());
    }
};

// Event handler to track the connection and call events
class HelloServerEventHandler : virtual public TServerEventHandler {
public:
    // Called before the server begins.
    void preServe() {
        printf("PID = %d, TID = %lu, server start!\n", getpid(), pthread_self());
    }

    // Called when a new client has connected and is about to being processing.
    void* createContext(boost::shared_ptr<TProtocol> input,
                        boost::shared_ptr<TProtocol> output) {
        printf("PID = %d, TID = %lu, server accept a new connection!\n", getpid(), pthread_self());

        (void)input;
        (void)output;
        return NULL;
    }

    // Called when a client has finished request-handling to delete server context.
    void deleteContext(void* serverContext,
                       boost::shared_ptr<TProtocol>input,
                       boost::shared_ptr<TProtocol>output) {
        printf("PID = %d, TID = %lu, server exit a connection!\n", getpid(), pthread_self());

        (void)serverContext;
        (void)input;
        (void)output;
    }

    // Called when a client is about to call the processor.
    virtual void processContext(void* serverContext,
                                boost::shared_ptr<TTransport> transport) {
        printf("PID = %d, TID = %lu, server is ready to get a next call!\n",
                getpid(), pthread_self());

        (void)serverContext;
        (void)transport;
  }
};

// Example to setup different types of server
int main(int argc, char** argv) {
    // gflags
    google::ParseCommandLineFlags(&argc, &argv, true);

    printf("transport_type = %s\n", FLAGS_transport_type.c_str());
    printf("protocol_type = %s\n", FLAGS_protocol_type.c_str());
    printf("server_type = %s\n", FLAGS_server_type.c_str());
    printf("server_port = %d\n", FLAGS_server_port);

    // Client Connected Transport (created after accept)
    // Pay attention, nonblocking server not use this transportFactory,
    // it always use TFramedTransportFactory internally.
    boost::shared_ptr<TTransportFactory> transport_factory;
    if (FLAGS_transport_type == "framed") {
        transport_factory = boost::shared_ptr<TTransportFactory>(new TFramedTransportFactory());
    } else if (FLAGS_transport_type == "buffered") {
        transport_factory = boost::shared_ptr<TTransportFactory>(new TBufferedTransportFactory());
    } else {
        transport_factory = boost::shared_ptr<TTransportFactory>(new TTransportFactory());
    }

    // Protocol
    boost::shared_ptr<TProtocolFactory> protocol_factory;
    if (FLAGS_protocol_type == "compact") {
        printf("server protocol TCompactProtocol\n");
        protocol_factory = boost::shared_ptr<TProtocolFactory>(new TCompactProtocolFactory());
    } else if (FLAGS_protocol_type == "json") {
        printf("server protocol TJSONProtocol\n");
        protocol_factory = boost::shared_ptr<TProtocolFactory>(new TJSONProtocolFactory());
    } else {
        printf("server protocol TBinaryProtocol\n");
        protocol_factory =  boost::shared_ptr<TProtocolFactory>(new TBinaryProtocolFactory());
    }

    // Processor with Handler (handler implements interface methods)
    boost::shared_ptr<HelloServiceIf> handler(new HelloServiceHandler());
    boost::shared_ptr<TProcessor> processor(new HelloServiceProcessor(handler));
    boost::shared_ptr<TProcessorFactory> processor_factory(
            new TSingletonProcessorFactory(processor));

    // Server
    boost::shared_ptr<TServer> server;

    if (FLAGS_server_type == "nonblocking") {
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

        // using thread pool with maximum 4 threads to handle incoming requests
        boost::shared_ptr<ThreadManager> thread_manager =
                ThreadManager::newSimpleThreadManager(4, 0);
        boost::shared_ptr<PlatformThreadFactory> thread_factory =
                boost::shared_ptr<PlatformThreadFactory>(new PosixThreadFactory());
        thread_manager->threadFactory(thread_factory);
        thread_manager->start();

        // here we do not use transport_factory

        boost::shared_ptr<TNonblockingServer> nonblocking_server(
                new TNonblockingServer(processor_factory,
                                       protocol_factory,
                                       FLAGS_server_port,
                                       thread_manager));

        // There are many options to tune TNonblockingServer for performance,
        // such as following, see TNonblockingServer.h for details.
        nonblocking_server->setNumIOThreads(2);
        nonblocking_server->setUseHighPriorityIOThreads(true);
        nonblocking_server->setMaxConnections(1000);

        server = nonblocking_server;
    } else if (FLAGS_server_type == "threadpool") {
        /*
         * ThreadPool Server
         * one task for each connect, and one thread in the pool will process the task later.
         */
        printf("Server type: TThreadPoolServer\n");
        printf("Each thread for each connect, and block new connect over pool size!\n");

        // Server Transport (for listen and accept client)
        boost::shared_ptr<TServerTransport> server_transport(new TServerSocket(FLAGS_server_port));

        boost::shared_ptr<PlatformThreadFactory> thread_factory(new PlatformThreadFactory());
        boost::shared_ptr<ThreadManager> thread_manager =
                ThreadManager::newSimpleThreadManager(4, 0);
        thread_manager->threadFactory(thread_factory);
        thread_manager->start();

        server = boost::shared_ptr<TServer>(new TThreadPoolServer(processor_factory,
                                                                  server_transport,
                                                                  transport_factory,
                                                                  protocol_factory,
                                                                  thread_manager));

    } else {
        /*
         * This is the most basic simple server. It is single-threaded and runs a
         * continuous loop of accepting a single connection, processing requests on
         * that connection until it closes, and then repeating.
         * This server can only server one client connection.
         */
        printf("Server type: TSimpleServer.\n");
        printf("Simple one thread server, only server one connect at a time. Just for test.\n");

        // Server Transport (for listen and accept client)
        boost::shared_ptr<TServerTransport> server_transport(new TServerSocket(FLAGS_server_port));

        server = boost::shared_ptr<TServer>(new TSimpleServer(processor_factory,
                                                              server_transport,
                                                              transport_factory,
                                                              protocol_factory));
    }

    // event handler is called when the server begin/accept a connect/process a call/exit
    boost::shared_ptr<TServerEventHandler> server_event_handler(new HelloServerEventHandler());
    server->setServerEventHandler(server_event_handler);

    printf("Start Server ... \n");
    server->serve();
    printf("Exit Server ... \n");

    return 0;
}

