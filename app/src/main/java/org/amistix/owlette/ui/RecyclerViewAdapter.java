package org.amistix.owlette.ui;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import org.amistix.owlette.R;

import java.util.ArrayList;
import java.util.List;

public class RecyclerViewAdapter extends RecyclerView.Adapter<RecyclerViewAdapter.ViewHolder> {

    private final List<String> items = new ArrayList<>();

    public RecyclerViewAdapter() { }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public final TextView textItem;

        public ViewHolder(View itemView) {
            super(itemView);
            textItem = itemView.findViewById(R.id.widget_text);
        }
    }

    @NonNull
    @Override
    public RecyclerViewAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
            .inflate(R.layout.item_chat_message, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerViewAdapter.ViewHolder holder, int position) {
        holder.textItem.setText(items.get(position));
    }

    @Override
    public int getItemCount() {
        return items.size();
    }

    // Call this on the main/UI thread
    public void addItem(String item) {
        int pos = items.size();
        items.add(item);
        notifyItemInserted(pos);
    }

    // Optional helpers if you need to reset list or batch insert
    public void setItems(List<String> newItems) {
        items.clear();
        items.addAll(newItems);
        notifyDataSetChanged();
    }
}