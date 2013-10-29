package org.apache.thrift.hello.service;

import org.apache.thrift.TException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static org.apache.thrift.hello.service.HelloService.Iface;

public class HelloHandler implements Iface {
    private static final Logger logger = LoggerFactory.getLogger(HelloHandler.class);

    @Override
    public String helloEcho(String message, HelloStruct body) throws InvalidRequestException {
        if ((message == null) || (!message.equalsIgnoreCase("hello"))) {
            throw new InvalidRequestException("You should say hello");
        }
        logger.info("HelloService got a hello!");
        return "Hello Thrift, it works!";
    }

    @Override
    public void hello(String message, HelloStruct body) throws TException {
        logger.info("HelloService got a oneway message:" + message);
    }
}
