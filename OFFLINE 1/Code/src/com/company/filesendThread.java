package com.company;

import ClientInfo.userInfo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class filesendThread implements Runnable{
    private DataInputStream in;
    private DataOutputStream out;
    private ServerSocket welcomeSocket;
    private Socket socket;
    private int chunkSize;
    private File file;
    userInfo user;

    filesendThread(File file, int chunkSize, userInfo user, DataOutputStream tempout, String path, String fname) throws IOException {

        this.chunkSize = chunkSize;
        this.file = file;
        welcomeSocket = new ServerSocket(0);
        this.user = user;
        tempout.writeUTF("download " + chunkSize + " " + file.length() + " " + fname + " " + user.getId() + " " + welcomeSocket.getLocalPort());
        new Thread(this).start();

    }

    @Override
    public void run() {

        try {
            socket = welcomeSocket.accept();
            in = new DataInputStream(socket.getInputStream());
            out = new DataOutputStream(socket.getOutputStream());
            sendFile();
            in.close();
            out.close();
            welcomeSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println("Client has disconnected");
        } catch (Exception e) {
            e.printStackTrace();
        }


    }

    private  void sendFile() throws Exception{
        int bytes = 0;

        byte[] buffer = new byte[(int) chunkSize];
        System.out.println("buffer length in client: " + buffer.length);
        FileInputStream fileInputStream = new FileInputStream(file);
        // send file size
        // break file into chunks
        while ((bytes=fileInputStream.read(buffer))!=-1 && user.isLogin()){
            
            out.write(buffer,0,bytes);
            out.flush();

        }

        if(bytes == -1){

            if(in.readUTF().equalsIgnoreCase("true")){
                out.writeUTF("file downloading is successful");
            }
            else
                out.writeUTF("file downloading is unsuccessful");
        }
        else
            System.out.println("Clients has logged out");

        fileInputStream.close();
    }
}
