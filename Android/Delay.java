package com.phase1eng.wsdlightmanager;
import android.util.Log;

import static java.lang.Thread.sleep;

public class Delay
{
    public static void delay(int ms)
    {
        try{
            sleep(ms);
        }catch(Exception e){
            Log.e("DELAY", "Sleep Interrupted");
        }
    }
}
