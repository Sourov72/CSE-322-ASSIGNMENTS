package com.company;

import ClientInfo.requestInfo;
import ClientInfo.userInfo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.io.FileOutputStream;

public class filereceiveThread implements Runnable{

    private DataInputStream in;
    private DataOutputStream out, tempout;
    ServerSocket welcomeSocket;
    Socket socket;
    String fileName;
    int chunkSize;
    long size;
    long initSize;
    userInfo user;
    FileInfo fileInfo;
    private HashMap<String, userInfo> map;
    private HashMap<String,requestInfo> reqmap;
    private int reqf;
    private String reqid;
    File f;
    FileOutputStream fileOutputStream;
    filereceiveThread(String fileName, int chunkSize, long size, DataOutputStream tempout, userInfo user, FileInfo fileinfo, HashMap<String, userInfo> map, HashMap<String, requestInfo> reqmap, int reqf, String reqid, String rfileName) throws IOException {
        this.fileName = fileName;
        this.chunkSize = chunkSize;
        this.size = size;
        welcomeSocket = new ServerSocket(0);
        this.tempout = tempout;
        this.user = user;
        this.fileInfo = fileinfo;
        this.map = map;
        this.reqmap = reqmap;
        this.reqf = reqf;
        this.reqid = reqid;
        tempout.writeUTF("start " + welcomeSocket.getLocalPort() + " " + chunkSize + " " + rfileName);
        new Thread(this).start();
    }

    @Override
    public void run() {
        try {
            socket = welcomeSocket.accept();
            in = new DataInputStream(socket.getInputStream());
            out = new DataOutputStream(socket.getOutputStream());
            int bytes = 0;
            fileOutputStream = new FileOutputStream(fileName);
            byte[] buffer = new byte[chunkSize];
            int i = 1, flag = 0; //flag for timeout
            long tempSize = 0;
            initSize = size;
            out.writeUTF("true");
            fileInfo.setCurrentSize((int) (fileInfo.getCurrentSize() + size));
            f = new File(fileName);
            String ack = "true";
            while (size > 0 && (bytes = in.read(buffer, 0, (int)Math.min(buffer.length, size))) != -1 && user.isLogin()) {

                //System.out.println("bytes size server: " + bytes);
                fileOutputStream.write(buffer,0,(int)Math.min(bytes, size));

                if(i == 5 && flag == 1) {
                    size -= bytes;
                    System.out.println(in.readUTF());
                    break;

                }
                tempSize = tempSize + (long)bytes;
                size -= bytes;      // read upto file size
                out.writeUTF(ack);
                i++;


            }
            fileOutputStream.close();
            String temp = "";
            if(size == 0)
                temp = in.readUTF();
            if(temp.equalsIgnoreCase("completed")){
                if(initSize == tempSize){
                    fileInfo.setCurrentSize((int) (fileInfo.getCurrentSize() - initSize));
                    tempout.writeUTF("File uploaded successfully");
                    if(reqf == 1){
                        String s = reqmap.get(reqid).getId();
                        map.get(s).storeMsg("your request has been made done where the request was : " + reqmap.get(reqid).getFileName());
                    }
                }

                else{
                    tempout.writeUTF("There is an error uploading the file");
                    fileInfo.setCurrentSize((int) (fileInfo.getCurrentSize() - initSize));
                    if(f.delete())
                        System.out.println("deleted successfully");
                    else
                        System.out.println("deleted unsuccessfully");

                }

            }

            else{
                System.out.println("There is an error uploading the file");
                fileInfo.setCurrentSize((int) (fileInfo.getCurrentSize() - initSize));

                if(f.delete())
                    System.out.println("deleted successfully");
                else
                    System.out.println("deleted unsuccessfully");
            }


        } catch (IOException e) {
            //e.printStackTrace();
            System.out.println("Transmission was not successful");
            fileInfo.setCurrentSize((int) (fileInfo.getCurrentSize() - initSize));
            try {
                
                fileOutputStream.close();
            } catch (IOException ioException) {
                ioException.printStackTrace();
            }

            if(f.delete())
                System.out.println("file deleted successfully1");
            else
                System.out.println("file deleted unsuccessfully1");
        }
        try {
            in.close();
            out.close();
            welcomeSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
