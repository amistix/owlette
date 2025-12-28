package org.amistix.owlette;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import java.nio.ByteBuffer;
import android.util.Log;

public class FontRenderer {

    private static final String TAG = "FontRenderer";

    private static final String CHARSET =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" +
        " .,!?;:'-_()[]{}@#$%^&*+=/<>\"\\|`~";

    public static JavaFontAtlas initFont(float textSize) {
        
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setTextSize(textSize);
        paint.setColor(0xFFFFFFFF);
        paint.setStyle(Paint.Style.FILL);
        paint.setTextAlign(Paint.Align.LEFT);

        Paint.FontMetrics metrics = paint.getFontMetrics();
        
        int ascent = (int) Math.ceil(Math.abs(metrics.ascent));
        int descent = (int) Math.ceil(Math.abs(metrics.descent));
        int height = ascent + descent;
        
        Log.d(TAG, "Font metrics - ascent: " + ascent + ", descent: " + descent + ", height: " + height);

        // Measure total width
        int totalWidth = 0;
        int[] glyphWidths = new int[CHARSET.length()];
        
        for (int i = 0; i < CHARSET.length(); i++) {
            char c = CHARSET.charAt(i);
            float measured = paint.measureText(String.valueOf(c));
            int w = (int) Math.ceil(measured);
            glyphWidths[i] = Math.max(w, 1);
            totalWidth += glyphWidths[i];
        }

        Log.d(TAG, "Creating atlas: " + totalWidth + "x" + height + " for size " + textSize);

        // Create bitmap
        Bitmap atlas = Bitmap.createBitmap(totalWidth, height, Bitmap.Config.ALPHA_8);
        Canvas canvas = new Canvas(atlas);
        canvas.drawColor(0x00000000);

        int x = 0;
        int[] glyphX = new int[CHARSET.length()];
        int[] glyphW = new int[CHARSET.length()];

        // Draw characters
        for (int i = 0; i < CHARSET.length(); i++) {
            char c = CHARSET.charAt(i);
            int w = glyphWidths[i];
            
            canvas.drawText(String.valueOf(c), x, ascent, paint);

            glyphX[i] = x;
            glyphW[i] = w;
            x += w;
        }

        // ✅ MANUAL PIXEL COPY - ensures proper byte order
        int bufferSize = totalWidth * height;
        ByteBuffer buffer = ByteBuffer.allocateDirect(bufferSize);
        
        // Get pixels as byte array
        byte[] pixels = new byte[bufferSize];
        ByteBuffer tempBuffer = ByteBuffer.allocate(bufferSize);
        atlas.copyPixelsToBuffer(tempBuffer);
        tempBuffer.rewind();
        tempBuffer.get(pixels);
        
        // Copy to direct buffer
        buffer.put(pixels);
        buffer.rewind();

        Log.d(TAG, "Buffer size: " + bufferSize + " bytes");
        
        // Log pixel sample for verification
        int nonZero = 0;
        for (int i = 0; i < Math.min(1000, pixels.length); i++) {
            if (pixels[i] != 0) nonZero++;
        }
        Log.d(TAG, "Pixel check: " + nonZero + " non-zero in first 1000 pixels");

        JavaFontAtlas result = new JavaFontAtlas();
        result.fontSize = textSize;
        result.buffer = buffer;
        result.width = totalWidth;
        result.height = height;
        result.glyphX = glyphX;
        result.glyphW = glyphW;
        result.chars = CHARSET;

        return result;
    }
}