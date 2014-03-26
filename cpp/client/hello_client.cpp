#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/shared_ptr.hpp"

#include "gflags/gflags.h"

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/protocol/TJSONProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/transport/TSocket.h"
#include "thrift/transport/TTransportUtils.h"

#include "hello_types.h"
#include "HelloService.h"

using std::cin;
using std::string;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace sample;

// Default server parameters
DEFINE_string(transport_type, "framed", "type of TTransport");
DEFINE_string(protocol_type, "compact", "type of TProtocol");
DEFINE_string(server_host, "localhost", "hostname or ip of server");
DEFINE_int32(server_port, 9090, "listen port of server");

// constants
const int kCinInputBufferSize = 1024;
const int kMessageBufferSize = 128;

// Example Thrift Client
int main(int argc, char** argv) {
    // gflags
    google::ParseCommandLineFlags(&argc, &argv, true);

    printf("transport_type = %s\n", FLAGS_transport_type.c_str());
    printf("protocol_type = %s\n", FLAGS_protocol_type.c_str());
    printf("server_host = %s\n", FLAGS_server_host.c_str());
    printf("server_port = %d\n", FLAGS_server_port);

    // Transport
    boost::shared_ptr<TTransport> socket(new TSocket(FLAGS_server_host, FLAGS_server_port));
    boost::shared_ptr<TTransport> transport;
    if (FLAGS_transport_type == "framed") {
        transport = boost::shared_ptr<TTransport>(new TFramedTransport(socket));
    } else if (FLAGS_transport_type == "buffered") {
        transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
    } else {
        transport = socket;
    }

    // Protocol
    boost::shared_ptr<TProtocol> protocol;
    if (FLAGS_protocol_type == "compact") {
        printf("server protocol TCompactProtocol\n");
        protocol = boost::shared_ptr<TProtocol>(new TCompactProtocol(transport));
    } else if (FLAGS_protocol_type == "json") {
        printf("server protocol TJSONProtocol\n");
        protocol = boost::shared_ptr<TProtocol>(new TJSONProtocol(transport));
    } else {
        printf("server protocol TBinaryProtocol\n");
        protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
    }

    // RPC message parameters
    string returned_message;
    HelloStruct hello_struct;
    hello_struct.__set_a(true);
    hello_struct.__set_b(100);
    hello_struct.__set_c(2000);
    hello_struct.__set_d(30000);
    hello_struct.__set_e(400000);
    hello_struct.__set_f(3.1415926);
    hello_struct.__set_g("haha hehe!");
    hello_struct.__set_g("god!");

    // Client
    HelloServiceClient client(protocol);

    try {
        // Connect to Server
        transport->open();

        char cin_input[kCinInputBufferSize];
        char message[kMessageBufferSize];
        while(true) {
            printf("HelloThrift>");
            cin.getline(cin_input, kCinInputBufferSize - 1);
            cin_input[kCinInputBufferSize - 1] = 0;

            if (strcmp(cin_input, "exit") == 0)  {
                break;
            } else if (strcmp(cin_input, "hello") == 0) {
                snprintf(message, kMessageBufferSize, "Hello Thrift! please echo me [ %d ]\n",
                         getpid());
                client.HelloEcho(returned_message, message, hello_struct);
                printf("helloEcho response: %s\n\n\n", returned_message.c_str());
            } else {
                snprintf(message, kMessageBufferSize, "Hello Thrift! oneway! %s [ %d ]\n",
                         cin_input, getpid());
                client.Hello(message, hello_struct);
                // no echo
            }
        }

        transport->close();
    } catch (const TException &te) {
        printf("ERROR: get an excdeption TException!\n");
    }

    return 0;
}

