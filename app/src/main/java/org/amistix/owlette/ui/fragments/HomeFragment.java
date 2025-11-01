package org.amistix.owlette.ui;

import org.amistix.owlette.*;
import org.amistix.owlette.i2pd.*;
import org.amistix.owlette.network.*;

import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.LinearLayout;

import android.widget.Toast;
import android.os.Bundle;
import java.util.Set;
import java.util.Map;

import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;

public class HomeFragment extends Fragment {

    private DaemonWrapper daemon;
    private View rootView;
    public HomeFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        rootView = inflater.inflate(R.layout.fragment_home, container, false);
        daemon = DaemonWrapper.getInstance(getContext());
        LinearLayout contactsFrame = rootView.findViewById(R.id.contacts_frame);
        contactsFrame.removeAllViews();
        Set<String> tunnels = TunnelsConfig.getTunnels(daemon.getRootPath());
        for (String tunnel : tunnels) {
            Map<String, String> props = TunnelsConfig.getTunnelProperties(daemon.getRootPath(), tunnel);

            View contactFrame = inflater.inflate(R.layout.item_contact_frame, contactsFrame, false);

            if (!props.containsKey("type")) continue;
            if (!props.get("type").equals("client")) continue;

            ((TextView) contactFrame.findViewById(R.id.client_tunnel_name))
                .setText(tunnel);

            String contactName = props.getOrDefault("name", "Undefined");
            ((TextView) contactFrame.findViewById(R.id.contact_name))
                .setText(contactName);

            contactFrame.setOnClickListener(v -> {
                TcpClient client = TcpClient.getInstance();
                client.setClientHost(props.get("address"), Integer.parseInt(props.get("port")));
                Toast.makeText(getContext(), "Loading "+ props.get("destination"), Toast.LENGTH_SHORT).show();
                NavHostFragment.findNavController(this).navigate(R.id.nav_channel);
            });

            contactsFrame.addView(contactFrame);

        }
        return rootView;
    }
}
