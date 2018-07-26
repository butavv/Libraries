package com.phase1eng.wsdlightmanager;


import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import java.util.ArrayList;

import static com.phase1eng.wsdlightmanager.Constants.*;
import static java.lang.Thread.sleep;

import static com.phase1eng.wsdlightmanager.BLEManager.*;

public class DataHandler extends Handler {

    private static DataHandler ourInstance = null;


    private static boolean ackReceived = false;
    private static int ackMsgId = -1;
    private static boolean waitingForAck = false;

    private static boolean waitingForMessage = false;
    private static int waitingMessageId = -1;
    private static int messageRcvdId = -2;

    public static final int ACK_TIMEOUT = 500;
    public static final int ACK_RETRY_PERIOD = 1;

    //Messages from the light
    public static final int LIGHT_MSG_ACK = 100;

    //Serial Numbers
    public static final int BROADCAST_SN = 0xFFFFFFFF;
    public static final int APP_SN = 0;
    public static final int RAM_SN = 1;



    private DataHandler() {
    }

    public static DataHandler getInstance() {
        if (ourInstance == null) {
            ourInstance = new DataHandler();
        }
        return ourInstance;
    }

    @Override
    public void handleMessage(Message msg)
    {
        switch(msg.what)
        {
            case MESSAGE_STATE_CHANGE:
                switch (msg.arg1)
                {
                    case BLE_STATE_CONNECTED:
                        Log.d("BLE STATE CHANGE", "CONNECTED");
                        break;
                    case BLE_STATE_CONNECTING:
                        Log.d("BLE STATE CHANGE", "CONNECTING");
                        break;
                    case BLE_STATE_DISCONNECTED:
                        Log.d("BLE STATE CHANGE", "DISCONNECTED");
                        break;
                }
                break;
            case MESSAGE_WRITE:
                break;
            case MESSAGE_READ:

                //Get the message
                byte[] bytesIn = (byte[]) msg.obj;

                int x = 0;
                //Remove first byte ("\r") if it is there
               /* if(bytesIn[0] == 13){
                    x = 1;
                }*/
                byte[] incomingPacket = new byte[bytesIn.length - x];

                for(int i=0; i<incomingPacket.length; i++)
                {
                    incomingPacket[i] = bytesIn[i+x];
                }


                //Filter messages with incomplete header
                if(incomingPacket.length < HEADER_LENGTH){
                    Log.d("DH", "Received a message that was too short");
                    if(DEBUG_DH) {
                        printPacket(incomingPacket);
                    }
                    break;
                }

                //Filter messages that are too long
                if(incomingPacket.length > MAX_PACKET_SIZE)
                {
                    Log.d("DH", "Received a message that was too long");
                    if(DEBUG_DH) {
                        printPacket(incomingPacket);
                    }
                    break;
                }

                LightMessage message = new LightMessage(incomingPacket);

                //Check for nulls in message formation
                if(message.header == null)
                {
                    Log.d("DH", "Ignoring message with no header");
                    return;
                }

                if(message.header.dstSn == null)
                {
                    Log.d("DH", "Ignoring message with no destination");
                    return;
                }

                //Filter message not intended for the app
                if(message.header.dstSn.snIntVal != APP_SN ){
                    Log.d("DH", "Received a message not intended for the app");
                    if(DEBUG_DH) {
                        printPacket(incomingPacket);
                    }
                    break;
                }

                if(waitingForMessage)
                {
                    messageRcvdId = message.header.messageId;
                }


                switch(message.header.messageId)
                {
                    case LIGHT_MSG_ACK:
                        handleAckMessage(message);
                        break;
                }

                if(DEBUG_DH)
                {
                    printPacket(incomingPacket);
                }



                break;
            case MESSAGE_DEVICE_NAME:
                break;
        }
    }


    private void handleAckMessage(LightMessage message)
    {
        if(waitingForAck) {
            ackMsgId = message.getData(0);
            ackReceived = true;

            Log.d("DH","Received Ack");
        }
        else{
            Log.d("DH", "Received Unsolicited Ack");
        }


    }


    public static boolean waitForMessage(int messageId)
    {
        Log.d("DH", "Waiting for message "+messageId);
        waitingForMessage = true;
        waitingMessageId = messageId;
        int count = 0;
        while( (waitingMessageId != messageRcvdId) )
        {
            Delay.delay(ACK_RETRY_PERIOD);
            count += ACK_RETRY_PERIOD;
            if(count > ACK_TIMEOUT){
                Log.d("DH", "Message "+messageId+" not received");
                waitingForMessage = false;
                messageRcvdId = -2;
                waitingMessageId = -1;
                return false;
            }
        }
        Delay.delay(100);
        waitingForMessage = false;
        messageRcvdId = -2;
        waitingMessageId = -1;
        Log.d("DH", "Message Received");
        return true;
    }

    public static boolean waitForAck(int ackId)
    {
        int count = 0;
        waitingForAck = true;
        if(DEBUG_DH) {
            Log.d("DH", "Looking for ACK = "+ackReceived+" AckMsgId="+ackMsgId);
        }
        int to = ACK_TIMEOUT;
        while( (!ackReceived) ){
            Delay.delay(ACK_RETRY_PERIOD);
            count += ACK_RETRY_PERIOD;
            if(DEBUG_DH) {
                Log.d("DH", "Waiting for ACK " + count);
            }
            if(ackReceived)
            {
                break;
            }
            if(count > to)
            {
                break;
            }
        }

        waitingForAck = false;
        ackReceived = false;

        if(count > to)
        {
            Log.d("DH", "ACK timeout expired!");
            ackMsgId = -1;
            return false;
        }
        else if(ackMsgId != ackId){
            Log.d("DH", "Wrong Ack ID!");
            ackMsgId = -1;
            return false;
        }
        else{
            if(DEBUG_DH) {
                Log.d("DH", "ACK Receieved = "+ackReceived+" AckMsgId="+ackMsgId);
            }
            ackMsgId = -1;
            return true;
        }
    }


    public static void printPacket(byte[] message){
        Log.d("DH", "Printing Packet");
        int i = 0;
        for(byte b : message){
            Log.d("DH", "Byte "+i+" "+Integer.toString((int)b));
            i++;
        }
    }




}