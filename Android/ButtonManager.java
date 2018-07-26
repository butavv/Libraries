package com.phase1eng.wsdlightmanager;

import android.graphics.Color;
import android.widget.Button;

public class ButtonManager
{
    public static void enableButton(final Button b)
    {
        ActivityManager.getActivity().runOnUiThread(new Runnable(){
            @Override
            public void run() {
                b.setEnabled(true);
                b.setBackgroundColor(Color.LTGRAY);
            }
        });
    }
    public static void disableButton(final Button b)
    {
        ActivityManager.getActivity().runOnUiThread(new Runnable(){
            @Override
            public void run() {
                b.setEnabled(false);
                b.setBackgroundColor(Color.GRAY);
            }
        });
    }


}
