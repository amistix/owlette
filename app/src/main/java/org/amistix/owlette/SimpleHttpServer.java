package org.amistix.owlette;

import fi.iki.elonen.NanoHTTPD;
import java.io.IOException;
import java.util.HashMap;
import java.nio.charset.StandardCharsets;
import java.util.Map;

public class SimpleHttpServer extends NanoHTTPD {
    private OnMessageReceivedListener listener;

    public void setOnMessageReceivedListener(OnMessageReceivedListener listener) {
        this.listener = listener;
    }

    public SimpleHttpServer(int port) throws IOException {
        super("0.0.0.0", port);
        start(NanoHTTPD.SOCKET_READ_TIMEOUT, false);
        System.out.println("Server started on port " + port);
    }

    @Override
    public Response serve(IHTTPSession session) {
        Method method = session.getMethod();

        if (Method.POST.equals(method)) {
            try {
                int contentLength = Integer.parseInt(session.getHeaders().get("content-length"));
                byte[] buffer = new byte[contentLength];
                session.getInputStream().read(buffer, 0, contentLength);
                String postData = new String(buffer, StandardCharsets.UTF_8);

                // Call a listener or handler
                if (listener != null) {
                    listener.onMessageReceived(postData);
                }

                return newFixedLengthResponse(Response.Status.OK, "text/plain", "Message received.\n");
            } catch (Exception e) {
                e.printStackTrace();
                return newFixedLengthResponse(Response.Status.INTERNAL_ERROR, "text/plain", "Error parsing request.\n");
            }
        }

        return newFixedLengthResponse("Use POST to send a message.");
    }
}
