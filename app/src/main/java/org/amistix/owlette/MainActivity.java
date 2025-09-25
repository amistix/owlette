package org.amistix.owlette;

import org.amistix.owlette.network.TcpClient;
import org.amistix.owlette.i2pd.*;
import org.amistix.owlette.ui.RecyclerViewAdapter;

import android.app.Activity;
import android.os.Bundle;

import android.widget.Button;
import android.view.View;
import android.widget.EditText;
import java.io.IOException;
import android.os.AsyncTask;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.LinearLayoutManager;
import org.amistix.owlette.databinding.ActivityChannelBinding;

import java.util.ArrayList;
import java.util.Arrays;

public class MainActivity extends Activity {

    private ActivityChannelBinding binding;
    private RecyclerViewAdapter adapter;

    private static MainActivity instance;
    private TcpClient tcpClient;
    private DaemonWrapper daemon;

    private String connectedDeviceIp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        instance = this;

        binding = ActivityChannelBinding.inflate(getLayoutInflater());

        setContentView(binding.getRoot());
        binding.toolbarTitle.setText("@anon");


        RecyclerView recyclerView = binding.recyclerGchat;
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        adapter = new RecyclerViewAdapter(new ArrayList<>());
        recyclerView.setAdapter(adapter);

        Button buttonSend = binding.buttonSend;
        EditText editMessage = binding.editMessage;

        tcpClient = new TcpClient();

        daemon = new DaemonWrapper(this);
        daemon.startDaemon();

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String message = editMessage.getText().toString();
                editMessage.getText().clear();
                if (message.isEmpty()) return;

                switchCommands(message);

                new Thread(() -> {
                    tcpClient.sendFromClient(message);

                    runOnUiThread(() -> {
                        adapter.addItem(message);
                        recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                    });
                }).start();
            }
        });

        tcpClient.startServer(8080, new TcpClient.MessageHandler() {
            @Override
            public void onMessage(String message, InetAddress addr, int port) {
                if (message.isEmpty()) return;
                runOnUiThread(() -> {
                    adapter.addItem(message);
                    recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                });
            }

            @Override
            public void onError(Exception e) {
                adapter.addItem("[SERVER ERROR] " + e.getMessage());
            }
        });
    }

    private void switchCommands(String message) {
        String[] words = message.split("\\s+");
        switch (words[0]) {
            case "/connect":
                connectedDeviceIp = words[1];
                adapter.addItem("Device " + connectedDeviceIp + " was added!");
                tcpClient.startClient(connectedDeviceIp, 9090, 10000, new TcpClient.MessageHandler() {
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

            case "/addtunnelconfig":
                TunnelsConfig.setTunnelProperty(daemon.getRootPath(), words[1], words[2], words[3]);
                break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        daemon.stopDaemon();
    }
}