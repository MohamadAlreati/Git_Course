package bgu.spl.net.impl.tftp;

public class Status {
    private boolean shouldTerminate;
    private boolean loggedIn;
    private boolean logInReq ;
    private OperationState operationState;
    private Action lastCommand;
    private String fileName;
    private byte[] fileData;
    private int dataIndex;

    public Status() {
        this.shouldTerminate = false;
        this.loggedIn = false;
        this.logInReq =false;
        this.operationState = OperationState.IDLE;
        this.lastCommand = Action.IDLE ;
        this.fileName = "";
        this.fileData = new byte[512];
        this.dataIndex = 0;
    }

    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    public boolean isLogInReq() {
        return logInReq;
    }

    public void setLogInReq(boolean logInReq) {
        this.logInReq = logInReq;
    }

    public void setShouldTerminate(boolean shouldTerminate) {
        this.shouldTerminate = shouldTerminate;
    }

    public boolean isLoggedIn() {
        return loggedIn;
    }

    public void setLoggedIn(boolean loggedIn) {
        this.loggedIn = loggedIn;
    }

    public OperationState getOperationState() {
        return operationState;
    }

    public void setOperationState(OperationState operationState) {
        this.operationState = operationState;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public byte[] getFileData() {
        return fileData;
    }

    public void setFileData(byte[] fileData) {
        this.fileData = fileData;
    }

    public int getDataIndex() {
        return dataIndex;
    }

    public void setDataIndex(int dataIndex) {
        this.dataIndex = dataIndex;
    }

    public Action getLastCommand(){return lastCommand ;}
    public void setLastCommand(Action command){this.lastCommand = command;}
    public enum OperationState {
        IDLE,
        WRQ_REQUESTED,
        WRQ_STARTED,
        WRQ_FINISHED,
        RRQ_REQUESTED,
        RRQ_STARTED,
        RRQ_FINISHED,
        DIRQ_REQUESTED,
        DIRQ_STARTED,
        DIRQ_FINISHED;
    }
    public enum Action{
        IDLE ,
        WRQ ,
        LOGRQ ,
        DISC ,
        RRQ ,
        DELRQ,
        DIRQ,

    }
}
