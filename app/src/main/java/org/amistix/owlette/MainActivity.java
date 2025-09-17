package org.amistix.owlette;

import android.app.Activity;
import android.os.Bundle;

import android.widget.Button;
import android.view.View;
import android.widget.EditText;

import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.LinearLayoutManager;
import org.amistix.owlette.databinding.ActivityChannelBinding;

import java.util.ArrayList;
import java.util.Arrays;

public class MainActivity extends Activity {

    private ActivityChannelBinding binding;
    private RecyclerViewAdapter adapter;
    private static MainActivity instance;

    static {
        System.loadLibrary("i2pd");
    }

    public native String stringFromJNI();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        instance = this;

        binding = ActivityChannelBinding.inflate(getLayoutInflater());

        setContentView(binding.getRoot());
        binding.label.setText(stringFromJNI());


        RecyclerView recyclerView = binding.recyclerGchat;
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        RecyclerViewAdapter adapter = new RecyclerViewAdapter(new ArrayList<>());
        recyclerView.setAdapter(adapter);

        Button buttonSend = binding.buttonSend;
        EditText editMessage = binding.editMessage;

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String message = editMessage.getText().toString();
                if (message.isEmpty()) return;

                adapter.addItem(message);
                editMessage.getText().clear();
            }
        });


    }
}
