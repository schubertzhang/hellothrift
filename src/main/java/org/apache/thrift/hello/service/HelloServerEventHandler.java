package org.apache.thrift.hello.service;

import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.server.ServerContext;
import org.apache.thrift.server.TServerEventHandler;
import org.apache.thrift.transport.TTransport;

import java.util.concurrent.atomic.AtomicLong;

public class HelloServerEventHandler implements TServerEventHandler {
    private AtomicLong nextConnectionId = new AtomicLong(0);

    public class HelloServerContext implements ServerContext {
        private long connectionId = 0;

        public HelloServerContext(long id) {
           this.connectionId = id;
        }

        public long getConnectionId() {
            return connectionId;
        }
    }

    @Override
    public void preServe() {
        System.out.println("thread: " + Thread.currentThread().getId() + ", service start!" );
    }

    @Override
    public ServerContext createContext(TProtocol input, TProtocol output) {
        long myConnId = nextConnectionId.incrementAndGet();
        System.out.println("thread: " + Thread.currentThread().getId() +
                ", connection= " + myConnId +
                ", service accept a new connection!" );
        return new HelloServerContext(myConnId);
    }

    @Override
    public void deleteContext(ServerContext serverContext, TProtocol input, TProtocol output) {
        System.out.println("thread: " + Thread.currentThread().getId() +
                ", connection= " + ((HelloServerContext)serverContext).getConnectionId() +
                ", service exit connection!");
    }

    @Override
    public void processContext(ServerContext serverContext, TTransport inputTransport, TTransport outputTransport) {
        System.out.println("thread: " + Thread.currentThread().getId() +
                ", connection= " + ((HelloServerContext)serverContext).getConnectionId() +
                ", service is ready to get a next call!");
    }
}
