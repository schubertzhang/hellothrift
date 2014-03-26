package org.apache.thrift.hello.service;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TCompactProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TFastFramedTransport;
import org.apache.thrift.transport.TFramedTransport;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.management.ManagementFactory;
import java.util.Date;

public class HelloClient  {
    public static void main(String[] args) throws Exception {

        String transportType = "framed";
        if (args.length > 0) {
            transportType = args[0];
        }

        String protocolType = "binary";
        if (args.length > 1) {
            protocolType = args[1];
        }

        String serverHost = "localhost";
        if (args.length > 2) {
            serverHost = args[2];
        }

        int serverPort = 9090;
        if (args.length >= 3) {
            serverPort = Integer.parseInt(args[1]);
        }

        System.out.println("Client arguments: transportType = " + transportType);
        System.out.println("Client arguments:  protocolType = " + protocolType);
        System.out.println("Client arguments:    serverHost = " + serverHost);
        System.out.println("Client arguments:    serverPort = " + serverPort);

        HelloStruct helloBody = new HelloStruct();

        // Transport
        TSocket socket = new TSocket(serverHost, serverPort);
        socket.setTimeout(2000);
        TTransport transport;
        if (transportType.equalsIgnoreCase("framed")) {
            transport = new TFramedTransport(socket);
        } else if (transportType.equalsIgnoreCase("fastframed")) {
            transport = new TFastFramedTransport(socket);
        } else {
            transport = socket;
        }

        // Protocol
        TProtocol protocol;
        if (protocolType.equalsIgnoreCase("json")) {
            protocol = new TJSONProtocol(transport);
        } else if (protocolType.equalsIgnoreCase("compact")) {
            protocol = new TCompactProtocol(transport);
        } else {
           protocol = new TBinaryProtocol(transport);
        }

        // Client
        HelloService.Client client = new HelloService.Client(protocol);

        // Connect
        transport.open();

        // A interactive shell
        BufferedReader cmdReader = new BufferedReader(new InputStreamReader(System.in));
        while(true) {
            System.out.print("HelloThrift>");
            String message = cmdReader.readLine();

            if (message.equalsIgnoreCase("exit")) {
                break;
            } else if (message.equalsIgnoreCase("stop")) {
                System.out.println("stop and exit!");
                break;
            } else if (message.toLowerCase().startsWith("hello")) {
                String echoMessage = client.helloEcho(message + ManagementFactory.getRuntimeMXBean().getName(), helloBody);
                System.out.println("echo: " + echoMessage);
            } else {
                client.hello(message, helloBody);
                // no echo
            }
        }

        // Disconnect
        transport.close();
    }

}
