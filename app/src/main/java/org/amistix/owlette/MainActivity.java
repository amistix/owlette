package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;

import org.amistix.owlette.i2pd.*;

public class MainActivity extends AppCompatActivity {

    private DaemonWrapper daemon;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        daemon = DaemonWrapper.getInstance(this);
        daemon.startDaemon();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (daemon != null) {
            daemon.stopDaemon();
        }
    }
}
