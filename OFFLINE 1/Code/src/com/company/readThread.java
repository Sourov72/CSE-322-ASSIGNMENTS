package com.company;


import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.Socket;

public class readThread implements Runnable{
    Socket socket;
    private DataInputStream in;
    private DataOutputStream out;

    readThread(Socket socket, DataInputStream  in, DataOutputStream out){
        this.socket = socket;
        this.in = in;
        this.out = out;

        new Thread(this).start();
    }
    @Override
    public void run() {
        while(true){
            Object received = null;
            try {
                received = in.readUTF();
                System.out.println("Message Received: " + (String)received);
                String string = (String)received;
                String[] parts = string.split(" ");
                if(parts[0].equalsIgnoreCase("start") && parts.length == 4){

                    File file = new File(parts[3]);
                    new uploadfileThread(file, (int)Long.parseLong(parts[2]), Integer.parseInt(parts[1]));
                }
                else if(parts[0].equalsIgnoreCase("download")){

                    new downloadfileThread(parts[3], Integer.parseInt(parts[1]), Long.parseLong(parts[2]),Integer.parseInt(parts[4]), Integer.parseInt(parts[5]));

                }
                else if(parts[0].equalsIgnoreCase("out")){
                    out.close();
                    in.close();
                    socket.close();
                }

            } catch (IOException e) {
                try {
                    in.close();
                    out.close();
                    socket.close();
                    break;
                } catch (IOException ioException) {
                    ioException.printStackTrace();
                }
            }

        }
    }
}
