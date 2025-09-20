package org.amistix.owlette;

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

    private String connectedDeviceIp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        instance = this;

        binding = ActivityChannelBinding.inflate(getLayoutInflater());

        setContentView(binding.getRoot());
        binding.label.setText("@anon");


        RecyclerView recyclerView = binding.recyclerGchat;
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        RecyclerViewAdapter adapter = new RecyclerViewAdapter(new ArrayList<>());
        recyclerView.setAdapter(adapter);

        Button buttonSend = binding.buttonSend;
        EditText editMessage = binding.editMessage;

        tcpClient = new TcpClient();

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String message = editMessage.getText().toString();
                editMessage.getText().clear();
                if (message.isEmpty()) return;

                String[] words = message.split("\\s+");
                if ("/connect".equals(words[0])){
                    connectedDeviceIp = words[1];
                    adapter.addItem("Device " + connectedDeviceIp + " was added!");
                    tcpClient.startClient(connectedDeviceIp, 8080, 10000, new TcpClient.MessageHandler() {
                        @Override
                        public void onMessage(String msg, InetAddress addr, int port) {
                            return;
                        }

                        @Override
                        public void onError(Exception e) {
                            adapter.addItem("[CLIENT ERROR] " + e.getMessage());
                        }
                    });
                    return;
                }
                new AsyncTask<Void, Void, String>() {
                    @Override
                    protected String doInBackground(Void... params) {
                        tcpClient.sendFromClient(message);
                        return message;
                    }
                    @Override
                    protected void onPostExecute(String result) {
                        adapter.addItem(result);
                        recyclerView.scrollToPosition(adapter.getItemCount() - 1);
                    }
                }.execute();
                
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
    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
