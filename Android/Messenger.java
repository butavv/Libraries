package com.phase1eng.wsdlightmanager;

public class Messenger
{
    public static final int MESSAGE_RETRY_PERIOD = 500;
    public static final int MESSAGE_TIMEOUT = 2000;
    private static final int MESSENGER_STATE_IDLE = 0;
    private static final int MESSENGER_STATE_WAITING = 1;
    private static int mMessengerState = MESSENGER_STATE_IDLE;

    public static void sendMessage(final LightMessage message, final int retryPeriod, final int timeout, final boolean ackExpected, final int messageExpected)
    {
        final Thread sendThread = new Thread(new Runnable() {
            @Override
            public void run() {
                int count = 0 - retryPeriod;
                mMessengerState = MESSENGER_STATE_WAITING;
                boolean cond1, cond2 = false;
                do{
                    BLEManager.sendMessage(message.toByteArray());
                    if(ackExpected){
                        cond1 = DataHandler.waitForAck(message.header.messageId);
                    }
                    else{
                        cond1 = true;
                    }
                    if(messageExpected > 0){
                        cond2 = DataHandler.waitForMessage(messageExpected);
                    }
                    else {
                        cond2 = true;
                    }
                    count += retryPeriod;
                    if(count > timeout){
                        ActivityManager.openDialog("Light Communication Error");
                        break;
                    }
                }while(!cond1 && !cond2);

                mMessengerState = MESSENGER_STATE_IDLE;
            }
        });

        final Thread startThread = new Thread(new Runnable(){
            @Override public void run()
            {
                int count = 0 - retryPeriod;
                while(mMessengerState != MESSENGER_STATE_IDLE)
                {
                    count += retryPeriod;
                    Delay.delay(retryPeriod);
                    if(count > timeout)
                    {
                        break;
                    }
                }
                if(count < timeout)
                {
                    sendThread.start();
                }
                else{
                    ActivityManager.openDialog("Bluetooth Messages backed up");
                }
            }
        });

        startThread.start();
    }
}
