package bgu.spl.net.impl.tftp;

import java.util.Arrays;

public class FileData {
    private String filename ;

    private byte[] data ;

    private int numOfBlocks ;

    public FileData(String filename , byte[] data){
        this.filename = filename ;
        this.data = data ;
        this.numOfBlocks = data.length / 512 ;
    }
    public byte[] getPartialData(int blockNumber) {
        int startIdx = (blockNumber - 1) * 512;
        int endIdx = Math.min(blockNumber * 512, data.length);

        if (startIdx < data.length) {
            return Arrays.copyOfRange(data, startIdx, endIdx);
        } else {
            return new byte[0]; // Return an empty byte array if blockNumber is out of bounds
        }
    }

    public byte[] getData() {
        return data;
    }


    public int getFileSize() {
        return data.length ;
    }

    public String getFilename() {
        return filename;
    }
}
