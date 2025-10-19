package org.amistix.owlette.ui;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;
import org.amistix.owlette.R;

public class RecyclerViewAdapter extends ListAdapter<String, RecyclerViewAdapter.ViewHolder> {

    public RecyclerViewAdapter() {
        super(DIFF_CALLBACK);
    }

    private static final DiffUtil.ItemCallback<String> DIFF_CALLBACK =
            new DiffUtil.ItemCallback<String>() {
                @Override
                public boolean areItemsTheSame(@NonNull String oldItem, @NonNull String newItem) {
                    // If you have unique IDs for messages use them here.
                    return oldItem.equals(newItem);
                }

                @Override
                public boolean areContentsTheSame(@NonNull String oldItem, @NonNull String newItem) {
                    return oldItem.equals(newItem);
                }
            };

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
        holder.textItem.setText(getItem(position));
    }

    // Helper to append a message (keeps use of ListAdapter)
    public void addItem(String item) {
        // Make a new list based on current list to submit - ListAdapter expects immutable lists
        java.util.List<String> newList = new java.util.ArrayList<>(getCurrentList());
        newList.add(item);
        submitList(newList);
    }
}