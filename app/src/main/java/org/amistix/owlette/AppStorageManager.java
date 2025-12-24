package org.amistix.owlette;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import org.amistix.owlette.BuildConfig;

import java.io.*;

public class AppStorageManager {

    private static final String TAG = "AppStorageManager";
    private String appStoragePath;
    private AssetManager assetManager;

    static {
        System.loadLibrary("native-lib");
    }

    private static native void setAppStoragePath(String path);

    public AppStorageManager(Context ctx) {
        assetManager = ctx.getAssets();

        // Determine writable folder
        File baseDir = ctx.getExternalFilesDir(null);
        if (baseDir == null) baseDir = ctx.getFilesDir();

        appStoragePath = new File(baseDir, "owlette").getAbsolutePath();
        new File(appStoragePath).mkdirs();

        // Copy assets if needed
        processAssets(BuildConfig.VERSION_NAME);

        // Pass path to native C++
        setAppStoragePath(appStoragePath);
    }

    private void processAssets(String versionName) {
        File markerFile = new File(appStoragePath, "assets.ready");
        String currentMarker = readFile(markerFile);

        if (!currentMarker.contains(versionName)) {
            // Clear old data
            deleteRecursive(new File(appStoragePath, "certificates"));

            // Copy required assets
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
            String[] list = assetManager.list(path);

            if (list == null || list.length == 0) {
                // It's a file
                copyFile(path);
            } else {
                // It's a directory
                File dir = new File(appStoragePath, path);
                dir.mkdirs();
                for (String child : list) {
                    copyAsset(path + "/" + child);
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "Error copying asset: " + path, e);
        }
    }

    private void copyFile(String path) {
        File outFile = new File(appStoragePath, path);
        try (InputStream in = assetManager.open(path);
             OutputStream out = new FileOutputStream(outFile)) {

            // Ensure parent exists
            File parent = outFile.getParentFile();
            if (parent != null) parent.mkdirs();

            byte[] buffer = new byte[4096];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }

        } catch (IOException e) {
            Log.e(TAG, "Error copying file: " + path, e);
        }
    }

    private void deleteRecursive(File file) {
        if (!file.exists()) return;
        if (file.isDirectory()) {
            for (File child : file.listFiles()) deleteRecursive(child);
        }
        file.delete();
    }

    private String readFile(File file) {
        if (!file.exists()) return "";
        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) sb.append(line);
            return sb.toString();
        } catch (IOException e) {
            Log.e(TAG, "readFile error: " + file, e);
            return "";
        }
    }

    private void writeFile(File file, String content) {
        try {
            File parent = file.getParentFile();
            if (parent != null) parent.mkdirs();
            try (FileWriter fw = new FileWriter(file)) {
                fw.write(content);
            }
        } catch (IOException e) {
            Log.e(TAG, "writeFile error: " + file, e);
        }
    }
}
