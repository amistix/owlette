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

import android.os.Bundle;

public class EditTunnelsFragment extends Fragment {
    private DaemonWrapper daemon;

    public EditTunnelsFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_edit_tunnels, container, false);
        LinearLayout recyclerView = rootView.findViewById(R.id.recycler_client_tunnels);

        daemon = DaemonWrapper.getInstance(getContext());
        for (String tunnel : TunnelsConfig.getTunnels(daemon.getRootPath())) {
            TextView textView = new TextView(getContext());
            textView.setTextSize(16);
            textView.setTextColor(android.graphics.Color.BLACK); 
            textView.setText(tunnel);

            recyclerView.addView(textView);
            recyclerView.invalidate();
        }
        LinearLayout recyclerView1 = rootView.findViewById(R.id.recycler_server_tunnels);
        for (int i=0; i < 100; i++)
        {
            TextView textView = new TextView(getContext());
            textView.setTextSize(16);
            textView.setText("hi");

            recyclerView1.addView(textView);
            recyclerView1.invalidate();
        }
        


        return rootView;
    }
}
