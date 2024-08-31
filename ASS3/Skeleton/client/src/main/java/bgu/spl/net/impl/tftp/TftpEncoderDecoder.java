
package bgu.spl.net.impl.tftp;

import bgu.spl.net.api.MessageEncoderDecoder;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class TftpEncoderDecoder implements MessageEncoderDecoder<byte[]> {

    private byte[] bytes = new byte[1 << 10]; // start with 1k
    private static int len = 0;
    public static int errCode = -1;
    public static short opcode = -1;
    public static int blockNum = -1;
    public static int packetSize = -1;
    public static int currentSize = 0;


    @Override
    public byte[] decodeNextByte(byte nextByte) {
        currentSize++;
        if (len == 1 && opcode == -1) {

            pushByte(nextByte);
            opcode = getOpErrCode(bytes);

            if (opcode != -1) {
                len = 0;
            }
            if (opcode == 6 || opcode == 10) {
                String result = "   ";
                return result.getBytes();
            }
            return null;
        }
        if (opcode == 9 && len == 0 && blockNum == -1) {
            if (nextByte == 0) {
                blockNum = 0;
                len = 0;
                return null;
            } else if (nextByte == 1) {
                blockNum = 1;
                len = 0;
                return null;
            }
        }
        if (opcode == 5) {
            if (len == 1 && errCode == -1) {
                pushByte(nextByte);
                errCode = getOpErrCode(bytes);
                len = 0;
                return null;
            }
        }

        if (opcode == 4) {
            if (len == 1 && blockNum == -1) {
                pushByte(nextByte);
                blockNum = bytesConvert(bytes);
                len = 0;
                String result = "";
                return result.getBytes();
            }
        }
        if (opcode == 3) {
            if (len == 1 && packetSize == -1) {
                pushByte(nextByte);
                packetSize = bytesConvert(bytes);
                len = 0;
                return null;
            }
            if (len == 1 && packetSize != -1 && blockNum == -1) {
                pushByte(nextByte);
                blockNum = bytesConvert(bytes);
                len = 0;
                return null;
            }
            if (packetSize != -1 && blockNum != -1) {
                pushByte(nextByte);
                if (currentSize == packetSize + 6) {
                    byte[] array = new byte[len];
                    for (int i = 0; i < len; i++) {
                        array[i] = bytes[i];
                    }
                    return array;
                }
                return null;
            }
        }
        if ((nextByte == '\0' || nextByte == '\n') && (len != 0)) {
            String result = popString();
            return result.getBytes();
        }
        pushByte(nextByte);
        return null;
    }
    public short getOpErrCode(byte[] byteArr) {
         if (byteArr[0] == 0 && byteArr[1] >= 0 && byteArr[1] <= 10) {
            return byteArr[1];
        } else {
        byteArr[0] = 0;
        byteArr[1] = 0;
        return -1;
    }
}

    public int bytesConvert(byte[] byteArr) {
        return ((byteArr[0] & 0xFF) << 8) | (byteArr[1] & 0xFF);

    }
    @Override
    public byte[] encode(byte[] message) {
        return (message);
    }
    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }
        bytes[len++] = nextByte;
    }

    private String popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }
    public static int getErrCode(){return errCode ;}

    public static short getOpcode() {
        return opcode;
    }


    public static int getBlocknum() {
        return blockNum;
    }

    public static int getPacketsize() {
        return packetSize;
    }

    public static void reset() {
        opcode = -1;
        errCode = -1;
        blockNum = -1;
        packetSize = -1;
        currentSize = 0;
        len = 0;
    }

}
