package org.apache.thrift.hello.service;

import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.server.ServerContext;
import org.apache.thrift.server.TServerEventHandler;
import org.apache.thrift.transport.TTransport;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class HelloServerEventHandler implements TServerEventHandler {
    private static final Logger logger = LoggerFactory.getLogger(HelloServerEventHandler.class);

    @Override
    public void preServe() {
        logger.info("thread {} server start", Thread.currentThread().getId());
    }

    @Override
    public ServerContext createContext(TProtocol input, TProtocol output) {
        logger.info("thread {} server accept a new connect", Thread.currentThread().getId());
        return null;
    }

    @Override
    public void deleteContext(ServerContext serverContext, TProtocol input, TProtocol output) {
        logger.info("thread {} server this connect exit", Thread.currentThread().getId());
    }

    @Override
    public void processContext(ServerContext serverContext, TTransport inputTransport, TTransport outputTransport) {
        logger.info("thread {} server is ready to get a next call", Thread.currentThread().getId());
    }
}
