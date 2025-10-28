package org.amistix.owlette.ui;

import org.amistix.owlette.*;
import org.amistix.owlette.i2pd.*;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import java.util.Map;
import java.util.Set;

public class EditTunnelsFragment extends Fragment {
    private DaemonWrapper daemon;
    private View rootView;

    public EditTunnelsFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        rootView = inflater.inflate(R.layout.fragment_edit_tunnels, container, false);

        daemon = DaemonWrapper.getInstance(getContext());

        Button buttonApply = rootView.findViewById(R.id.button_apply);
        EditText editField = rootView.findViewById(R.id.edit_field);

        // Apply changes to tunnels.conf
        buttonApply.setOnClickListener(v -> {
            String text = editField.getText().toString().trim();
            if (text.isEmpty()) return;
            editField.getText().clear();

            try {
                String[] parts = text.split(" ", 2);
                String[] keyParts = parts[0].split("\\.", 2);
                String tunnelName = keyParts[0];
                String propertyName = keyParts[1];
                String value = parts[1];

                TunnelsConfig.setTunnelProperty(daemon.getRootPath(), tunnelName, propertyName, value);
                Toast.makeText(getContext(), "Updated " + tunnelName, Toast.LENGTH_SHORT).show();
                refreshTunnelList(); // reload after modification
            } catch (Exception e) {
                Toast.makeText(getContext(), "Invalid format", Toast.LENGTH_SHORT).show();
            }
        });

        // Initial load
        refreshTunnelList();

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        refreshTunnelList(); // reload every time fragment becomes visible
    }

    private void refreshTunnelList() {
        if (daemon == null || rootView == null) return;

        LinearLayout recyclerClient = rootView.findViewById(R.id.recycler_client_tunnels);
        LinearLayout recyclerServer = rootView.findViewById(R.id.recycler_server_tunnels);
        recyclerClient.removeAllViews();
        recyclerServer.removeAllViews();

        Set<String> tunnels = TunnelsConfig.getTunnels(daemon.getRootPath());
        LayoutInflater inflater = LayoutInflater.from(getContext());

        for (String tunnel : tunnels) {
            Map<String, String> props = TunnelsConfig.getTunnelProperties(daemon.getRootPath(), tunnel);
            boolean isCommented = TunnelsConfig.isTunnelCommentedOut(daemon.getRootPath(), tunnel);

            // Inflate tunnel item
            View tunnelFrame = inflater.inflate(R.layout.item_tunnel_frame, recyclerClient, false);

            // Determine layout based on type
            String type = props.getOrDefault("type", "client");
            LinearLayout parentLayout = type.equalsIgnoreCase("server") ? recyclerServer : recyclerClient;

            ((TextView) tunnelFrame.findViewById(R.id.tunnel_name)).setText(tunnel);
            ((TextView) tunnelFrame.findViewById(R.id.tunnel_host))
                    .setText(props.containsKey("host") ? props.get("host") 
                        : props.getOrDefault("address", "-"));
            ((TextView) tunnelFrame.findViewById(R.id.tunnel_port))
                    .setText(props.getOrDefault("port", "-"));
            ((TextView) tunnelFrame.findViewById(R.id.tunnel_destination))
                    .setText(props.toString());

            // Switch setup
            SwitchCompat toggle = tunnelFrame.findViewById(R.id.activity_indicator);
            toggle.setOnCheckedChangeListener(null); // prevent triggering listener on setChecked
            toggle.setChecked(!isCommented);

            toggle.setOnCheckedChangeListener((button, isChecked) -> {
                if (isChecked) {
                    TunnelsConfig.uncommentTunnel(daemon.getRootPath(), tunnel);
                    Toast.makeText(getContext(), tunnel + " enabled", Toast.LENGTH_SHORT).show();
                } else {
                    TunnelsConfig.commentOutTunnel(daemon.getRootPath(), tunnel);
                    Toast.makeText(getContext(), tunnel + " disabled", Toast.LENGTH_SHORT).show();
                }
            });

            // Triple click removes tunnel
            tunnelFrame.setOnClickListener(new TripleClickListener(v -> {
                parentLayout.removeView(v);
                TunnelsConfig.removeTunnel(daemon.getRootPath(), tunnel);
                Toast.makeText(getContext(), tunnel + " removed", Toast.LENGTH_SHORT).show();
            }));

            parentLayout.addView(tunnelFrame);
        }
    }
}
