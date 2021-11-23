package ClientInfo;

import java.util.LinkedList;
import java.util.Queue;

public class userInfo {
    private String id;
    private boolean login = false;
    private Queue<String> q = new LinkedList<>();

    public userInfo(String id){
        this.id = id;
        this.login = true;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public boolean isLogin() {
        return login;
    }

    public void setLogin(boolean login) {
        this.login = login;
    }
    public void storeMsg(String s){
        q.add(s);
    }
    public int getQueueSize(){
        return q.size();
    }
    public String showmsg(){

        return q.remove();

    }
    
}
