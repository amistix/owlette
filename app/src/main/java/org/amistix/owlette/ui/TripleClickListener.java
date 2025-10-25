package org.amistix.owlette.ui;
import android.view.View;
import android.animation.ObjectAnimator;
import android.animation.Animator;
import android.os.Handler;

public class TripleClickListener implements View.OnClickListener {

    private static final int MAX_CLICKS = 3;
    private static final long CLICK_INTERVAL = 500;

    private int clickCount = 0;
    private long lastClickTime = 0;
    private ViewAction onTripleClickAction;

    public TripleClickListener(ViewAction onTripleClickAction) {
        this.onTripleClickAction = onTripleClickAction;
    }

    @Override
    public void onClick(View v) {
        long currentTime = System.currentTimeMillis();

        if (currentTime - lastClickTime <= CLICK_INTERVAL) {
            clickCount++;
        } else {
            clickCount = 1; 
        }

        lastClickTime = currentTime;

        flashView(v);

        if (clickCount == MAX_CLICKS) {
            if (onTripleClickAction != null) {
                onTripleClickAction.run(v);
            }
            clickCount = 0; 
        }
    }

    private void flashView(View v) {
        float originalAlpha = v.getAlpha();

        ObjectAnimator flashAnimator = ObjectAnimator.ofFloat(v, "alpha", originalAlpha, 0.3f); // Darken the view
        flashAnimator.setDuration(100); // Duration of the "flash" effect (100ms)
        flashAnimator.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                ObjectAnimator revertAnimator = ObjectAnimator.ofFloat(v, "alpha", 0.3f, originalAlpha);
                revertAnimator.setDuration(100); 
                revertAnimator.start();
            }

            @Override
            public void onAnimationCancel(Animator animation) {
            }

            @Override
            public void onAnimationRepeat(Animator animation) {
            }
        });

        flashAnimator.start(); // Start the flash animation
    }

    // Functional interface for the action
    public interface ViewAction {
        void run(View v);
    }
}


