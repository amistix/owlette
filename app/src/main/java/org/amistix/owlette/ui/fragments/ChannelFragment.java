package org.amistix.owlette.ui;

import org.amistix.owlette.*;

import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import java.io.IOException;
import android.os.AsyncTask;
import android.widget.Toast;

import org.amistix.owlette.network.*;
import org.amistix.owlette.i2pd.*;
import org.amistix.owlette.ui.RecyclerViewAdapter;

import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.drawerlayout.widget.DrawerLayout;

import android.os.Bundle;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.LinearLayoutManager;
import org.amistix.owlette.databinding.ActivityChannelBinding;
import org.amistix.owlette.SharedViewModel;
import androidx.lifecycle.ViewModelProvider;


import java.util.ArrayList;
import java.util.Arrays;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ChannelFragment extends Fragment {
    private RecyclerViewAdapter adapter;
    private TcpClient tcpClient;
    private TcpServer tcpServer;
    private ExecutorService bgExecutor;
    private String connectedDeviceIp;
    private SharedViewModel sharedViewModel;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_channel, container, false);
        RecyclerView recyclerView = rootView.findViewById(R.id.recycler_gchat);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        adapter = new RecyclerViewAdapter();
        recyclerView.setAdapter(adapter);

        Button buttonSend = rootView.findViewById(R.id.button_send);
        EditText editMessage = rootView.findViewById(R.id.edit_message);

        // single executor reused for background tasks (no new threads per message)
        bgExecutor = Executors.newSingleThreadExecutor();

        tcpClient = TcpClient.getInstance();
        

        tcpClient.startClient(10000, new TcpClient.MessageHandler() {
            @Override
            public void onMessage(String msg, InetAddress addr, int port) {
                adapter.addItem("[CLIENT RESPONSE] " + msg);
            }

            @Override
            public void onError(Exception e) {
                adapter.addItem("[CLIENT ERROR] " + e.getMessage());
            }
        });
        Toast.makeText(getContext(), "Loaded", Toast.LENGTH_SHORT).show();

        buttonSend.setOnClickListener(v -> {
            String message = editMessage.getText().toString();
            editMessage.getText().clear();
            if (message.isEmpty()) return;

            switchCommands(message);

            bgExecutor.submit(() -> {
                try {
                    tcpClient.sendFromClient(tcpClient.getClientPort(), tcpClient.getDestinationClientPort(), message);
                } catch (Exception e) {
                    // log or handle
                }
                if (isAdded()) {
                    requireActivity().runOnUiThread(() -> {
                        adapter.addItem(message);
                        recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                    });
                }
            });
        });

        // Start server in background to avoid blocking UI
        sharedViewModel = new ViewModelProvider(requireActivity()).get(SharedViewModel.class);

        // Observe LiveData for new messages
        sharedViewModel.getMessageLiveData().observe(getViewLifecycleOwner(), (messageObject) -> {
            if (messageObject.message.isEmpty()) return;
            if (isAdded() ) {
                requireActivity().runOnUiThread(() -> {
                    adapter.addItem(messageObject.message);
                    recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                });
            }
        });

        return rootView;
    }
    private void switchCommands(String message) {
        String[] words = message.split("\\s+");
        switch (words[0]) {
            case "/showwebpage":
                adapter.addItem(I2PD_JNI.getWebConsAddr());
                break;

            case "/reload":
                I2PD_JNI.reloadTunnelsConfigs();
                break;
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        // Shutdown executor and stop server to avoid leaks
        if (bgExecutor != null) {
            bgExecutor.shutdownNow();
            bgExecutor = null;
        }
        if (tcpClient != null) {
            tcpClient = null;
        }
    }
}