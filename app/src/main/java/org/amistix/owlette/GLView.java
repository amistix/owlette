package org.amistix.owlette;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.Choreographer;

public class GLView extends GLSurfaceView implements Choreographer.FrameCallback {

    static {
        System.loadLibrary("i2pd");
    }

    private native void nativeInit();
    private native void nativeResize(int width, int height);
    private native void nativeDraw();

    public GLView(Context context) {
        super(context);
        setEGLContextClientVersion(2);

        setRenderer(new Renderer() {
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
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void onResume() {
        super.onResume();
        Choreographer.getInstance().postFrameCallback(this);
    }

    @Override
    public void onPause() {
        Choreographer.getInstance().removeFrameCallback(this);
        super.onPause();
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        requestRender();
        Choreographer.getInstance().postFrameCallback(this);
    }
} 