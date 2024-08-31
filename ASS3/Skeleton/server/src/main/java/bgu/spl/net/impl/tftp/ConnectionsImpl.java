package bgu.spl.net.impl.tftp;

import bgu.spl.net.srv.BlockingConnectionHandler;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImpl<T> implements Connections<T> {
    //TODO : add fields and others
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    private ConcurrentHashMap<String, Boolean> usersNames;
    private ConcurrentHashMap<Integer, Boolean> usersIds;
    private ConcurrentHashMap<Integer, String> userIdToNames;
    private ConcurrentHashMap<String, FileData> files;
    private String fileToUpload;
    private FilesManager allFiles;
    private AtomicInteger id;
    private static ConnectionsImpl connections = null;


    public static ConnectionsImpl getInstance() {
        if (connections == null)
            connections = new ConnectionsImpl();
        return connections;
    }

    public ConcurrentHashMap<Integer, Boolean> getUsersIds() {
        return usersIds;
    }

    public ConcurrentHashMap<String, Boolean> getUsersNames() {
        return usersNames;
    }

    public ConcurrentHashMap<Integer, ConnectionHandler<T>> getConnectionHandlers() {
        return connectionHandlers;
    }

    public ConnectionsImpl() {
        this.connectionHandlers = new ConcurrentHashMap<>();
        this.usersNames = new ConcurrentHashMap<>();
        this.usersIds = new ConcurrentHashMap<>();
        this.userIdToNames = new ConcurrentHashMap<>();
        this.files = new ConcurrentHashMap<>();
        this.fileToUpload = "";
        this.id = new AtomicInteger(0);
        this.allFiles = new FilesManager("server\\Flies");
    }
    public void connect(int connectionId, ConnectionHandler<T> handler) {
        connectionHandlers.put(connectionId, handler);
    }
    public boolean send(int connectionId, T msg) {
        if (connectionHandlers.containsKey(connectionId)) {

            connectionHandlers.get(connectionId).send(msg);
            return true;
        }
        return false;
    }
    public void disconnect(int connectionId) {
        if (!usersIds.containsKey(connectionId)) {
            byte[] byteToSend = createErrorMessage("User not logged in" , (short) 6) ;
            send(connectionId, (T) byteToSend);
        } else if(usersIds.containsKey(connectionId)) {
            String name = userIdToNames.get(connectionId);
            usersNames.remove(name);
            usersIds.remove(connectionId);
            userIdToNames.remove(connectionId);
            byte[] byteToSend = createAckMessage((short) 0) ;
            send(connectionId, (T) byteToSend);
            connectionHandlers.remove(connectionId);
        }
    }
    public int getIdAdd() {
        return this.id.getAndIncrement();
    }

    public void LOGRQ(int connectionId, String name) {
        if(usersIds.containsKey(connectionId)){
            String errorMessage = "User already logged in";
            byte[] byteToSend = createErrorMessage(errorMessage, (short) 7);
            send(connectionId, (T) byteToSend);
        }
        if (usersNames.containsKey(name)) {
            String errorMessage = "User already logged in";
            byte[] byteToSend = createErrorMessage(errorMessage, (short) 7);
            send(connectionId, (T) byteToSend);
        } else if(!usersIds.containsKey(connectionId) && !userIdToNames.containsKey(connectionId) && !usersNames.containsKey(connectionId)) {
            usersNames.put(name, true);
            usersIds.put(connectionId, true);
            userIdToNames.put(connectionId, name);
            byte[] ackBytes = createAckMessage((short) 0);
            send(connectionId, (T) ackBytes);
        }
    }

    public void DELRQ(int connectionId, String name) {
        if (!usersIds.containsKey(connectionId)) {
            String errorMessage = "User not logged in";
            byte[] byteToSend = createErrorMessage(errorMessage, (short) 6);
            send(connectionId, (T) byteToSend);
        }
        else {
            try{
                if(!allFiles.fileExists(name)){
                    byte[] errorMessage = createErrorMessage("File not found" , (short) 1) ;
                    send(connectionId , (T) errorMessage) ;
                }
                else{
                    allFiles.deleteFile(name);
                    byte[] ackMsg = createAckMessage((short) 0) ;
                    send(connectionId , (T) ackMsg) ;
                    byte[] bcastToSend = createBcastMessage(false , name) ;
                    for (Integer id : usersIds.keySet()){
                        send(id , (T) bcastToSend) ;
                    }
                }
            }catch (IOException e){
            }
        }
    }

    public void WRQ(int connectionId, String fileName) {
        if (!usersIds.containsKey(connectionId)) {
            byte[] byteToSend = createErrorMessage("User not logged in", (short) 6);
            send(connectionId, (T) byteToSend);
        }
        if (allFiles.fileExists(fileName)) {
            byte[] byteToSend = createErrorMessage("File already exists", (short) 5);
            send(connectionId, (T) byteToSend);
        } else {
            try {
                allFiles.createFile(fileName);
                fileToUpload = fileName ;
                byte[] byteToSend = createAckMessage((short) 0);
                send(connectionId, (T) byteToSend);
            } catch (IOException e) {
            }
        }
    }

    public void DIRQ(int connectionId) {  //not sure about this
        if (!usersIds.containsKey(connectionId)) {
            byte[] byteToSend = createErrorMessage("User not logged in", (short) 6);
            send(connectionId, (T) byteToSend);
        } else {
            if (allFiles.isEmpty()) {
                byte[] byteToSend = createErrorMessage("The Folder is Empty", (short) 0);
                send(connectionId, (T) byteToSend);
            }
            ConcurrentHashMap<String, byte[]> filesNames = allFiles.getFiles();
            String allFilesNames = "";
            int count = 0;
            for (String fi : filesNames.keySet()) {
                allFilesNames = allFilesNames + fi + "0";
                count++;
            }
            StringBuilder st = new StringBuilder(allFilesNames) ;
            st.setCharAt(st.length() -1 , '\0');
            allFilesNames = String.valueOf(st);

            byte[] data = allFilesNames.getBytes();
            int packetSize = allFilesNames.length();
            int blockNum = 1;
            int curSize = 0;
            if (packetSize <= 511) {
                send(connectionId, (T) createSmplPacket(packetSize, curSize, blockNum, data));
            } else {
                while (packetSize - curSize > 511) {
                    send(connectionId, (T) createSmplPacket(packetSize, curSize, blockNum, data));
                    blockNum++;
                    curSize += 512;
                }
                if (packetSize - curSize <= 511) {
                    byte[] lastPacket = new byte[6 + packetSize - curSize];
                    lastPacket[0] = 0;
                    lastPacket[1] = 3;
                    lastPacket[2] = (byte) (packetSize - curSize >> 8);
                    lastPacket[3] = (byte) (packetSize - curSize);
                    lastPacket[4] = (byte) (blockNum >> 8);
                    lastPacket[5] = (byte) (blockNum);
                    for (int i = 0; i < packetSize - curSize; i++) {
                        lastPacket[i + 6] = data[i + curSize];
                    }
                    send(connectionId, (T) lastPacket);
                }
            }

        }
    }
    private byte[] createSmplPacket(int packetSize ,int currentSize ,int blockNum ,byte[] data ){
        byte[] arr = new byte[6+packetSize+1];
        arr[0]=0;arr[1]=3;arr[2]=(byte)(packetSize-currentSize >> 8);arr[3] =(byte)(packetSize-currentSize);
        arr[4] = (byte) (blockNum >> 8) ;arr[5] = (byte)  (blockNum); //block num
        for(int i = 0 ; i < packetSize ; i++){
            arr[i+6] = data[i + currentSize];
        }
        arr[6+packetSize] = "\0".getBytes()[0];
        return arr ;
    }
    public void RRQ(int connectionId, String fileName) {
        if (!usersIds.containsKey(connectionId)) {
            byte[] byteToSend = createErrorMessage("User not logged in", (short) 6);
            send(connectionId, (T) byteToSend);
        }
        if (usersIds.containsKey(connectionId) && usersIds.get(connectionId)) {
            if (!allFiles.fileExists(fileName)) {
                byte[] byteToSend = createErrorMessage("File not found", (short) 1);
                send(connectionId, (T) byteToSend);
            } else {
                try{
                    byte[] fileToSend = allFiles.readDataFromFile(fileName) ;
                    int packetSize = fileToSend.length ;
                    if(packetSize <= 511){
                        send(connectionId ,(T) createSmplPacket(packetSize ,0 ,1,fileToSend));
                    }
                    else{
                        int currentSize = 0 ;
                        int blockNum = 1 ;
                        while (packetSize - currentSize > 511){
                            send(connectionId , (T) createSmplPacket(512,currentSize,blockNum,fileToSend)) ;
                            currentSize +=512 ;
                            blockNum ++ ;
                        }
                        if(packetSize - currentSize <=511){
                            byte [] lastPacket = new byte[6+packetSize - currentSize] ;
                            lastPacket[0] = 0 ;lastPacket[1] = 3 ; lastPacket[2] = (byte) (packetSize - currentSize >>8) ; lastPacket[3] = (byte) (packetSize - currentSize) ;
                            lastPacket[4] = (byte) (blockNum >> 8) ; lastPacket[5] = (byte) (blockNum);
                            for (int i = 0 ; i < packetSize - currentSize ; i++){
                                lastPacket[i+6] = fileToSend[i+currentSize] ;
                            }
                            send(connectionId , (T) lastPacket) ;
                        }
                    }

                }
                catch (IOException e){
                }
            }
        }
    }

    public void DATA(int connectionId, byte[] msg, int blockNum, int packetSize) {
        try {
            if (!fileToUpload.equals("")) {
                allFiles.writeData(fileToUpload ,msg);
                byte[] ackToSend = createAckMessage((short) blockNum) ;
                send(connectionId , (T) ackToSend) ;
                if (packetSize < 512) {//this means that this msg is the last block to upload
                    //BCAST TO EVERYONE
                    for (Integer id : usersIds.keySet()) {
                        byte[] castToSend = createBcastMessage(true ,fileToUpload) ;
                        send(id, (T) castToSend);
                    }
                    fileToUpload = "" ;
                }
            }
        } catch (IOException e) {
        }
    }

    public void ACK(int blockNumber) {
        System.out.println("ACK " + blockNumber);
    }

    public static byte[] createErrorMessage(String errorMessage, short errorCode) {
        byte[] errorMessageBytes = errorMessage.getBytes(StandardCharsets.UTF_8);
        byte[] completeErrorMessage = new byte[errorMessageBytes.length + 5];
        completeErrorMessage[0] = 0;
        completeErrorMessage[1] = 5;
        completeErrorMessage[2] = (byte) (errorCode >> 8);
        completeErrorMessage[3] = (byte) errorCode;
        System.arraycopy(errorMessageBytes, 0, completeErrorMessage, 4, errorMessageBytes.length);
        completeErrorMessage[completeErrorMessage.length - 1] = 0;
        return completeErrorMessage;
    }

    public static byte[] createAckMessage(short blockNumber) {
        // Opcode for ACK is 4
        short opcode = 4;
        // Converting opcode and block number to byte array
        byte[] ackBytes = new byte[]{
                (byte) (opcode >> 8), (byte) opcode, // Opcode bytes
                (byte) (blockNumber >> 8), (byte) blockNumber // Block number bytes
        };
        return ackBytes;
    }

    public static byte[] createBcastMessage(boolean added, String filename) {
        short opcode = 9;
        byte deletedAddedByte = added ? (byte) 1 : (byte) 0;
        byte[] filenameBytes = filename.getBytes(StandardCharsets.UTF_8);
        byte[] bcastBytes = new byte[filenameBytes.length + 4];
        bcastBytes[0] = (byte) (opcode >> 8);
        bcastBytes[1] = (byte) opcode;
        bcastBytes[2] = deletedAddedByte;
        System.arraycopy(filenameBytes, 0, bcastBytes, 3, filenameBytes.length);
        bcastBytes[bcastBytes.length - 1] = 0;
        return bcastBytes;
    }
}

