// Copyright 2013, Baidu Inc.
// Author: Zhang Guangqiang <zhangguangqiang@baidu.com>
//

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include "thrift/transport/TSocket.h"
#include "thrift/transport/TTransportUtils.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/protocol/TJSONProtocol.h"

#include "hello_types.h"
#include "HelloService.h"

using namespace std;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using std::string;

using namespace baidu::longscheduler::sample;

const string kServerHost = "localhost";
const int    kServerPort = 9090;

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

    string serverHost = kServerHost;
    if (argc > 3) {
        serverHost = argv[3];
        boost::to_lower(serverHost);
    }

    int serverPort = kServerPort;
    if (argc >= 4) {
        serverPort = boost::lexical_cast<int>(argv[4]);
    }

    printf("Server arguments: transportType = %s\n", transportType.c_str());
    printf("Server arguments:  protocolType = %s\n", protocolType.c_str());
    printf("Server arguments:    serverHost = %s\n", serverHost.c_str());
    printf("Server arguments:    serverPort = %d\n", serverPort);

    // Transport
    boost::shared_ptr<TTransport> socket(new TSocket(kServerHost, kServerPort));
    boost::shared_ptr<TTransport> transport;
    if (transportType == "framed") {
        transport = boost::shared_ptr<TTransport>(new TFramedTransport(socket));
    } else if (transportType == "buffered") {
        transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
    } else {
        transport = socket;
    }

    // Protocol
    boost::shared_ptr<TProtocol> protocol;
    if (protocolType == "json") {
        printf("server protocol TJSONProtocol\n");
        protocol = boost::shared_ptr<TProtocol>(new TJSONProtocol(transport));
    } else if (protocolType == "compact") {
        printf("server protocol TCompactProtocol\n");
        protocol = boost::shared_ptr<TProtocol>(new TCompactProtocol(transport));
    } else {
        printf("server protocol TBinaryProtocol\n");
        protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
    }

    // Client
    HelloServiceClient client(protocol);

    // Connect to Server
    transport->open();

    // RPC message parameters
    string returnedMessage;
    HelloStruct helloStruct;
    helloStruct.__set_a(true);
    helloStruct.__set_b(100);
    helloStruct.__set_c(2000);
    helloStruct.__set_d(30000);
    helloStruct.__set_e(400000);
    helloStruct.__set_f(3.1415926);
    helloStruct.__set_g("haha hehe!");
    helloStruct.__set_g("god!");

    char cinInput[128];
    char message[64];
    while(true) {
        printf("HelloThrift>\n");
        cin.getline(cinInput, 128);

        if (strcmp(cinInput, "exit") == 0)  {
            break;
        } else if (strcmp(cinInput, "stop") == 0) {
            printf("stop and exit!\n");
            break;
        } else if (strcmp(cinInput, "hello") == 0) {
            sprintf(message, "Hello Thrift! please echo me [ %d ]\n", getpid());
            client.helloEcho(returnedMessage, message,  helloStruct);
            printf("helloEcho response: %s\n\n\n", returnedMessage.c_str());
        } else {
            client.hello("Hello Thrift! oneway!" + getpid(), helloStruct);
            // no echo
        }
    }

    transport->close();

    return 0;
}

