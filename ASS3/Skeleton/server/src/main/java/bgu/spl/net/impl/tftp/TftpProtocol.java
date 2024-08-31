package bgu.spl.net.impl.tftp;

import bgu.spl.net.api.BidiMessagingProtocol;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import java.io.FileInputStream ;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class TftpProtocol implements BidiMessagingProtocol<byte[]>  {

    private boolean shouldTerminate ;
    private Connections<byte[]> connections ;
    private int connectionId ;


    @Override
    public void start(int connectionId, ConnectionsImpl<byte[]> connections) {
        // TODO implement this
        this.shouldTerminate = false ;
        this.connectionId = connectionId ;
        this.connections = connections ;

    }
    private short bytesToShort(byte[] bytes) {
        return (short) (((short) bytes[0] << 8) | ((short) bytes[1] & 0xff));
    }
    @Override
    public void process(byte[] message) {
        // TODO implement this
        int opCode = TftpEncoderDecoder.getOpcode();
        switch (opCode) {
            case 1:

                ConnectionsImpl.getInstance().RRQ(connectionId , getName(message));
                break;
            case 2:

                ConnectionsImpl.getInstance().WRQ(connectionId , getName(message));
                break;
            case 3:
                ConnectionsImpl.getInstance().DATA(connectionId , message,TftpEncoderDecoder.getBlocknum() , TftpEncoderDecoder.getPacketsize());
                break;
            case 4:
                ConnectionsImpl.getInstance().ACK(TftpEncoderDecoder.getBlocknum()) ;
                break;
            case 5:
                break;
            case 6:
                ConnectionsImpl.getInstance().DIRQ(connectionId);
                break;
            case 7: {
                ConnectionsImpl.getInstance().LOGRQ(connectionId, getName(message));
            }
                break;
            case 8:
                ConnectionsImpl.getInstance().DELRQ(connectionId , getName(message));
                break;
            case 9:
                break;
            case 10:
                ConnectionsImpl.getInstance().disconnect(connectionId);
                break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    public static String getName(byte[] message){
        int nameLength = 0 ;
        for(int i  = 0 ; i < message.length ; i++){
            if(message[i] == 0){
                nameLength = i ;
                break;
            }
            if(i == message.length - 1){
                nameLength = message.length ;
            }
        }

        String res =new String(message,0 , nameLength , StandardCharsets.UTF_8) ;
        return res ;
    }
    @Override
    public boolean shouldTerminate() {
        // TODO implement this
        return shouldTerminate ;
    }


    
}
