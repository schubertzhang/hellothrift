package org.apache.thrift.hello.service;

import org.apache.thrift.TException;

public class HelloHandler implements HelloService.Iface {
    @Override
    public String helloEcho(String message, HelloStruct body) throws InvalidRequestException {
        if (message == null) {
            throw new InvalidRequestException("You should say hello");
        }
        System.out.println("thread: " + Thread.currentThread().getId() +
                ": HelloService got a helloEcho!" + message);
        return "Hello Thrift, it works!";
    }

    @Override
    public void hello(String message, HelloStruct body) throws TException {
        System.out.println("thread: " + Thread.currentThread().getId() +
                ": HelloService got a onway hello!" + message);
    }
}
