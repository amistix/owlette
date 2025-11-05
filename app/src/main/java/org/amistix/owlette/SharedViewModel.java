package org.amistix.owlette;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;
import java.net.InetAddress;
import org.amistix.owlette.network.ReceivedMessage;

public class SharedViewModel extends ViewModel {
    private final MutableLiveData<ReceivedMessage> messageLiveData = new MutableLiveData<>();

    // The fragment will observe this LiveData
    public LiveData<ReceivedMessage> getMessageLiveData() {
        return messageLiveData;
    }

    // The activity (server callback) will post messages here
    public void postMessage(String message, int receiverPort, int senderPort) {
        ReceivedMessage msg = new ReceivedMessage(message, receiverPort, senderPort);
        messageLiveData.postValue(msg); 
    }
}
