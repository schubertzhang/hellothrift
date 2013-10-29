package org.apache.thrift.hello.service;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TCompactProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class HelloClient  {

    private static final Logger logger = LoggerFactory.getLogger(HelloClient.class);

    public static void main(String[] args) throws Exception {
        if (args.length < 2) {
            System.out.println("arguments: serverHost serverPort [protocolType]");
            System.exit(-1);
        }

        String serverHost = args[0];
        int serverPort = Integer.parseInt(args[1]);
        String protocolType = "binary";
        if (args.length > 2) {
            protocolType = args[2];
        }

        logger.info("client arguments: {} {}", serverPort, protocolType);

        HelloStruct helloBody = new HelloStruct();

        // Transport
        TSocket socket = new TSocket(serverHost, serverPort);
        socket.setTimeout(2000);
        TTransport transport = socket;

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
            } else if (message.equalsIgnoreCase("hello")) {
                String echoMessage = client.helloEcho(message, helloBody);
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
