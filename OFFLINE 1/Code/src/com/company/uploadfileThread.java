package com.company;

import java.io.*;
import java.net.Socket;
import java.util.Timer;
import java.util.TimerTask;

public class uploadfileThread implements Runnable{

    private DataInputStream in;
    private DataOutputStream out;
    Socket socket;
    File file;
    int chunkSize;

    uploadfileThread(File file, int chunkSize, int port) throws IOException {
        socket = new Socket("localhost", port);
        in = new DataInputStream(socket.getInputStream());
        out = new DataOutputStream(socket.getOutputStream());
        this.file = file;
        this.chunkSize = chunkSize;
        new Thread(this).start();

    }

    @Override
    public void run() {

        try{

            sendFile();
            in.close();
            out.close();
            socket.close();

        }catch (IOException e) {
            //e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
    private  void sendFile() throws Exception{
        int bytes = 0;

        byte[] buffer = new byte[(int) chunkSize];
        FileInputStream fileInputStream = new FileInputStream(file);
        String ack;
        ack = in.readUTF();
        while ((bytes=fileInputStream.read(buffer))!=-1){
            if(ack.equalsIgnoreCase("true")){
                //System.out.println("bytes size client: " + bytes);
                out.write(buffer,0,bytes);
                out.flush();
                ack = "";
                Task t1 = new Task(fileInputStream, in, out, socket);
                Timer t = new Timer(true);
                t.schedule(t1, 10000, 10000); //  executes for every 10 seconds
                ack = in.readUTF();
                t1.cancel();
                t.cancel();
            }
            else
                break;

        }
        out.writeUTF("completed");
        fileInputStream.close();
    }
}


class Task extends TimerTask {

    DataInputStream in;
    DataOutputStream out;
    Socket socket;
    FileInputStream fi;
    public Task(FileInputStream fi, DataInputStream in, DataOutputStream out, Socket socket) {
        this.fi = fi;
        this.out = out;
        this.in = in;
        this.socket = socket;
    }
    public void run() {
        //System.out.println("[" + new Date() + "] " + "10 seconds passed" + ": File uploading is unsuccessful");
        try {
            out.writeUTF("Time out, Transmission in cancelled");
            fi.close();
            out.close();
            in.close();
            socket.close();
            this.cancel();
        } catch (IOException e) {

            e.printStackTrace();

        }

    }
}