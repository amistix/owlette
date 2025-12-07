package org.amistix.owlette;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import java.nio.ByteBuffer;
import android.util.Log;

public class FontRenderer {

    static {
        System.loadLibrary("native-lib");
    }
    private static final String TAG = "FontRenderer";

    private static final String CHARSET =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" +
        " .,!?;:'-_()[]{}@#$%^&*+=/<>\"\\|`~";

    private static boolean initialized = false;

    public static void initFont(float textSize) {
        
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setTextSize(textSize);
        paint.setColor(0xFFFFFFFF);

        int totalWidth = 0;
        for (char c : CHARSET.toCharArray()) {
            int w = (int) paint.measureText(String.valueOf(c));
            totalWidth += w;
            Log.d(TAG, "  Char '" + c + "' width: " + w);
        }

        int height = (int) (paint.getFontMetrics().bottom - paint.getFontMetrics().top);

        Bitmap atlas = Bitmap.createBitmap(totalWidth, height, Bitmap.Config.ALPHA_8);
        Canvas canvas = new Canvas(atlas);

        int x = 0;
        int[] glyphX = new int[CHARSET.length()];
        int[] glyphW = new int[CHARSET.length()];

        for (int i = 0; i < CHARSET.length(); i++) {
            char c = CHARSET.charAt(i);
            int w = (int) paint.measureText(String.valueOf(c));
            canvas.drawText(String.valueOf(c), x, -paint.getFontMetrics().top, paint);

            glyphX[i] = x;
            glyphW[i] = w;
            x += w;
        }

        Log.d(TAG, "Creating ByteBuffer of size: " + (atlas.getWidth() * atlas.getHeight()));
        ByteBuffer buffer = ByteBuffer.allocateDirect(atlas.getWidth() * atlas.getHeight());
        atlas.copyPixelsToBuffer(buffer);
        buffer.position(0);

        Log.d(TAG, "Calling native method...");
        nativeSetFontData(
                buffer,
                atlas.getWidth(),
                atlas.getHeight(),
                glyphX,
                glyphW,
                CHARSET
        );
        
        initialized = true;
    }

    public static boolean isInitialized() {
        return initialized;
    }

    public static native void nativeSetFontData(
            ByteBuffer buf,
            int width,
            int height,
            int[] glyphX,
            int[] glyphW,
            String chars
    );
}