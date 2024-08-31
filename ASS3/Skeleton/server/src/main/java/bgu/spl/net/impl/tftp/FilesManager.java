package bgu.spl.net.impl.tftp;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.concurrent.ConcurrentHashMap;

public class FilesManager {
    private String directory ;
    private ConcurrentHashMap<String , byte[]> files ;
    private ConcurrentHashMap<String , Integer> size ;
    private ConcurrentHashMap<String , Path> nameToPath ;
    public FilesManager(String directory){
        this.directory = directory ;
        this.files = new ConcurrentHashMap<>() ;
        this.nameToPath = new ConcurrentHashMap<>() ;
        this.size = new ConcurrentHashMap<>();
        try {
            Files.list(Paths.get(directory)).filter(Files::isRegularFile)
                        .forEach(filePath -> {
                        String fileName = filePath.getFileName().toString();
                        try {
                            byte[] fileData = Files.readAllBytes(filePath);
                            files.put(fileName, fileData);
                            nameToPath.put(fileName, filePath);
                            size.put(fileName, fileData.length);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    });
        }catch (IOException e){
        }
    }
    public ConcurrentHashMap<String , byte[]> getFiles(){
        return files ;
    }
    public boolean isEmpty(){
        return files.isEmpty() ;
    }
    public boolean fileExists(String fileName) {
        return files.containsKey(fileName);
    }
    public void createFile(String fileName) throws IOException {
            Path filePath = Paths.get(directory, fileName);
            Files.createFile(filePath);
            files.put(fileName, new byte[0]);
            size.put(fileName, 0);
            nameToPath.put(fileName, filePath);
        }
    public byte[] readDataFromFile(String fileName)throws IOException{
        return files.get(fileName) ;
    }

    public void deleteFile(String fileName)throws IOException{
        Path filePath = Paths.get(directory , fileName) ;
        Files.delete(filePath);
        files.remove(fileName);
        nameToPath.remove(fileName) ;
        size.remove(fileName) ;
    }
    public void writeData(String fileName , byte[] data) throws IOException{
        byte[] fileData = files.get(fileName);
        byte[] newData = new byte[fileData.length + data.length];
        System.arraycopy(fileData, 0, newData, 0, fileData.length);
        System.arraycopy(data, 0, newData, fileData.length, data.length);

        files.put(fileName, newData);
        size.put(fileName, newData.length);

        Path filePath = nameToPath.get(fileName);
        Files.write(filePath, data, StandardOpenOption.APPEND);
    }

}



