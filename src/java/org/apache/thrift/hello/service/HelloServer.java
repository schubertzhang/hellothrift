package org.apache.thrift.hello.service;

import org.apache.thrift.TProcessorFactory;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TCompactProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TProtocolFactory;
import org.apache.thrift.server.TSimpleServer;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TServerSocket;
import org.apache.thrift.transport.TServerTransport;
import org.apache.thrift.transport.TTransportFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class HelloServer {
    private static final Logger logger = LoggerFactory.getLogger(HelloServer.class);

    public static void main(String[] args) throws Exception {
        TServer helloServer;

        int serverPort = 10000;
        String protocolType = "binary";
        String serverType = "simpole";
        if (args.length >= 1) {
            serverPort = Integer.parseInt(args[0]);
        }
        if (args.length >= 2) {
            protocolType = args[1];
        }
        if (args.length >= 3) {
            serverType = args[2];
        }

        logger.info("server arguments:" + " port:" + serverPort + " protocol:"  + protocolType + " type:" + serverType);

        // Transport
        // Server Transport (for listen and accept client)
        TServerTransport serverTransport = new TServerSocket(serverPort);
        // Client Connected Transport (after accept)
        TTransportFactory transportFactory = new TTransportFactory();

        // Protocol
        TProtocolFactory protocolFactory;
        if (protocolType.equalsIgnoreCase("json")) {
            protocolFactory = new TJSONProtocol.Factory();
        } else if (protocolType.equalsIgnoreCase("compact")) {
            protocolFactory = new TCompactProtocol.Factory();
        } else {
            protocolFactory = new TBinaryProtocol.Factory();
        }

        // Processor with Handler (handler implements interface methods)
        HelloHandler handler = new HelloHandler();
        HelloService.Processor processor = new HelloService.Processor(handler);
        TProcessorFactory processorFactory = new TProcessorFactory(processor);

        // Service Server (with serverTransport, Protocol, Processor...)

        if (serverType.equalsIgnoreCase("threadpool")) {
            // ThreadPool Server, one task for each connect, and one thread in the pool will process the task later.
            TThreadPoolServer.Args threadPoolServerArgs = new TThreadPoolServer.Args(serverTransport);
            threadPoolServerArgs.transportFactory(transportFactory);
            threadPoolServerArgs.protocolFactory(protocolFactory);
            threadPoolServerArgs.processorFactory(processorFactory);

            threadPoolServerArgs.minWorkerThreads(2);
            threadPoolServerArgs.maxWorkerThreads(4);

            helloServer = new TThreadPoolServer(threadPoolServerArgs);
        } else {
            // Simple one thread server, only server one connect at a time
            TServer.Args serverArgs = new TServer.Args(serverTransport);
            serverArgs.transportFactory(transportFactory);
            serverArgs.protocolFactory(protocolFactory);
            serverArgs.processorFactory(processorFactory);
            helloServer = new TSimpleServer(serverArgs);
        }

        // event handler is called when the server begin/accept a connect/process a call/exit
        helloServer.setServerEventHandler(new HelloServerEventHandler());

        // Start server
        helloServer.serve();
    }

}
