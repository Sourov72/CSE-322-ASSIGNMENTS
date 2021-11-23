package com.company;


import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class writeThread implements Runnable{
    private DataOutputStream  out;
    Socket socket;
    writeThread(Socket socket, DataOutputStream out){
        this.socket = socket;
        this.out = out;


        new Thread(this).start();
    }

    @Override
    public void run() {
        Scanner input = new Scanner(System.in);
        while (true){
            String msg = input.nextLine();

            try {
                System.out.println(msg + " successfully send");
                out.writeUTF(msg);
                if(msg.equals("6")){


                    String filename = input.nextLine();
                    out.writeUTF(filename);
                    msg = input.nextLine();
                    out.writeUTF(msg);
                    File file = new File(filename);
                    //System.out.println("file length " + file.length());
                    if(file.exists())
                        out.writeLong(file.length());
                    else
                        out.writeLong(0);

                }
                else if(msg.equals("3")){
                    msg = input.nextLine();
                    out.writeUTF(msg);
                }

                else if(msg.equals("7")){


                    String reqid = input.nextLine();
                    out.writeUTF(reqid);
                    String filename = input.nextLine();
                    out.writeUTF(filename);

                    out.writeUTF("public");
                    File file = new File(filename);
                    if(file.exists()){
                        //System.out.println("file length " + file.length());
                        out.writeLong(file.length());
                    }
                    else{
                        out.writeLong(0);
                    }
                    

                }

                else if(msg.equals("4")){
                    String name = input.nextLine();
                    out.writeUTF(name);
                }

                else if(msg.equals("8")){
                    break;
                }

                System.out.println("press 1:List of all the students connected to the server" + "\n" + "press 2:List of all the files uploaded both private and public" + "\n" +   "press 3:Lookup all the public files of a specific student"   + "\n" + "press 4:Request for a file"  + "\n" + "press 5:show all the unread messages"  + "\n" +  "press 6:upload a file"   + "\n" + "press 7:upload a file in response to a request" + "\n" + "press 8:Logout" + "\n");


            } catch (IOException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
        input.close();
        

    }


}


