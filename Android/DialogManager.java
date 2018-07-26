package com.phase1eng.wsdlightmanager;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class DialogManager
{
    private static Dialog messageDialog;

    //Must be called whenever a new activity is opened
    public static void init(Activity activity)
    {
        messageDialog = new Dialog(activity);
        messageDialog.setContentView(R.layout.dialog);
    }

    public static void openDialog(Activity activity, String message)
    {

        closeDialog(activity);

        final TextView messageDialogText = (TextView)messageDialog.findViewById(R.id.dialogText);
        Button b = (Button)messageDialog.findViewById(R.id.button1);
        b.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                messageDialog.dismiss();
            }
        });

        final String fMessage = message;

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                messageDialogText.setText(fMessage);
                messageDialog.show();
            }
        });
    }

    public static void closeDialog(Activity activity)
    {
        if(messageDialog.isShowing()){
           activity.runOnUiThread(new Runnable(){
                @Override
                public void run(){
                    messageDialog.dismiss();
                }
            });
        }
    }
}
