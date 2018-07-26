package com.phase1eng.nick.graham2;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.BluetoothServerSocket;
import android.os.Handler;
import android.provider.SyncStateContract;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.phase1eng.nick.graham2.MotorDataHandler.MESSAGE_READ;
import static java.lang.Thread.sleep;

/**
 * Created by Nick on 12/21/2016.
 *
 * Manages the lifecycle of a bluetooth connection
 * Nested classes include:
 *      AcceptThread(): Creates a thread to connect with a device
 *      ConnectedThread(): Creates a thread to manage a bluetooth connection
 */

public class BTManager
{
    private static final int REQUEST_ENABLE_BT = 1;

    private static final String NAME = "BT";
    private static final UUID MYUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private static BluetoothSocket socket;
    private static int mState;

    private static AcceptThread mAcceptThread;
    private static ConnectThread mConnectThread;
    private static ConnectedThread mConnectedThread;

    private static BluetoothAdapter mBluetoothAdapter;

    //private static BTHandler mHandler = new BTHandler();
    //private static MotorDataHandler mHandler = MotorDataHandler.getInstance();

    // Constants that indicate the current connection state
    public static final int STATE_IDLE = 0;       // enabled but we're doing nothing
    public static final int STATE_LISTEN = 1;     // now listening for incoming connections
    public static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
    public static final int STATE_CONNECTED = 3;  // now connected to a remote device
    public static final int STATE_NOT_SUPPORTED = 4; //Device does not support BT
    public static final int STATE_DISABLED = 5;    //BT disabled on device
    public static final int STATE_NOT_PAIRED = 6;  //Serial adapter not paired
    public static final int STATE_SHUTDOWN = 7;    //BT shutdown to idle

    private static final boolean DEBUG = false;
    private static boolean writeFlag = false;
    //Bluetooth Packet sequences
    private static final byte STX = 0x0F;
    private static final byte ETX = 0x04;
    private static final byte DLE = 0x05;


    public static synchronized void startServer()
    {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if(mBluetoothAdapter == null){
            //Device does not support bluetooth
            setState(STATE_NOT_SUPPORTED);
        }

        else if(!mBluetoothAdapter.isEnabled()){
            //Bluetooth not enabled
            setState(STATE_DISABLED);
        }

        //Kill a connection thread that already exists
        if(mConnectedThread != null)
        {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        if(mAcceptThread == null)
        {
            mAcceptThread = new AcceptThread();
            mAcceptThread.start();
        }
        setState(STATE_LISTEN);
        Log.d("BT", "LISTENING");
    }

    public static synchronized void startClient()
    {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if(mBluetoothAdapter == null){
            //Device does not support bluetooth
            setState(STATE_NOT_SUPPORTED);
            return;
        }

        else if(!mBluetoothAdapter.isEnabled()){
            //Bluetooth not enabled
            setState(STATE_DISABLED);
            return;
        }

        //Kill a connection thread that already exists
        if(mConnectedThread != null)
        {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        if(mConnectThread == null){
            BluetoothDevice device = getSerialAdapter();
            if(device == null){
                setState(STATE_NOT_PAIRED);
            }
            else {
                connect(device);
            }
        }
    }

    public static BluetoothDevice getSerialAdapter(){
        setState(STATE_LISTEN);

        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        for(BluetoothDevice device : pairedDevices){
            if(device.getName().startsWith("SN") || device.getName().startsWith("FireFly")){
                Log.d("BT", "Found device"+ device.getName());
                return device;
            }
            else{
                Log.d("BT", "Serial Device not paired");
            }
        }
        return null;
    }

    /**
     * Stop all threads
     */
    public static synchronized void stop() {

        /*if (mState == STATE_CONNECTED){
            MotorDataHandler.getInstance().stopChat();
        }*/

        setState(STATE_SHUTDOWN);

        if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}
        if (mAcceptThread != null) {mAcceptThread.cancel(); mAcceptThread = null;}

        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }


    }

    public static void connectionLost(){
        Log.d("BT ERROR", "Connection was lost unexpectedly");

        if(mState != STATE_SHUTDOWN) {
            setState(STATE_IDLE);
        }
        try{
            sleep(1000);
        }
        catch (Exception e){
            Log.e("BT", "Sleep Interrupted", e);
        }
        //MotorDataHandler.getInstance().setBtStatus(STATE_IDLE);
        stop();
    }

    public static synchronized void connect(BluetoothDevice device){
        Log.d("BT", "Preparing to connect to device: "+device.getName());
        // Cancel any thread attempting to make a connection
        if (mState == STATE_CONNECTING) {
            if (mConnectThread != null) {
                mConnectThread.cancel();
                mConnectThread = null;
            }
        }

        // Cancel any thread currently running a connection
        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        // Start the thread to connect with the given device
        mConnectThread = new ConnectThread(device);
        mConnectThread.start();
        setState(STATE_CONNECTING);
    }

    public static synchronized void connected(BluetoothSocket socket, BluetoothDevice device)
    {

        // Cancel the thread that completed the connection
        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }

        // Cancel any thread currently running a connection
        if (mConnectedThread != null)
        {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        // Cancel the accept thread because we only want to connect to one device
        if (mAcceptThread != null)
        {
            mAcceptThread.cancel();
            mAcceptThread = null;
        }

        mConnectedThread = new ConnectedThread(socket);
        
        Log.d("BT", "CONNECTED to "+device.getName());

        mConnectedThread.start();

    }

    public static boolean isConnected()
    {
        if(mState == STATE_CONNECTED) {
            //Log.d("BT", "IS CONNECTED");
            return true;
        }
        else {
            return false;
        }
    }

    public static boolean isConnecting()
    {
        if(mState == STATE_CONNECTING) {
            return true;
        }
        else {
            return false;
        }
    }

    public static boolean isListening(){
        if(mState == STATE_LISTEN) {
            return true;
        }
        else {
            return false;
        }
    }

    public static int getState()
    {
        return mState;
    }

    private static synchronized void setState(int state){
        mState = state;

        MotorDataHandler.getInstance().obtainMessage(MotorDataHandler.MESSAGE_STATE_CHANGE, state, -1).sendToTarget();
    }


    /**
     * Spawns a new thread to listen for incoming connections.
     * Runs until connection is accepted or cancel is invoked
     */
    private static class AcceptThread extends Thread
    {
        private final BluetoothServerSocket mmServerSocket;


        public AcceptThread()
        {
            BluetoothServerSocket tmp = null;

                try {
                    tmp = mBluetoothAdapter.listenUsingRfcommWithServiceRecord(NAME, MYUUID);
                } catch (IOException e) {
                    setState(STATE_IDLE);
                    Log.d("BT", "IDLE");
                }

            mmServerSocket = tmp;
        }


        public void run()
        {
            socket = null;
            Log.d("BT", "Accept Thread Running "+Integer.toString(mState));
            while (mState != STATE_CONNECTED) {
                Log.d("BT", "Ready to Accept "+Integer.toString(mState));
                try {
                    Log.d("BT", "Trying to accept socket");
                    socket = mmServerSocket.accept();
                    Log.d("BT", "Connection Accepted");
                    setState(STATE_CONNECTING);
                } catch (IOException e) {
                    Log.e("BT", "Error", e);
                    break;
                }
                Log.d("BT", "Test");
                //connection accepted
                if (socket != null)
                {
                    Log.d("BT", "Socket initialized");
                    switch(mState) {
                        case STATE_LISTEN:
                            break;
                        case STATE_CONNECTING:
                            //Start the connected thread
                            Log.d("BT", "Connecting to device"+socket.getRemoteDevice().getName());
                            connected(socket, socket.getRemoteDevice());
                            try {
                                mmServerSocket.close();
                            } catch (IOException e) {}
                            break;
                        case STATE_IDLE:
                            break;
                        case STATE_CONNECTED:
                            break;
                    }
                    break;
                }
            }
        }

        public void cancel()
        {
            try{
                mmServerSocket.close();
            }catch(IOException e){}
        }
    }

    private static class ConnectThread extends Thread{
        private BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public ConnectThread(BluetoothDevice device){
            BluetoothSocket tmp = null;
            mmDevice = device;

            try{
                tmp = device.createRfcommSocketToServiceRecord(MYUUID);
                Log.d("BT", "Temp Socket Created");
            }catch(IOException e){
                Log.e("BT", "Socket Failed", e);
            }

            mmSocket = tmp;
            /*try {
                tmp.close();
                Log.d("BT", "Temp Socket closed");
            }catch(Exception e){
                Log.e("BT", "Failed closing temp socket", e);
            }*/
        }

        public void run(){
            //This speeds up connection
            mBluetoothAdapter.cancelDiscovery();

            try{
                Log.d("BT", "Establishing Bluetooth Connection");
                mmSocket.connect();
            }catch(Exception e){
                Log.e("BT ERROR","Bluetooth connection failed",e);
                try {
                    Log.d("BT CONNECT","trying fallback...");
                    mmSocket.close();
                    mmSocket =(BluetoothSocket) mmDevice.getClass().getMethod("createRfcommSocket", new Class[] {int.class}).invoke(mmDevice,1);
                    mmSocket.connect();

                    Log.d("BT","Connected");
                }
                catch (Exception e2) {
                    Log.e("BT", "Bluetooth Connection failed again!", e2);

                    try{
                        Log.d("BT CONNECT", "Trying with Insecure Socket");
                        mmSocket.close();
                        mmSocket =(BluetoothSocket) mmDevice.getClass().getMethod("createInsecureRfcommSocket", new Class[] {int.class}).invoke(mmDevice,1);
                        mmSocket.connect();

                    }catch(Exception e3){
                        Log.e("BT ERROR", "Could not establish connection... Giving up ", e3);
                        try {
                            Log.d("BT", "Closing the socket");
                            mmSocket.close();
                        } catch (IOException closeException) {
                            Log.e("BT", "Could not close the client socket", closeException);
                        }
                        setState(STATE_IDLE);
                        return;
                    }
                }
            }
            mConnectThread = null;
            connected(mmSocket, mmDevice);
        }

        // Closes the client socket and causes the thread to finish.
        public void cancel() {
            try {
                Log.d("BT", "Closing connection");
                mmSocket.close();
            } catch (IOException e) {
                Log.e("BT", "Could not close the client socket", e);
            }
        }
    }


    private static class ConnectedThread extends Thread
    {
        private BufferedInputStream mmInStream;
        private OutputStream mmOutStream;
        private final BluetoothSocket mmSocket;
        private List<Byte> packet;
        private static int bytes;
        private boolean startFlag = false;
        private boolean escapeFlag = false;
        private boolean addToPacket = false;
        private int checksum = 0;
        private int checksumIndex = 0;

        volatile boolean running = true;

        public ConnectedThread(BluetoothSocket socket)
        {
            //this.setPriority(Thread.MAX_PRIORITY);
            mmSocket = socket;
            BufferedInputStream tmpIn = null;
            OutputStream tmpOut = null;

            packet = new ArrayList<>();

            try{
                tmpIn = new BufferedInputStream( socket.getInputStream() );
                tmpOut = socket.getOutputStream();
            }catch(IOException e){
                Log.e("BT", "Temp sockets not created", e);
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run()
        {
            //setState(STATE_CONNECTED);
            Log.d("BT", "BEGIN CONNECTION THREAD");
            //mHandler.startChat();

            int incomingInt =0;
            //byte incomingByte;

            setState(STATE_CONNECTED);
            byte[] inputData = new byte[1024];
            int retries = 5;

            while (mState == STATE_CONNECTED)
            {

                if(!running){
                    Log.d("BT ERROR", "Not Running");
                    return;
                }

                if(!mmSocket.isConnected()){
                    Log.d("BT ERROR", "Tried to read from disconnected stream");
                    connectionLost();
                    break;
                }

                try {
                    int bytesToRead = 0;
                    int to = 1000;
                    int count = 0;
                    while((bytesToRead = mmInStream.available()) <= 0){
                        count++;
                        if(!mmSocket.isConnected()){
                            Log.d("BT ERROR", "Tried to read from disconnected stream");
                            connectionLost();
                            break;
                        }
                        if(bytesToRead < 0 || bytesToRead > 1024){
                            Log.d("BT", "Invalid read length " + bytesToRead);
                        }
                        if(count > to){
                            Log.d("BT", "Read exceeded blocking timeout");
                            try {
                                Log.d("BT", "Re-attempting connection. Retries left = "+retries);
                                mmSocket.connect();
                                if(!mmSocket.isConnected()){
                                    retries--;
                                    Log.d("BT", "Socket not connected");
                                }
                            }catch(Exception e){
                                retries--;
                                Log.e("BT", "Connection Failed", e);
                            }
                            if(retries == 0) {
                                connectionLost();
                            }
                            break;
                        }
                        try {
                            sleep(1);
                        }
                        catch(Exception e){
                            Log.e("BT", "Sleep interrupted", e);
                        }
                    }

                    if(bytesToRead > 1024){
                        Log.d("BT ERROR", "Too many bytes to read "+bytesToRead);
                    }

                    //Log.d("BT", "Bytes Available = "+bytesToRead);
                    int bytesRead = mmInStream.read(inputData, 0, 1024);
                    if(bytesRead ==  -1){
                        Log.d("BT ERROR", "EOF, No Data read");
                        continue;
                    }
                    //while((incomingInt = mmInStream.read()) != -1){
                    for(int j=0; j<bytesRead; j++){
                        byte incomingByte = inputData[j];
                        //Log.d("Byte", Integer.toString(incomingInt));
                        //incomingByte = (byte)incomingInt;


                        if(!running){return;}

                        if(decodePacket(incomingByte) == 1 && packet.size()>1){
                            if(DEBUG){
                                Log.d("BT", "Found a valid packet");
                            }
                            byte[] buffer = new byte[packet.size()-1];
                            //Copy packet to byte array (-1 to leave out checksum)
                            for(int i=0; i<packet.size()-1; i++) {
                                buffer[i] = packet.get(i);
                            }
                            //Test checksum
                            int check = ((~((byte)checksum)+1) & 255);
                            if(check == 0) {
                                if(DEBUG){
                                    Log.d("BT", "Sending packet to MDH");
                                }
                                MotorDataHandler.getInstance().obtainMessage(MESSAGE_READ, bytes - 1, -1, buffer).sendToTarget();
                            }
                            else{
                                Log.d("BT ERROR", "Incorrect checksum");
                            }
                            packet.clear();
                            bytes = 0;
                            checksum = 0;
                        }
                    }

                } catch (IOException e) {
                    Log.d("BT ERROR", "Exception during read ", e);
                }
            }
            Log.d("BT", "NOT CONNECTED... State = "+mState);
        }

        /**
         * Write to the connected OutStream.
         * @param buffer  The bytes to write
         */
        public void write(byte[] buffer)
        {
            writeFlag = true;
            try {
                //for(byte b : buffer){Log.d("OUTGOING BYTE", Integer.toHexString(b));}
                if(mmSocket.isConnected()) {
                    //Log.d("BTM", "Writing to output stream");
                    if(DEBUG){
                        Log.d("BT OUT", ""+buffer);
                    }
                    mmOutStream.write(buffer);
                    //Log.d("BTM", "Success writing to output stream");
                }
                else{
                    Log.d("BT ERROR", "Tried to write to closed socket");
                    BTManager.stop();
                }
            } catch (IOException e) {
                Log.e("BT ERROR", "Exception during write", e);

            }
            writeFlag = false;
        }


        private int decodePacket(byte b){

            if(DEBUG){
                Log.d("PROCESSING BYTE", Integer.toString((int)b));
            }

            //Check for Escape sequence
            if(b == DLE && !escapeFlag){
                escapeFlag = true;
                startFlag = false;
            }

            //Find first start sequence
            else if(b == STX && !escapeFlag && !startFlag){
                startFlag = true;
            }

            //Find second start sequence
            else if(b == STX && !escapeFlag && startFlag){
                addToPacket = true;
                startFlag = false;
            }

            //Check for end sequence
            else if(b == ETX && !escapeFlag){
                startFlag = false;
                return 1;
            }

            //Check for valid byre
            else if(addToPacket){
                startFlag = false;
                escapeFlag = false;
                packet.add(b);
                checksum += b;
                bytes++;
            }
            else{
                Log.d("BT ERROR", "Byte: "+Integer.toString((int)b)+" not handled");
            }
            
            return -1;
        }
       

        public void cancel() {
            Log.d("BT", "Cancled");
            running = false;
            try {
                mmSocket.close();
            } catch (IOException e) {}
        }

    }


    public static void sendMessage(byte[] message){
        List<Byte> packet = encodePacket(message, message.length);
        byte[] packetData = new byte[packet.size()];
        int j=0;
        for(Byte b : packet){
            packetData[j++] = b;
        }
        int count = 0;
        int to = 5000;

        if(getState() == STATE_CONNECTED) {
            //Log.d("BTM", "Sending Message");
            final byte[] pd = packetData;
            Thread t = new Thread(new Runnable(){
                @Override
                public void run(){
                    if(mConnectedThread != null) {
                        mConnectedThread.write(pd);
                    }
                }
            });
            t.start();
            while(writeFlag) {
                count++;
                if(count > to){
                    Log.d("BT ERROR", "Write exceeded blocking timeout");
                    //connectionLost();
                    return;
                }
                try{
                    sleep(1);
                }catch(Exception e){
                    Log.e("BT ERROR", "Sleep Interrupted", e);
                }
            }
        }
        else{
            Log.d("BT ERROR", "Tried to write to disconnected stream");
        }
    }

     private static List<Byte> encodePacket(byte[] data, int count)
        {
            List<Byte> packet = new ArrayList<>();
            int packetCount;
            byte tempByte;
            int checksum = 0;
            int packetIndex = 2;

            if(count == 0){
                return packet;
            }

            //start condition
            packet.add(STX);
            packet.add(STX);

            //build packet
            for(packetCount = 0; packetCount < count; packetCount++) {
                switch (data[packetCount]) {
                    case STX:
                    case ETX:
                    case DLE: {
                        packet.add(DLE);
                        packetIndex++;
                        break;
                    }
                }

                packet.add(data[packetCount]);
                checksum += (int) packet.get(packetIndex);
                packetIndex++;
            }

            //insert checksum
            tempByte = (byte)(((~(checksum))+1)&255);
            switch (tempByte) {
                case STX:
                case ETX:
                case DLE: {
                    packet.add(DLE);
                    packetIndex++;
                    break;
                }
            }

            packet.add(tempByte);
            packetIndex++;

            //stop condition
            packet.add(ETX);
            packetIndex++;

            return packet;
        }
}