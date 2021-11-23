package com.company;

import ClientInfo.requestInfo;
import ClientInfo.userInfo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;


public class ClientHandler implements Runnable{

    private Socket client;
    private DataInputStream  in;
    private DataOutputStream  out;
    FileInfo fileInfo;
    private HashMap<String, userInfo> map;
    private HashMap<String, requestInfo> reqmap;
    userInfo user;
    requestInfo req;
    String root = "allstudentFiles//";


    ClientHandler(Socket client, HashMap<String, userInfo> map, HashMap<String, requestInfo> reqmap, FileInfo fileInfo) throws IOException {
        this.client = client;
        in = new DataInputStream (client.getInputStream());
        out =  new DataOutputStream (client.getOutputStream());
        //this.clients = clients;
        this.map = map;
        this.reqmap = reqmap;
        this.fileInfo = fileInfo;

    }


    @Override
    public void run() {
        Object received = null;
        String id = null;
        String msg = null;

        try {
            out.writeUTF("please give your login id: ");
            received = in.readUTF();

            id = (String) received;
            System.out.println("server received " + id);

            if(map.containsKey(received) && map.get(received).isLogin()){

                //System.out.println("Sorry you are already logged in");
                out.writeUTF("please login with another id, you are already logged in");

            }
            else{

                if(!map.containsKey((String)received)){
                    user = new userInfo(id);
                    map.put(id, user);
                    new File(root+id).mkdir();
                    new File(root+id+"//public").mkdir();
                    new File(root+id+"//private").mkdir();
                }
                else{
                    user = map.get(id);
                }


                try {
                    out.writeUTF("Successful");
                    user.setLogin(true);
                    //System.out.println(map);
                    while(true){
                        received = in.readUTF();
                        msg = (String) received;
                        if(msg.equals("1")){
                            out.writeUTF("The Connected Students are:");
                            for (Map.Entry<String, userInfo> entry : map.entrySet()) {
                                out.writeUTF(entry.getKey() + (entry.getValue().isLogin()?("(online)"):"(offline)"));
                            }
                        }

                        else if(msg.equals("2")){
                            File[] files = new File(root + user.getId()).listFiles();
                            if(files != null){
                                printFiles(files);
                                if(fileInfo.getFileNos() != 0){
                                    out.writeUTF("Do you want to download any of these files?");
                                    msg = in.readUTF();
                                    if(msg.equalsIgnoreCase("yes")){
                                        out.writeUTF("please specify the file name: ");
                                        String reqfileName = in.readUTF();
                                        String reqfilePath = "";
                                        filepathReturn(files, reqfileName, reqfilePath);
                                        System.out.println(reqfileName);
                                        reqfilePath = fileInfo.getPath();
                                        fileInfo.setpathName("");
                                        System.out.println("path: " + reqfilePath);
                                        File file = new File(reqfilePath);
                                        if(file.exists()){
                                            System.out.println("when sending size is: " + file.length());
                                            new filesendThread(file, fileInfo.getMaxchunkSize(), user, out, reqfilePath, reqfileName);
                                        }
                                        else
                                            out.writeUTF("There is no file with this name");
                                        
                                    }
                                }
                                fileInfo.setFileNos(0);

                            }

                            else
                                out.writeUTF("No files to show");

                        }

                        else if(msg.equals("3")){
                            out.writeUTF("The id of the student whose files you want to see: ");
                            String specificPath = root + "//" + in.readUTF() + "//" + "public";
                            File[] files = new File(specificPath).listFiles();
                            if(files != null){
                                printFiles(files);
                                if(fileInfo.getFileNos() != 0){
                                    out.writeUTF("Do you want to download any of these files?");
                                    msg =  in.readUTF();
                                    if(msg.equalsIgnoreCase("yes")){
                                        out.writeUTF("please specify the file name: ");
                                        String fname = in.readUTF();
                                        String reqfilePath = specificPath + "//" + fname;
                                        File file = new File(reqfilePath);
                                        System.out.println(fname + "file name in 4");
                                        System.out.println(reqfilePath + "in 3");
                                        if(file.exists()){
                                            System.out.println("when sending size is: " + file.length());
                                            new filesendThread(file, fileInfo.getMaxchunkSize(), user, out, reqfilePath, fname);
                                        }
                                        else
                                            out.writeUTF("There is no file with this name");
                                       
                                    }
                                }
                                fileInfo.setFileNos(0);

                            }

                            else
                                out.writeUTF("NO files for this student is available now");
                        }

                        else if(msg.equals("4")){

                            out.writeUTF("please type the name of requested file: ");
                            received = in.readUTF();
                            msg = (String) received;
                            req = new requestInfo(Integer.toString(fileInfo.getReqid()), msg, user.getId());
                            reqmap.put(Integer.toString(fileInfo.getReqid()), req);
                            for (Map.Entry<String, userInfo> entry : map.entrySet()) {
                                if(!entry.getKey().equals(user.getId())){
                                    entry.getValue().storeMsg("Requested file Id: " + fileInfo.getReqid() + " and Request was: " + msg);
                                }
                            }
                            fileInfo.setReqid(fileInfo.getReqid() + 1);


                        }

                        else if(msg.equals("5")){
                            out.writeUTF("your unread messages are: ");
                            int s = user.getQueueSize();
                            //System.out.println("queue size: " + s);
                            for(int i = 0; i < s; i++){
                                out.writeUTF(user.showmsg());
                            }
                        }

                        else if(msg.equals("6")){

                            fileinputtake("0",0);

                        }

                        else if(msg.equals("7")){
                            out.writeUTF("Please provide the request id: ");
                            String reqid = in.readUTF();
                            if(reqmap.containsKey(reqid))
                                fileinputtake(reqid, 1);
                            else
                                out.writeUTF("No request has been made with this id");
                        }

                        else if(msg.equals("8")){
                            user.setLogin(false);
                            out.writeUTF("Logged out successfully");
                            out.writeUTF("out");
                            break;
                        }

                    }

                } catch (IOException e) {
                    //e.printStackTrace();
                    try {
                        if(user != null)
                            user.setLogin(false);
                        in.close();
                        out.close();
                        client.close();
                        //System.out.println("client has disconnected inner");
                    } catch (IOException ioException) {
                        ioException.printStackTrace();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

                }


        } catch (IOException e) {

            try {
                if(user != null)
                    user.setLogin(false);
                in.close();
                out.close();
                client.close();
                //e.printStackTrace();
                //System.out.println("Client has disconnected root1");

            } catch (IOException ioException) {
                ioException.printStackTrace();
            }

        }

        if(user != null)
            user.setLogin(false);
        try {
            in.close();
            out.close();
            client.close();
            //e.printStackTrace();
            //System.out.println("Client has disconnected root2");
        } catch (IOException e) {
            e.printStackTrace();
        }




    }


    private void fileinputtake(String reqid,int reqf) throws IOException {

        out.writeUTF("file name: ");
        String rfileName = in.readUTF();
        if(reqf != 1)
            out.writeUTF("file type? private or public: ");
        String type = in.readUTF();
        long size = in.readLong();


        if(size != 0 && size + fileInfo.getCurrentSize() <= fileInfo.getMaxbufferSize()){
            int chunkSize = (int) (Math.random() * (fileInfo.getMaxchunkSize() - fileInfo.getMinchunkSize())) + fileInfo.getMinchunkSize();
            String fileName = root + user.getId() + "//" + type +"//" + fileInfo.getFileid() + "_" + rfileName;
            fileInfo.setFileid(fileInfo.getFileid() + 1);
            new filereceiveThread(fileName, chunkSize, size, out, user, fileInfo, map, reqmap, reqf, reqid, rfileName);

        }
        else{
            out.writeUTF("You can't transmit the file or the file doesn't exist");
        }

    }


    private void printFiles(File[] files) throws IOException {
        for(File file: files){
            if(file.isDirectory()){
                out.writeUTF("Type of these files: " + file.getName());
                printFiles(file.listFiles());
            }
            else{
                out.writeUTF("File is: " + file.getName());
                fileInfo.setFileNos(fileInfo.getFileNos() + 1);
            }
        }
    }

    private void filepathReturn(File[] files, String fname, String pathname) throws IOException {
        for(File file: files){
            if(file.isDirectory()){
                filepathReturn(file.listFiles(), fname, pathname);
            }
            else{
               
                if(file.getName().equals(fname)){
                    fileInfo.setpathName(file.getPath());
                    //System.out.println(fname + " in func");
                    return;
                }
            }
        }
        
    }
}
