package com.company;


import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) throws IOException, ClassNotFoundException {
        Socket socket = new Socket("localhost", 6666);
        System.out.println("Connection established");
        System.out.println("Remote port: " + socket.getPort());
        System.out.println("Local port: " + socket.getLocalPort());

        
        // buffers
        DataOutputStream out = new DataOutputStream (socket.getOutputStream());
        DataInputStream in = new DataInputStream (socket.getInputStream());

        //
        while(true){
            Object received = null;
            received = in.readUTF();
            System.out.println("Message Received: " + (String)received);
            Scanner input = new Scanner(System.in);
            String msg = input.nextLine();
            out.writeUTF(msg);
            received = in.readUTF();
            System.out.println("Message Received: " + (String)received);
            msg = (String)received;
            if(msg.equalsIgnoreCase("Successful")){
                System.out.println("Logged in Successfully");

                new writeThread(socket,out);
                new readThread(socket,in, out);

                System.out.println("press 1:List of all the students connected to the server" + "\n" + "press 2:List of all the files uploaded both private and public" + "\n" +   "press 3:Lookup all the public files of a specific student"   + "\n" + "press 4:Request for a file"  + "\n" + "press 5:show all the unread messages"  + "\n" +  "press 6:upload a file"   + "\n" + "press 7:upload a file in response to a request" + "\n" + "press 8:Logout" + "\n");

                break;
            }
            else{
                input.close();
                in.close();
                out.close();
                socket.close();
                break;
            }

        }


    }
}