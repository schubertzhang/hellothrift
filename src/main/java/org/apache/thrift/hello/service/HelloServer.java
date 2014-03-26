package org.apache.thrift.hello.service;

import org.apache.thrift.TProcessorFactory;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TCompactProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TProtocolFactory;
import org.apache.thrift.server.*;
import org.apache.thrift.transport.*;

public class HelloServer {
    public static void main(String[] args) throws Exception {

        String transportType = "framed";
        if (args.length > 0) {
            transportType = args[0];
        }

        String protocolType = "binary";
        if (args.length > 1) {
            protocolType = args[1];
        }

        String serverType = "threadedselector";
        if (args.length > 2) {
            serverType = args[2];
        }

        int serverPort = 9090;
        if (args.length > 3) {
            serverPort = Integer.parseInt(args[3]);
        }

        System.out.println("Server arguments:" + " transportType = " + transportType);
        System.out.println("Server arguments:" + "  protocolType = " + protocolType);
        System.out.println("Server arguments:" + "    serverType = " + serverType);
        System.out.println("Server arguments:" + "    serverPort = " + serverPort );

        // Client Connected Transport (created after accept)
        TTransportFactory transportFactory;
        if (transportType.equalsIgnoreCase("framed")) {
            transportFactory = new TFramedTransport.Factory();
        } else if (transportType.equalsIgnoreCase("fastframed")) {
            transportFactory = new TFastFramedTransport.Factory();
        } else {
            // .equals("buffered") => default value
            transportFactory = new TTransportFactory();
        }

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
        TServer helloServer;

        if (serverType.equalsIgnoreCase("threadedselector")) {
            /*
             * A Half-Sync/Half-Async service with a separate pool of threads to handle
             * non-blocking I/O. Accepts are handled on a single thread, and a configurable
             * number of nonblocking selector threads manage reading and writing of client
             * connections. A synchronous worker thread pool handles processing of requests.
             *
             * Performs better than TNonblockingServer/THsHaServer in multi-core
             * environments when the the bottleneck is CPU on the single selector thread
             * handling I/O. In addition, because the accept handling is decoupled from
             * reads/writes and invocation, the service has better ability to handle back-
             * pressure from new connections (e.g. stop accepting when busy).
             *
             * Like TNonblockingServer, it relies on the use of TFramedTransport.
             */
            System.out.println("Server type: TThreadedSelectorServer.");
            System.out.println("this is a real reactor architecture! we should use it!!");
            System.out.println("must use TFramedTransport!");
            if (!(transportFactory instanceof TFramedTransport.Factory)) {
                System.out.println("The current connection transport is not TFramedTransport!");
                System.out.println("Server not startup and exit!");
                return;
            }

            // Server Transport (for listen and accept client)
            TNonblockingServerTransport nonblockingServerTransport = new TNonblockingServerSocket(serverPort, 0);

            // Sever Arguments
            TThreadedSelectorServer.Args threadedSelectorServerArgs
                    = new TThreadedSelectorServer.Args(nonblockingServerTransport);
            threadedSelectorServerArgs.processorFactory(processorFactory);
            threadedSelectorServerArgs.protocolFactory(protocolFactory);
            threadedSelectorServerArgs.transportFactory(transportFactory);

            threadedSelectorServerArgs.selectorThreads(2);
            threadedSelectorServerArgs.workerThreads(5);
            threadedSelectorServerArgs.acceptQueueSizePerThread(4);
            threadedSelectorServerArgs.stopTimeoutVal(10);
            threadedSelectorServerArgs.acceptPolicy(TThreadedSelectorServer.Args.AcceptPolicy.FAST_ACCEPT);

            helloServer = new TThreadedSelectorServer(threadedSelectorServerArgs);
        } else if (serverType.equalsIgnoreCase("hsha")) {
            /*
             * An extension of the TNonblockingServer to a Half-Sync/Half-Async service.
             * Like TNonblockingServer, it relies on the use of TFramedTransport.
             */
            System.out.println("Server type: THsHaServer.");
            System.out.println("this is an extension of the TNonblockingServer!");
            System.out.println("must use TFramedTransport!");
            if (!(transportFactory instanceof TFramedTransport.Factory)) {
                System.out.println("The current connection transport is not TFramedTransport!");
                System.out.println("Server not startup and exit!");
                return;
            }

            // Server Transport (for listen and accept client)
            TNonblockingServerTransport nonblockingServerTransport = new TNonblockingServerSocket(serverPort, 0);

            // Sever Arguments
            THsHaServer.Args hshaServerArgs = new THsHaServer.Args(nonblockingServerTransport);
            hshaServerArgs.transportFactory(transportFactory);
            hshaServerArgs.protocolFactory(protocolFactory);
            hshaServerArgs.processorFactory(processorFactory);

            hshaServerArgs.workerThreads(4);
            hshaServerArgs.stopTimeoutVal(10);

            helloServer = new THsHaServer(hshaServerArgs);
        } else if (serverType.equalsIgnoreCase("nonblocking")) {
            /*
             * A nonblocking TServer implementation. This allows for fairness amongst all
             * connected clients in terms of invocations.
             *
             * This service is inherently single-threaded. If you want a limited thread pool
             * coupled with invocation-fairness, see THsHaServer.
             *
             * To use this service, you MUST use a TFramedTransport at the outermost
             * transport, otherwise this service will be unable to determine when a whole
             * method call has been read off the wire. Clients must also use TFramedTransport.
             */
            System.out.println("Server type: TNonblockingServer.");
            System.out.println("allows for fairness amongst all connected clients in terms of invocations.");
            System.out.println("single-threaded!");
            System.out.println("must use TFramedTransport!");
            if (!(transportFactory instanceof TFramedTransport.Factory)) {
                System.out.println("The current connection transport is not TFramedTransport!");
                System.out.println("Server not startup and exit!");
                return;
            }

            // Server Transport (for listen and accept client)
            TNonblockingServerTransport nonblockingServerTransport = new TNonblockingServerSocket(serverPort, 0);

            // Sever Arguments
            TNonblockingServer.Args nonblockingServerArgs = new TNonblockingServer.Args(nonblockingServerTransport);
            nonblockingServerArgs.transportFactory(transportFactory);
            nonblockingServerArgs.protocolFactory(protocolFactory);
            nonblockingServerArgs.processorFactory(processorFactory);

            helloServer = new TNonblockingServer(nonblockingServerArgs);
        } else if (serverType.equalsIgnoreCase("threadpool")) {
            // ThreadPool Server
            // each task for each connect, and one thread in the pool will process the task later.
            System.out.println("Server type: TThreadPoolServer ");
            System.out.println("each thread for each connect, and block new connect over pool size!");

            // Server Transport (for listen and accept client)
            TServerTransport serverTransport = new TServerSocket(serverPort);

            // Sever Arguments
            TThreadPoolServer.Args threadPoolServerArgs = new TThreadPoolServer.Args(serverTransport);
            threadPoolServerArgs.transportFactory(transportFactory);
            threadPoolServerArgs.protocolFactory(protocolFactory);
            threadPoolServerArgs.processorFactory(processorFactory);

            threadPoolServerArgs.minWorkerThreads(2);
            threadPoolServerArgs.maxWorkerThreads(4);

            helloServer = new TThreadPoolServer(threadPoolServerArgs);
        } else {
            // Simple one thread service, only service one connect at a time. Just for test.
            System.out.println("Server type: TSimpleServer.");
            System.out.println("simple one thread service, only service one connect at a time. Just for test.");

            // Server Transport (for listen and accept client)
            TServerTransport serverTransport = new TServerSocket(serverPort);

            // Sever Arguments
            TServer.Args serverArgs = new TServer.Args(serverTransport);
            serverArgs.transportFactory(transportFactory);
            serverArgs.protocolFactory(protocolFactory);
            serverArgs.processorFactory(processorFactory);

            helloServer = new TSimpleServer(serverArgs);
        }

        // event handler is called when the service begin/accept a connect/process a call/exit
        helloServer.setServerEventHandler(new HelloServerEventHandler());

        // Start service
        System.out.println("Start Server ...");
        helloServer.serve();
        System.out.println("Exit Server ...");
    }
}
