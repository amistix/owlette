package org.amistix.owlette.network;

import org.json.JSONObject;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

public class TcpClient {

    private volatile Socket lastClientSocket;

    private String clientAddress = "127.0.0.1";
    private int clientPort = 9090;
    private int destinationClientPort = 9090;

    private static TcpClient clientInstance;

    public interface MessageHandler {
        void onMessage(String message, InetAddress address, int port);
        void onError(Exception e);
    }

    private final ExecutorService executor = Executors.newCachedThreadPool();

    public static synchronized TcpClient getInstance() {
        if (clientInstance == null) {
            clientInstance = new TcpClient();
        }
        return clientInstance;
    }

    public void setClientHost(String address, int port, int destPort) {
        this.clientAddress = address;
        this.destinationClientPort = destPort;
        this.clientPort = port;
    }

    public Integer getClientPort() {
        return clientPort;
    }
    public Integer getDestinationClientPort() {
        return destinationClientPort;
    }

    public void startClient(int timeoutMs, MessageHandler handler) {
        executor.submit(() -> {
            try (Socket socket = new Socket()) {
                socket.connect(new InetSocketAddress(clientAddress, clientPort), timeoutMs);
                lastClientSocket = socket;

                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                String line;
                while ((line = in.readLine()) != null) {
                    try {
                        JSONObject obj = new JSONObject(line);
                        String text = obj.optString("message", line);
                        handler.onMessage(text, socket.getInetAddress(), socket.getPort());
                    } catch (Exception e) {
                        // Fallback if it's not valid JSON
                        handler.onMessage(line, socket.getInetAddress(), socket.getPort());
                    }
                }
            } catch (SocketTimeoutException e) {
                handler.onError(new IOException("Connection to " + clientAddress + ":" + clientPort + " timed out", e));
            } catch (IOException e) {
                handler.onError(e);
            }
        });
    }

    public void sendFromClient(Integer senderClientPort, Integer receiverClientPort, String data) {
        sendJson(senderClientPort, receiverClientPort, data);
    }

    public void sendJson(Integer senderClientPort, Integer receiverClientPort, String data) {
        if (lastClientSocket != null && lastClientSocket.isConnected()) {
            executor.submit(() -> {
                try {
                    JSONObject json = new JSONObject();
                    try {
                        json.put("senderClientPort", senderClientPort);
                        json.put("receiverClientPort", receiverClientPort);
                        json.put("message", data);
                    } catch (org.json.JSONException e) {
                        e.printStackTrace();
                        return; // Abort sending if JSON fails
                    }

                    PrintWriter out = new PrintWriter(lastClientSocket.getOutputStream(), true);
                    out.println(json.toString());
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
        }
    }
}
