package org.amistix.owlette.ui;

import org.amistix.owlette.*;

import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.recyclerview.widget.RecyclerView;
import org.amistix.owlette.ui.RecyclerViewAdapter;
import androidx.recyclerview.widget.LinearLayoutManager;

import android.os.Bundle;

public class EditTunnelsFragment extends Fragment {

    private RecyclerViewAdapter adapter1;
    private RecyclerViewAdapter adapter2;
    public EditTunnelsFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_edit_tunnels, container, false);
        RecyclerView recyclerView = rootView.findViewById(R.id.recycler_client_tunnels);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        adapter1 = new RecyclerViewAdapter();
        adapter2 = new RecyclerViewAdapter();
        recyclerView.setAdapter(adapter1);
        adapter1.addItem("Hsdfsfsfi");
        adapter1.addItem("Heloosdfsf");
        RecyclerView recyclerView1 = rootView.findViewById(R.id.recycler_server_tunnels);
        recyclerView1.setLayoutManager(new LinearLayoutManager(getContext()));
        recyclerView1.setAdapter(adapter2);
        adapter2.addItem("Hihi");
        adapter2.addItem("Helghoo");

        return rootView;
    }
}
