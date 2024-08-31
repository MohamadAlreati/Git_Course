package bgu.spl.net.impl.tftp;

import java.awt.*;
import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

public class TftpClient {
    //TODO: implement the main logic of the client, when using a thread per client the main logic goes here
    private Socket socket ;
    private int port;
    private BufferedReader input;
    private BufferedWriter output;
    private BlockingQueue<String> commands;
    private FilesManager allFiles ;
    private ConnectionHandler handler ;
    private TftpEncoderDecoder encdec ;
    private boolean sentRequest = false ;
    private final Semaphore semaphore = new Semaphore(0);

    public TftpClient(int port , String host) throws IOException{
        this.socket = new Socket(host , port);
        this.port = port ;
        this.input = new BufferedReader(new InputStreamReader(socket.getInputStream() , StandardCharsets.UTF_8)) ;
        this.output = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream() , StandardCharsets.UTF_8)) ;
        this.commands = new LinkedBlockingQueue<>();
        this.allFiles = new FilesManager("client") ;
        this.handler = new ConnectionHandler(host , port , this , this.allFiles) ;
        this.encdec = new TftpEncoderDecoder() ;
    }
    public boolean isClosed() {
        return socket.isClosed();
    }
    public void terminate() throws IOException {
        commands.add("DISC") ;
        socket.close();
        input.close();
        output.close() ;
        Thread.currentThread().interrupt();
        Thread.currentThread().interrupt();
    }
    public class KeyboardThread extends Thread{
        private TftpClient client;
        private ConnectionHandler handler ;
        public KeyboardThread(TftpClient client , ConnectionHandler handler){
            this.client = client ;
            this.handler = handler ;
        }
        public void run(){
            Scanner myScanner = new Scanner(System.in) ;
            while (true) {
                String input = myScanner.nextLine().trim();
                String[] parts = input.split("\\s+", 2);
                String firstPart = parts[0];
                String secondPart = parts.length > 1 ? parts[1] : "";
                handleCommand(firstPart, secondPart, this.handler);
                semaphore.release();
            }
        }
    }
    public class ListeningThread extends Thread{
        private TftpClient client ;
        private ConnectionHandler handler ;
        private BlockingQueue<String> commands ;
        public ListeningThread(TftpClient client , ConnectionHandler handler , BlockingQueue<String> commands){
            this.client = client ;
            this.handler = handler ;
            this.commands = commands ;
        }
        public void run() {
            try {
                BufferedReader reader = new BufferedReader(new InputStreamReader((socket.getInputStream())));
                while(!socket.isClosed()){
                    semaphore.acquire();
                    while (handler.receive() != null) {}
                    if (socket.isClosed()) {
                        break;
                    }
                }
            }catch (IOException |InterruptedException e){}
        }
    }
    public void start(){
        KeyboardThread keyboardThread = new KeyboardThread(this , handler) ;
        ListeningThread listeningThread = new ListeningThread(this , handler ,commands);
        keyboardThread.start();listeningThread.start();
    }
    public byte[] makeRequest(String name , int opcode){
        byte[] requestToSend = new byte[name.length() + 3];
        requestToSend[0] = 0 ; requestToSend[1] = (byte)opcode ;
        byte[] nameToBytes = name.getBytes();
        for(int i = 0 ; i < name.length() ; i++){
            requestToSend[i+2] = nameToBytes[i] ;
        }
        requestToSend[2+ name.length()] = "\0".getBytes()[0] ;
        return requestToSend ;
    }
    public void handleCommand(String firstPart , String secondPart , ConnectionHandler handler){
        if(firstPart.equals("LOGRQ")){
            handler.status.setLogInReq(true);
            String name = secondPart ;
            byte[] request = makeRequest(name,7);
            try{
                this.sentRequest = true ;
                handler.send(request);
                handler.status.setLastCommand(Status.Action.LOGRQ);
            }catch (IOException e){}
        }
        else if(firstPart.equals("DISC")){
            if(handler.status.isLoggedIn()){
                handler.status.setShouldTerminate(true);
                byte[] request = makeRequest("" ,10);
                try{
                    handler.status.setLastCommand(Status.Action.DISC);
                    handler.send(request);
                }catch (IOException e){}
            }
        }
        else if(firstPart.equals("WRQ")){
            String name = secondPart ;
            if(!allFiles.fileExists(name)){
                return;
            }
            byte[] request = makeRequest(name , 2) ;
            try{
                handler.send(request);
                handler.status.setFileName(name);
                handler.status.setLastCommand(Status.Action.WRQ);
                handler.status.setOperationState(Status.OperationState.WRQ_REQUESTED);

            }catch (IOException e){}
        }
        else if(firstPart.equals("RRQ")){
            String name = secondPart ;
            if(allFiles.fileExists(name)){
                System.out.println("File already exists");
                return;
            }
            else try {
                allFiles.createFile(name);
                handler.status.setFileName(name);
                byte[] request = makeRequest(name , 1);
                try{
                    handler.status.setLastCommand(Status.Action.RRQ);
                    handler.status.setOperationState(Status.OperationState.RRQ_REQUESTED);
                    handler.send(request);
                }catch (IOException e){}
            }catch (IOException a){}
        }
        else if( firstPart.equals("DELRQ")){
            String name = secondPart ;
            byte[] request = makeRequest(name , 8) ;
            try{
                handler.status.setLastCommand(Status.Action.DELRQ);
                handler.send(request);
            }catch (IOException e){}
        }
        else if( firstPart.equals("DIRQ")){
            byte[] request = makeRequest("" , 6);
            try{
                handler.status.setLastCommand(Status.Action.DIRQ);
                handler.status.setOperationState(Status.OperationState.DIRQ_REQUESTED);
                handler.send(request);
            }catch (IOException e){}
        }
    }
    public static void main(String[] args) throws IOException {
        String host = "localhost";
        int port = 7777;
        TftpClient client = new TftpClient(port , host);
        try {
            client.start();
        } catch (Exception e) {

        }
    }
}
