package com.company;

public class FileInfo {
    private long maxbufferSize = 2000000000L;
    private int maxchunkSize = 30000;
    private int minchunkSize = 10000;
    private long currentSize = 0L;
    private int fileid = 1;
    private int reqid = 1;
    private int fileNos = 0;
    private String pathname = "";

    public int getFileNos() {
        return fileNos;
    }

    public void setFileNos(int fileNos) {
        this.fileNos = fileNos;
    }

    public long getMaxbufferSize() {
        return maxbufferSize;
    }

    public void setMaxbufferSize(long maxbufferSize) {
        this.maxbufferSize = maxbufferSize;
    }

    public int getMaxchunkSize() {
        return maxchunkSize;
    }

    public void setMaxchunkSize(int maxchunkSize) {
        this.maxchunkSize = maxchunkSize;
    }

    public int getMinchunkSize() {
        return minchunkSize;
    }

    public void setMinchunkSize(int minchunkSize) {
        this.minchunkSize = minchunkSize;
    }

    public long getCurrentSize() {
        return currentSize;
    }

    public void setCurrentSize(int currentSize) {
        this.currentSize = currentSize;
    }

    public int getFileid() {
        return fileid;
    }

    public void setFileid(int fileid) {
        this.fileid = fileid;
    }

    public int getReqid() {
        return reqid;
    }

    public void setReqid(int reqid) {
        this.reqid = reqid;
    }

    public void setpathName(String path){
        this.pathname = path;
    }

    public String getPath(){
        return pathname;
    }
}
