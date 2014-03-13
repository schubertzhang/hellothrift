package org.apache.thrift.hello.service;

import org.apache.thrift.TException;

public class Hello2Handler implements Hello2Service.Iface {

    @Override
    public String hello2Echo(String message) throws TException {
        if (message == null) {
            throw new InvalidRequestException("You should say hello2");
        }
        System.out.println("thread: " + Thread.currentThread().getId() +
                                   ": Hello2Service got a hello2Echo!" + message);
        return "Hello2 Thrift, it works!";
    }
}
