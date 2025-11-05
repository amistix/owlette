package org.amistix.owlette.network;

public class ReceivedMessage {
    public final String message;
    public final Integer receiverClientPort;
    public final Integer senderClientPort;

    public ReceivedMessage(String message, int receiverPort, int senderPort) {
        this.message = message;
        this.receiverClientPort = receiverPort;
        this.senderClientPort = senderPort;
    }
}