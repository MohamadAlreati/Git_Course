package bgu.spl.net.impl.tftp;

import javax.swing.plaf.synth.SynthOptionPaneUI;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class ConnectionHandler {
    private String host ;
    private int port ;
    private Socket socket ;
    private TftpEncoderDecoder endec ;
    public Status status ;
    private FilesManager allFiles ;
    private TftpClient client ;
    private int blockNumHelper =0;
    private int indexHelper = 0;

    public ConnectionHandler(String host , int port , TftpClient client , FilesManager allFiles) throws IOException {
        this.host = host;
        this.port = port;
        this.socket = new Socket(host, port);
        this.status = new Status() ;
        this.allFiles = allFiles ;
        this.endec = new TftpEncoderDecoder();
        this.client = client ;
    }
    public void send(byte[] bytes) throws IOException {
        System.out.println("sending request");
        OutputStream outputStream = this.socket.getOutputStream();
        byte[] encodedBytes = this.endec.encode(bytes);
        outputStream.write(encodedBytes);
        outputStream.flush();
    }
    public void disconnect() throws IOException {
        try {
            this.socket.close();
        } catch (IOException e) {
        }
    }
    public byte[] receive() throws IOException{
        if(client.isClosed()){
            return null ;
        }
        InputStream inputStream = new BufferedInputStream(this.socket.getInputStream());
        int readMe ;
        while( (readMe= inputStream.read()) >= 0){
            byte[] nextMsg = this.endec.decodeNextByte((byte) readMe) ;
            if (nextMsg == null) {
            }
            if(nextMsg !=null){
                process(nextMsg);
                endec.reset() ;
            }
        }
        return null ;
    }

    public void process(byte[] msg ){
        short opCode = endec.getOpcode() ;
        int errCode = endec. getErrCode();
        int blockNum = endec.getBlocknum();
        int packetSize = endec.getPacketsize();
        switch (opCode){
            case 3:
                DATA(msg , blockNum , packetSize);
                break;
            case 4:
                ACK(blockNum) ;
                break;
            case 5:
                ERROR(new String(msg) , errCode);
                break;
            case 9:
                BCAST(msg , blockNum);
                break;
        }
    }
    public void ACK(int blockNum){
        if(status.getLastCommand().equals(Status.Action.DISC)){
            if(blockNum == 0){
                status.setShouldTerminate(true);
                System.out.println("ACK 0 : DISCONNECTING");
                try{
                    client.terminate();
                    disconnect();
                }catch (IOException e){}
            }
        }
        else if(status.getLastCommand().equals(Status.Action.LOGRQ)){
            if(blockNum == 0){
                status.setLoggedIn(true);
                status.setLogInReq(false);
                System.out.println("ACK 0 : LOGGING IN");
            }
        }
        else if(status.getLastCommand().equals(Status.Action.DELRQ)){
            if(blockNum == 0){
                System.out.println("ACK 0 : FILE DELETED");
            }
        }
        else if(status.getLastCommand().equals(Status.Action.WRQ)){
            if(blockNum == 0){
                System.out.println("ACK 0 : WRQ");
                handleWRQ();
            }else{
                System.out.println("ACK " + blockNum +" , UPLOADING");
            }
        }
    }
public void handleWRQ() {
    while (status.getOperationState().equals(Status.OperationState.WRQ_REQUESTED) || !status.getOperationState().equals(Status.OperationState.WRQ_FINISHED)|| status.getOperationState().equals(Status.OperationState.WRQ_STARTED)) {
        if ((status.getOperationState().equals(Status.OperationState.WRQ_REQUESTED) || !status.getOperationState().equals(Status.OperationState.WRQ_FINISHED)|| !status.getOperationState().equals(Status.OperationState.WRQ_STARTED))) {
            try {
                byte[] data = allFiles.readDataFromFile(status.getFileName());

                int blockSize = Math.min(512, data.length - indexHelper);
                byte[] toSend = Arrays.copyOfRange(data, indexHelper, indexHelper + blockSize);
                sendPacket(toSend);
                indexHelper += blockSize;
                if (indexHelper >= data.length) {
                    status.setOperationState(Status.OperationState.WRQ_FINISHED);
                    return;
                }
            } catch (IOException e) {}
        }
    }
}

    private void sendPacket(byte[] dataToSend) throws IOException {
        int size = dataToSend.length;
        byte[] array = new byte[2 + 2 + 2 + size];
        array[0] = 0;
        array[1] = 3;
        array[2] = (byte) (size >> 8);
        array[3] = (byte) size;
        array[4] = (byte) (blockNumHelper >> 8);
        array[5] = (byte) blockNumHelper;
        System.arraycopy(dataToSend, 0, array, 6, size);
        send(array);
        blockNumHelper++;
    }
    public void ERROR(String msg , int errCode){
        System.out.println("ERROR: " +errCode +" , "+msg );
    }
    public void BCAST(byte[] data , int blockNum){
        String name = getName(data) ;
        if(blockNum == 1){
            System.out.println("BCAST, added: " + name);
        }
        else if(blockNum == 0){
            System.out.println("BCAST, deleted: " + name);
        }
    }
    public void sendACK(int blockNum){
        try {
            byte[] array = new byte[4];
            array[0] = 0;
            array[1] = 4;
            array[2] = (byte) (blockNum >> 8);
            array[3] = (byte) blockNum;
            send(array);
        }catch (IOException e){}
    }
    public void DATA(byte[] data , int blockNum , int packetSize){
        if(status.getLastCommand().equals(Status.Action.DIRQ)){
            handleDIRQ(data, blockNum, packetSize);
        }
        else if (status.getLastCommand().equals(Status.Action.RRQ)){
            handleRRQ(data, blockNum, packetSize);
        }

    }
    public void handleRRQ(byte[] data , int blockNum , int packetSize){
        String fileToHandle = status.getFileName() ;
        if(packetSize == 512){
            if(status.getOperationState().equals(Status.OperationState.RRQ_REQUESTED)){
                status.setOperationState(Status.OperationState.RRQ_STARTED);
                    try {
                        allFiles.writeData(fileToHandle , data);
                        sendACK(1);
                    }catch (IOException e){}
            }
            else if(blockNum !=1 && status.getOperationState().equals(Status.OperationState.RRQ_STARTED)){
                try{
                    allFiles.writeData(fileToHandle , data);
                    sendACK(blockNum);
                }catch (IOException e){}
            }
        }
        else if(packetSize < 512){
            if(status.getOperationState().equals(Status.OperationState.RRQ_REQUESTED) || status.getOperationState().equals(Status.OperationState.RRQ_STARTED)){
                status.setOperationState(Status.OperationState.RRQ_FINISHED);
                try{
                    allFiles.writeData(fileToHandle , data);
                    sendACK(blockNum);
                }catch (IOException e){}
            }
        }
    }
    public void downloadNames(byte[] msg){
        byte[] data = status.getFileData();
        byte[] newData = new byte[data.length + 512] ;
        for (int i = 0; i < data.length; i++) {
            newData[i] = data[i];
        }
        for (int i = 0; i < 512; i++) {
            newData[i + data.length] = msg[i];
        }
        status.setFileData(newData);
    }
    public void handleDIRQ(byte[] msg , int blockNum , int packetSize){
            if(packetSize == 512){
                if(blockNum == 1 && status.getOperationState().equals(Status.OperationState.DIRQ_REQUESTED)){
                    status.setOperationState(Status.OperationState.DIRQ_STARTED);
//                    downloadNames(data);
                    byte[] data = status.getFileData();
                    byte[] newData = new byte[data.length + 512];
                    for (int i = 0; i < msg.length; i++) {
                        newData[i] = data[i];
                    }
                    for (int i = 0; i < 512; i++) {
                        newData[i + msg.length] = msg[i];
                    }
                    status.setFileData(newData);
                    sendACK(blockNum);
                }else if (blockNum !=1 && status.getOperationState().equals(Status.OperationState.DIRQ_STARTED)){
                    byte[] data = status.getFileData();
                    byte[] newData = new byte[msg.length + 512];
                    for (int i = 0; i < msg.length; i++) {
                        newData[i] = data[i];
                    }
                    for (int i = 0; i < 512; i++) {
                        newData[i + data.length] = msg[i];
                    }
                    sendACK(blockNum);
                }
            }else if(packetSize < 512 &&(status.getOperationState().equals(Status.OperationState.DIRQ_REQUESTED) || status.getOperationState().equals(Status.OperationState.DIRQ_STARTED))){
                status.setOperationState(Status.OperationState.DIRQ_FINISHED);
//                downloadNames(data);
                byte[] data = status.getFileData();
                byte[] newData = new byte[data.length + packetSize];
                for (int i = 0; i < msg.length; i++) {
                    newData[i] = data[i];
                }
                for (int i = 0; i < packetSize; i++) {
                    newData[i + data.length] = msg[i];
                }
                status.setFileData(new byte[512]);
                String st = new String(newData ,StandardCharsets.UTF_8 ) ;
                String[] filesNames = st.split("0") ;
                for(String f : filesNames){
                    System.out.print(f +"\n");
                }
                sendACK(blockNum);
            }
    }
    protected static String getName(byte[] data) {
        int filenameLength = 0;
        for (int i = 0; i < data.length; i++) {
            if (data[i] == 0) {
                filenameLength = i;
                break;
            }
            if (i == data.length - 1) {
                filenameLength = data.length;
            }
        }
        return new String(data, 0, filenameLength, StandardCharsets.UTF_8);
    }

}
