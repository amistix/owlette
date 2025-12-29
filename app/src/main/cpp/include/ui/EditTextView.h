#pragma once

#include "ui/TextView.h"
#include <jni.h>
#include <string>

namespace ui {

    class EditTextView : public TextView {
    public:
        EditTextView();  
        ~EditTextView(); 

        static EditTextView* getFocusedInstance()
        { return _focusedInstance; }

        void setFocused(bool focused);
        bool isFocused() const { return _focused; }
        
        void updateText(const std::string& text);
        void openKeyboard();
        void closeKeyboard();

        void setOnFocusedListener(std::function<void()> f);
        void onFocused();
        
        void update(float deltaTime);
        void drawSelf() override;
        
    private:
        static EditTextView* _focusedInstance;
        bool _focused = false;

        std::function<void()> _onFocusedFunc;
    };

}