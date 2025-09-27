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


import org.amistix.owlette.network.TcpClient;
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

import java.util.ArrayList;
import java.util.Arrays;


public class ChannelFragment extends Fragment {

    private RecyclerViewAdapter adapter;

    private TcpClient tcpClient;
    private DaemonWrapper daemon;

    private String connectedDeviceIp;

    public ChannelFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_channel, container, false);
        RecyclerView recyclerView = rootView.findViewById(R.id.recycler_gchat);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        adapter = new RecyclerViewAdapter(new ArrayList<>());
        recyclerView.setAdapter(adapter);

        Button buttonSend = rootView.findViewById(R.id.button_send);
        EditText editMessage = rootView.findViewById(R.id.edit_message);

        tcpClient = new TcpClient();

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String message = editMessage.getText().toString();
                editMessage.getText().clear();
                if (message.isEmpty()) return;

                switchCommands(message);

                new Thread(() -> {
                    tcpClient.sendFromClient(message);

                    requireActivity().runOnUiThread(() -> {
                        adapter.addItem(message);
                        recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                    });
                }).start();
            }
        });

        if (!tcpClient.isServerRunning()) {
            tcpClient.startServer(8080, new TcpClient.MessageHandler() {
                @Override
                public void onMessage(String message, InetAddress addr, int port) {
                    if (message.isEmpty()) return;
                    if (isAdded()) {
                        requireActivity().runOnUiThread(() -> {
                            adapter.addItem(message);
                            recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                        });
                    }
                }

                @Override
                public void onError(Exception e) {
                    if (isAdded()) {
                        requireActivity().runOnUiThread(() -> {
                            adapter.addItem("[SERVER ERROR] " + e.getMessage());
                            recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                        });
                    }
                }
            });
        }
        

        return rootView;
    }

    private void switchCommands(String message) {
        String[] words = message.split("\\s+");
        switch (words[0]) {
            case "/connect":
                connectedDeviceIp = words[1];
                adapter.addItem("Device " + connectedDeviceIp + " was added!");
                tcpClient.startClient(connectedDeviceIp, Integer.valueOf(words[2]), 10000, new TcpClient.MessageHandler() {
                    @Override
                    public void onMessage(String msg, InetAddress addr, int port) {
                        return;
                    }

                    @Override
                    public void onError(Exception e) {
                        adapter.addItem("[CLIENT ERROR] " + e.getMessage());
                    }
                });
                break;

            case "/showwebpage":
                adapter.addItem(I2PD_JNI.getWebConsAddr());
                break;

            case "/reload":
                I2PD_JNI.reloadTunnelsConfigs();
                break;

            // case "/addtunnelconfig":
            //     TunnelsConfig.setTunnelProperty(daemon.getRootPath(), words[1], words[2], words[3]);
            //     break;
        }
    }
}

