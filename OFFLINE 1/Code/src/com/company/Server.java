package com.company;

import ClientInfo.requestInfo;
import ClientInfo.userInfo;

import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;

public class Server {
    public static void main(String[] args) throws IOException, ClassNotFoundException {
        ServerSocket welcomeSocket = new ServerSocket(6666);
        HashMap<String, userInfo> map = new HashMap<String, userInfo>();
        HashMap<String, requestInfo> reqmap = new HashMap<String, requestInfo>();
        FileInfo fileinfo = new FileInfo();
        new File("allstudentFiles").mkdir();
        new File("downloadedFiles").mkdir();

        while(true) {
            System.out.println("Waiting for connection...");
            Socket socket = welcomeSocket.accept();
            System.out.println("Connection established");
            ClientHandler clientThread = new ClientHandler(socket,map, reqmap, fileinfo);
            // open thread
            Thread worker = new Thread(clientThread);
            worker.start();


        }
        

    }
}
