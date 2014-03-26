package org.apache.thrift.hello.service;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TMultiplexedProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TFramedTransport;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.management.ManagementFactory;

public class MultiplexedHelloClient {
    public static void main(String[] args) throws Exception {
        HelloStruct helloBody = new HelloStruct();

        // Transport
        TSocket socket = new TSocket("localhost", 9090);
        socket.setTimeout(2000);
        TTransport transport = new TFramedTransport(socket);

        // Protocol
        TProtocol protocol = new TJSONProtocol(transport);

        // Client
        TMultiplexedProtocol mp = new TMultiplexedProtocol(protocol, "HelloService");
        HelloService.Client helloClient = new HelloService.Client(mp);

        TMultiplexedProtocol mp2 = new TMultiplexedProtocol(protocol, "Hello2Service");
        Hello2Service.Client hello2Client = new Hello2Service.Client(mp2);

        // Connect
        transport.open();

        // A interactive shell
        BufferedReader cmdReader = new BufferedReader(new InputStreamReader(System.in));
        while(true) {
            System.out.print("MultiplexedHelloThrift>");
            String message = cmdReader.readLine();

            if (message.equalsIgnoreCase("exit")) {
                break;
            } else if (message.equalsIgnoreCase("stop")) {
                System.out.println("stop and exit!");
                break;
            } else if (message.equalsIgnoreCase("hello")) {
                String echoMessage = helloClient.helloEcho(message + ManagementFactory.getRuntimeMXBean().getName(), helloBody);
                System.out.println("echo: " + echoMessage);
            } else if (message.equalsIgnoreCase("hello2")) {
                String echoMessage = hello2Client.hello2Echo(message);
                System.out.println("echo: " + echoMessage);
            } else {
                // continue
            }
        }

        // Disconnect
        transport.close();
    }
}
