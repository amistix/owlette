package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.opengl.GLSurfaceView;

import org.amistix.owlette.i2pd.*;

public class MainActivity extends AppCompatActivity {

    private DaemonWrapper daemon;
    private GLView glView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        glView = new GLView(this);
        setContentView(glView);


        daemon = DaemonWrapper.getInstance(this);
        daemon.startDaemon();

    }

    @Override
    public void onResume() {
        super.onResume();
        glView.onResume();
    }

    @Override
    public void onPause() {
        glView.onPause();
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (daemon != null) {
            daemon.stopDaemon();
        }
    }
}
