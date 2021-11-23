package com.company;


import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.Socket;

public class downloadfileThread implements Runnable{

    private DataInputStream in;
    private DataOutputStream out;
    private Socket socket;
    private String fileName;
    private int chunkSize;
    private long size;
    private FileOutputStream fileOutputStream;
    downloadfileThread(String fileName, int chunkSize, long size, int id, int port) throws IOException {
        socket = new Socket("localhost", port);
        String[] parts = fileName.split("_");
        this.fileName = "downloadedFiles//" + "d" + id + "_" + parts[parts.length - 1];
        this.chunkSize = chunkSize;
        this.size = size;
        new Thread(this).start();
    }


    @Override
    public void run() {
        try {
            in = new DataInputStream(socket.getInputStream());
            out = new DataOutputStream(socket.getOutputStream());
            int bytes = 0;
            fileOutputStream = new FileOutputStream(fileName);
            byte[] buffer = new byte[chunkSize];
            long tempSize = 0;
            long initSize = size;
            while (size > 0 && (bytes = in.read(buffer, 0, (int)Math.min(buffer.length, size))) != -1) {
                fileOutputStream.write(buffer,0,(int)Math.min(bytes, size));
                tempSize = tempSize + (long)bytes;
                size -= bytes;      // read upto file size


            }
            fileOutputStream.close();
            //System.out.println("out of the loop");

            if(size == 0){

                if(initSize == tempSize)
                    out.writeUTF("true");
                else{
                    out.writeUTF("false");
                }
                System.out.println(in.readUTF());
            }

            in.close();
            out.close();
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
