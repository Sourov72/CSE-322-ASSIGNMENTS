package ClientInfo;

public class requestInfo {
    private String reqId;
    private String fileName;
    private String id;
    public requestInfo(String reqId, String fileName, String id){
        this.reqId = reqId;
        this.fileName = fileName;
        this.id = id;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getReqId() {
        return reqId;
    }

    public void setReqId(String reqId) {
        this.reqId = reqId;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }


}
