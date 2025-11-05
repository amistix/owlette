package org.amistix.owlette;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.NotificationCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import com.google.android.material.navigation.NavigationView;

import org.amistix.owlette.i2pd.*;
import org.amistix.owlette.network.*;

import java.net.InetAddress;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import org.amistix.owlette.SharedViewModel;

import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

public class MainActivity extends AppCompatActivity {

    private static final String CHANNEL_ID = "local_channel";

    private AppBarConfiguration mAppBarConfiguration;
    private DaemonWrapper daemon;
    private ExecutorService bgExecutor;
    private SharedViewModel sharedViewModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        bgExecutor = Executors.newSingleThreadExecutor();

        daemon = DaemonWrapper.getInstance(this);
        daemon.startDaemon();
        
        sharedViewModel = new ViewModelProvider(this).get(SharedViewModel.class);

        // Example: load and start TCP servers in background
        Set<String> tunnels = TunnelsConfig.getTunnels(daemon.getRootPath());
        for (String tunnel : tunnels) {
            Map<String, String> props = TunnelsConfig.getTunnelProperties(daemon.getRootPath(), tunnel);

            if (!props.containsKey("type")) continue;
            if (!"server".equals(props.get("type"))) continue;

            TcpServer server = new TcpServer();
            bgExecutor.submit(() -> {
                if (!server.isServerRunning()) {
                    server.startServer(Integer.parseInt(props.get("port")), new TcpServer.MessageHandler() {
                        @Override
                        public void onMessage(String message, int senderClientPort, int receiverClientPort) {
                                sharedViewModel.postMessage(message, receiverClientPort, senderClientPort);
                                createNotificationChannel();
                                showNotification("New message!", message);
                        }

                        @Override
                        public void onError(Exception e) {
                            // handle error if needed
                        }
                    });
                }
            });
        }

        // Standard Navigation setup
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        NavigationView navigationView = findViewById(R.id.nav_view);

        NavHostFragment navHostFragment =
                (NavHostFragment) getSupportFragmentManager().findFragmentById(R.id.nav_host_fragment);
        NavController navController = navHostFragment.getNavController();

        mAppBarConfiguration = new AppBarConfiguration.Builder(
                R.id.nav_edit_tunnels, R.id.nav_home, R.id.nav_settings, R.id.nav_qrcode
        )
                .setOpenableLayout(drawer)
                .build();

        NavigationUI.setupActionBarWithNavController(this, navController, mAppBarConfiguration);
        NavigationUI.setupWithNavController(navigationView, navController);
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController =
                ((NavHostFragment) getSupportFragmentManager().findFragmentById(R.id.nav_host_fragment))
                        .getNavController();
        return NavigationUI.navigateUp(navController, mAppBarConfiguration)
                || super.onSupportNavigateUp();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (daemon != null) {
            daemon.stopDaemon();
        }
        if (bgExecutor != null) {
            bgExecutor.shutdownNow();
        }
    }

    // --- Notification setup (local only) ---
    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID,
                    "Local Notifications",
                    NotificationManager.IMPORTANCE_DEFAULT
            );
            NotificationManager manager = getSystemService(NotificationManager.class);
            if (manager != null) {
                manager.createNotificationChannel(channel);
            }
        }
    }

    private void showNotification(String title, String message) {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setSmallIcon(android.R.drawable.ic_dialog_info)
                .setContentTitle(title)
                .setContentText(message)
                .setAutoCancel(true);

        NotificationManager manager = (NotificationManager)
                getSystemService(Context.NOTIFICATION_SERVICE);
        if (manager != null) {
            manager.notify(1, builder.build());
        }
    }
}
