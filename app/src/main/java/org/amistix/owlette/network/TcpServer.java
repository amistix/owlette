package org.amistix.owlette.network;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

import org.json.JSONObject;

public class TcpServer {
    private volatile Socket lastServerSocket;

    private static boolean serverRunning = false;

    public interface MessageHandler {
        void onMessage(String message, int senderClientPort, int receiverClientPort);
        void onError(Exception e);
    }

    public boolean isServerRunning(){
        return serverRunning;
    }

    private final ExecutorService executor = Executors.newCachedThreadPool();

    public void startServer(int port, MessageHandler handler) {
        executor.submit(() -> {
            try (ServerSocket serverSocket = new ServerSocket(port)) {

                while (true) {
                    try {
                        serverRunning = true;
                        Socket socket = serverSocket.accept();
                        lastServerSocket = socket;
                        executor.submit(() -> handleClient(socket, handler));
                    } catch (SocketTimeoutException e) {
                        handler.onError(e);
                    }
                }
            } catch (IOException e) {
                handler.onError(e);
            }
        });
    }

    private void handleClient(Socket socket, MessageHandler handler) {
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String line;
            while ((line = in.readLine()) != null) {
                try {
                    JSONObject obj = new JSONObject(line);
                    String msg = obj.optString("message", line);
                    Integer senderClientPort = obj.optInt("senderClientPort", 0);
                    Integer receiverClientPort = obj.optInt("receiverClientPort", 0);
                    handler.onMessage(msg, senderClientPort, receiverClientPort);
                } catch (org.json.JSONException e) {
                    e.printStackTrace();
                    return; // Abort sending if JSON fails
                }
            }
        } catch (SocketTimeoutException e) {
            handler.onError(new IOException("Read timed out from " + socket.getInetAddress(), e));
        } catch (IOException e) {
            handler.onError(e);
        }
    }
}
