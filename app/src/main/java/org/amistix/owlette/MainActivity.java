package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.opengl.GLSurfaceView;

import org.amistix.owlette.i2pd.*;

public class MainActivity extends AppCompatActivity {

    private DaemonWrapper daemon;
    private GLSurfaceView glView;

    static {
        System.loadLibrary("i2pd");
    }

    private native void nativeInit();
    private native void nativeResize(int width, int height);
    private native void nativeDraw();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(2);
        glView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10 gl, javax.microedition.khronos.egl.EGLConfig config) {
                nativeInit();
            }

            @Override
            public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl, int width, int height) {
                nativeResize(width, height);
            }

            @Override
            public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl) {
                nativeDraw();
            }
        });
        glView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        setContentView(glView);


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
