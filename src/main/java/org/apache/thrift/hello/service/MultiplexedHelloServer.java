package org.apache.thrift.hello.service;

import org.apache.thrift.TMultiplexedProcessor;
import org.apache.thrift.TProcessorFactory;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TCompactProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TProtocolFactory;
import org.apache.thrift.server.*;
import org.apache.thrift.transport.*;

public class MultiplexedHelloServer {
    public static void main(String[] args) throws Exception {
        // Client Connected Transport (created after accept)
        TTransportFactory transportFactory = new TFramedTransport.Factory();

        // Protocol
        TProtocolFactory protocolFactory = new TJSONProtocol.Factory();

        // Processor with Handler (handler implements interface methods)
        TMultiplexedProcessor multiplexedProcessor = new TMultiplexedProcessor();

        HelloHandler helloHandler = new HelloHandler();
        HelloService.Processor helloProcessor = new HelloService.Processor(helloHandler);
        multiplexedProcessor.registerProcessor("HelloService", helloProcessor);

        Hello2Handler hello2Handler = new Hello2Handler();
        Hello2Service.Processor hello2Processor = new Hello2Service.Processor(hello2Handler);
        multiplexedProcessor.registerProcessor("Hello2Service", hello2Processor);

        TProcessorFactory processorFactory = new TProcessorFactory(multiplexedProcessor);

        // Service Server (with serverTransport, Protocol, Processor...)
        System.out.println("Server type: TThreadedSelectorServer.");
        System.out.println("this is a real reactor architecture! we should use it!!");
        System.out.println("must use TFramedTransport!");
        if (!(transportFactory instanceof TFramedTransport.Factory)) {
            System.out.println("The current connection transport is not TFramedTransport!");
            System.out.println("Server not startup and exit!");
            return;
        }

        // Server Transport (for listen and accept client)
        TNonblockingServerTransport nonblockingServerTransport = new TNonblockingServerSocket(9090, 0);

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

        TServer helloServer = new TThreadedSelectorServer(threadedSelectorServerArgs);
        helloServer.setServerEventHandler(new HelloServerEventHandler());

        // Start service
        System.out.println("Start Server ...");
        helloServer.serve();
        System.out.println("Exit Server ...");
    }
}
