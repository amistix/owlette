package org.amistix.owlette.i2pd;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;

import androidx.core.content.ContextCompat;
import org.amistix.owlette.BuildConfig;

import java.io.*;
import java.util.Locale;

public class DaemonWrapper {
    private static final String TAG = "DaemonWrapper";

    private static DaemonWrapper instance;
    private static boolean daemonRunning = false; 

    private final AssetManager assetManager;
    private final String owlettePath;
    private static final String appLocale =  Locale.getDefault().getDisplayLanguage(Locale.ENGLISH).toLowerCase();

    private DaemonWrapper(Context ctx) {
        this.assetManager = ctx.getAssets();

        // public path we want
        String publicPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/owlette";

        boolean canUsePublic = false;
        // Android 11+ needs MANAGE_EXTERNAL_STORAGE (special permission)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            try {
                canUsePublic = Environment.isExternalStorageManager();
            } catch (NoSuchMethodError | SecurityException ignored) {
                canUsePublic = false;
            }
        } else {
            // Android < 11: check WRITE_EXTERNAL_STORAGE permission
            int perm = ContextCompat.checkSelfPermission(ctx, android.Manifest.permission.WRITE_EXTERNAL_STORAGE);
            canUsePublic = (perm == android.content.pm.PackageManager.PERMISSION_GRANTED);
        }

        if (canUsePublic) {
            this.owlettePath = publicPath;
        } else {
            // fallback to app-private external storage (no runtime permission needed)
            File appDir = ctx.getExternalFilesDir(null);
            if (appDir != null) {
                this.owlettePath = appDir.getAbsolutePath() + "/owlette";
            } else {
                // last fallback: internal files dir
                this.owlettePath = ctx.getFilesDir().getAbsolutePath() + "/owlette";
            }
        }

        Log.d(TAG, "using owlettePath = " + this.owlettePath);
    }

    public static DaemonWrapper getInstance(Context ctx) {
        if (instance == null) {
            instance = new DaemonWrapper(ctx);
        }
        return instance;
    }

    public String getRootPath() {
        return owlettePath;
    }

    public synchronized void stopDaemon() {
        daemonRunning = false;
        I2PD_JNI.stopDaemon();
    }

    public synchronized void startDaemon() {
        if (daemonRunning) return;
        daemonRunning = true;
        new Thread(() -> {
            String version = BuildConfig.VERSION_NAME;
            processAssets(version);
            I2PD_JNI.loadLibraries();
            synchronized (DaemonWrapper.this) {
                I2PD_JNI.setDataDir(owlettePath);
                Log.i(TAG, "setting webconsole language to " + appLocale);
                I2PD_JNI.setLanguage(appLocale);
                I2PD_JNI.startDaemon();
            }
        }, "i2pdDaemonStart").start();
    }

    public void processAssets(String versionName) {
        File appDir = new File(owlettePath);
        if (!appDir.exists()) {
            boolean created = appDir.mkdirs();
            Log.d(TAG, "Created dir: " + appDir + " -> " + created);
        }

        File markerFile = new File(appDir, "assets.ready");
        String currentMarker = readFile(markerFile);

        if (!currentMarker.contains(versionName)) {
            deleteRecursive(new File(appDir, "certificates"));

            copyAsset("addressbook");
            copyAsset("certificates");
            copyAsset("i2pd.conf");
            copyAsset("tunnels.conf");
            copyAsset("tunnels.d");

            writeFile(markerFile, versionName);
        }
    }

private void copyAsset(String path) {
    try {
        InputStream in = null;
        try {
            in = assetManager.open(path); // try open as file
            copyFileAsset(path);
            return; // done
        } catch (IOException ignored) {
            // Not a file, must be a directory
        } finally {
            if (in != null) in.close();
        }

        // Directory
        String[] contents = assetManager.list(path);
        if (contents != null) {
            File dir = new File(owlettePath, path);
            if (!dir.exists()) dir.mkdirs();
            for (String entry : contents) {
                copyAsset(path + "/" + entry);
            }
        }
    } catch (IOException e) {
        Log.e(TAG, "Error copying asset: " + path, e);
    }
}

    private void copyFileAsset(String path) {
        File outFile = new File(owlettePath, path);

        // Ensure parent exists
        File parent = outFile.getParentFile();
        if (parent != null && !parent.exists()) parent.mkdirs();

        // Avoid overwriting existing file (like original DaemonWrapper behavior)
        if (outFile.exists()) return;

        try (InputStream in = assetManager.open(path);
             OutputStream out = new FileOutputStream(outFile)) {
            byte[] buffer = new byte[4096];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
        } catch (IOException e) {
            Log.e(TAG, "Error copying file asset: " + path, e);
        }
    }

    private void deleteRecursive(File file) {
        if (!file.exists()) return;
        if (file.isDirectory()) {
            File[] children = file.listFiles();
            if (children != null) {
                for (File child : children) deleteRecursive(child);
            }
        }
        boolean deleted = file.delete();
        if (!deleted) Log.d(TAG, "delete returned false for " + file.getAbsolutePath());
    }

    private String readFile(File file) {
        if (!file.exists()) return "";
        StringBuilder sb = new StringBuilder();
        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = br.readLine()) != null) sb.append(line);
            return sb.toString();
        } catch (IOException e) {
            Log.e(TAG, "readFile error: " + file, e);
            return "";
        }
    }

    private void writeFile(File file, String content) {
        // ensure parent dir exists
        File parent = file.getParentFile();
        if (parent != null && !parent.exists()) parent.mkdirs();

        try (FileWriter fw = new FileWriter(file)) {
            fw.write(content);
        } catch (IOException e) {
            Log.e(TAG, "Error writing file: " + file, e);
        }
    }
}
