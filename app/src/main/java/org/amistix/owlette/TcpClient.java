package org.amistix.owlette;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

public class TcpClient {

    private volatile Socket lastClientSocket;
    private volatile Socket lastServerSocket;

    public interface MessageHandler {
        void onMessage(String message, InetAddress address, int port);
        void onError(Exception e);
    }

    private final ExecutorService executor = Executors.newCachedThreadPool();

    public void startServer(int port, MessageHandler handler) {
        executor.submit(() -> {
            try (ServerSocket serverSocket = new ServerSocket(port)) {

                while (true) {
                    try {
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

    public void startClient(String host, int port, int timeoutMs, MessageHandler handler) {
        executor.submit(() -> {
            try (Socket socket = new Socket()) {
                socket.connect(new InetSocketAddress(host, port), timeoutMs);
                lastClientSocket = socket;

                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                out.println("Someone connected to you!");

                String line;
                while ((line = in.readLine()) != null) {
                    handler.onMessage(line, socket.getInetAddress(), socket.getPort());
                }
            } catch (SocketTimeoutException e) {
                handler.onError(new IOException("Connection to " + host + ":" + port + " timed out", e));
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
