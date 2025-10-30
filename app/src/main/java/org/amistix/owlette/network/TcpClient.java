package org.amistix.owlette.network;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

public class TcpClient {

    private volatile Socket lastClientSocket;
    private volatile Socket lastServerSocket;

    private static boolean serverRunning = false;

    private String clientAddress = "127.0.0.1";
    private int clientPort = 9090;

    private static TcpClient clientInstance;

    public interface MessageHandler {
        void onMessage(String message, InetAddress address, int port);
        void onError(Exception e);
    }

    public boolean isServerRunning() {
        return serverRunning;
    }
    public static TcpClient getInstance() {
        if (clientInstance == null) {
            clientInstance = new TcpClient();
        }
        return clientInstance;
    }

    public void setClientHost(String address, int port)
    {
        this.clientAddress = address;
        this.clientPort = port;
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
                handler.onMessage(line, socket.getInetAddress(), socket.getPort());
            }
        } catch (SocketTimeoutException e) {
            handler.onError(new IOException("Read timed out from " + socket.getInetAddress(), e));
        } catch (IOException e) {
            handler.onError(e);
        }
    }

    public void startClient(int timeoutMs, MessageHandler handler) {
        executor.submit(() -> {
            try (Socket socket = new Socket()) {
                socket.connect(new InetSocketAddress(clientAddress, clientPort), timeoutMs);
                lastClientSocket = socket;

                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                out.println("Someone connected to you!");

                String line;
                while ((line = in.readLine()) != null) {
                    handler.onMessage(line, socket.getInetAddress(), socket.getPort());
                }
            } catch (SocketTimeoutException e) {
                handler.onError(new IOException("Connection to " + clientAddress + ":" + clientPort + " timed out", e));
            } catch (IOException e) {
                handler.onError(e);
            }
        });
    }

    public synchronized void sendFromClient(String data) {
        send(lastClientSocket, data);
    }

    private void send(Socket socket, String data) {
        if (socket != null && socket.isConnected()) {
            try {
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                out.println(data);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
