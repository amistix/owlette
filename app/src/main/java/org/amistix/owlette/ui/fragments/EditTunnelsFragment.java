package org.amistix.owlette.ui;

import org.amistix.owlette.*;

import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import androidx.recyclerview.widget.LinearLayoutManager;

import org.amistix.owlette.i2pd.*;

import java.util.*;
import android.os.Bundle;

public class EditTunnelsFragment extends Fragment {
    private DaemonWrapper daemon;
    private View tunnelFrame; 
    public EditTunnelsFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_edit_tunnels, container, false);
        LinearLayout recyclerViewClient = rootView.findViewById(R.id.recycler_client_tunnels);
        LinearLayout recyclerViewServer = rootView.findViewById(R.id.recycler_server_tunnels);

        daemon = DaemonWrapper.getInstance(getContext());
        LinearLayout layoutToAddFrame = null;

        for (String tunnel : TunnelsConfig.getTunnels(daemon.getRootPath())) {
            Map<String, String> tunnelProperties = TunnelsConfig.getTunnelProperties(daemon.getRootPath(), tunnel);

            tunnelFrame = LayoutInflater.from(getContext())
                .inflate(R.layout.item_tunnel_frame, container, false);
            
            if (tunnelProperties.containsKey("type")){
                switch(tunnelProperties.get("type")){
                    case "server":
                        layoutToAddFrame = recyclerViewServer;
                        break;
                    case "client":
                        layoutToAddFrame = recyclerViewClient;
                        break;
                }
            }

            if (!tunnel.isEmpty()){
                ((TextView) tunnelFrame.findViewById(R.id.tunnel_name))
                    .setText(tunnel);
            }

            if (tunnelProperties.containsKey("host")){
                ((TextView) tunnelFrame.findViewById(R.id.tunnel_host))
                    .setText(tunnelProperties.get("host"));
            }

            if (tunnelProperties.containsKey("port")){
                ((TextView) tunnelFrame.findViewById(R.id.tunnel_port))
                    .setText(tunnelProperties.get("port"));
            }

            if (tunnelProperties.containsKey("keys")){
                try {
                    String base64Destination = TunnelsConfig.getTunnelBase32(tunnel);
                    ((TextView) tunnelFrame.findViewById(R.id.tunnel_destination))
                        .setText(tunnelProperties.toString());
                }
                catch (Exception e) {
                    ((TextView) tunnelFrame.findViewById(R.id.tunnel_destination))
                        .setText(tunnelProperties.toString());
                }
            }

            if (layoutToAddFrame != null){
                final ViewGroup layoutReference = layoutToAddFrame;
                tunnelFrame.setOnClickListener(new TripleClickListener(v -> {
                    layoutReference.removeView(v);
                }));
                layoutReference.addView(tunnelFrame);
                layoutReference.invalidate();
            }
        }
        return rootView;
    }
}
