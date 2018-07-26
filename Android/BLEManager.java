package com.phase1eng.wsdlightmanager;

import android.Manifest;
import android.app.Activity;
import android.app.Application;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.Toast;

import static com.phase1eng.wsdlightmanager.Constants.*;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static java.lang.Thread.sleep;


public class BLEManager extends Service
{

    private static BLEManager ourInstance = null;

    private static BluetoothAdapter mBluetoothAdapter = null;
    private static ArrayList<BluetoothDevice> mBleDeviceList = new ArrayList<BluetoothDevice>();
    private static BluetoothGatt mBluetoothGatt = null;

    private static boolean mScanning;
    private static boolean mConnecting;
    private static Handler mHandler;


    private static boolean writeFlag = false;
    //Bluetooth Packet sequences
    private static final byte STX = 0x0F;
    private static final byte ETX = 0x04;
    private static final byte DLE = 0x05;
    private static final byte CR  = 0x0D;

    private static List<Byte> packet = new ArrayList();
    private static int bytes = 0;
    private static boolean startFlag = false;
    private static boolean escapeFlag = false;
    private static boolean addToPacket = false;
    private static int checksum = 0;
    private static int checksumIndex = 0;



    //BLE UUID Definitions
    public static final UUID TX_POWER_UUID = UUID.fromString("00001804-0000-1000-8000-00805f9b34fb");
    public static final UUID TX_POWER_LEVEL_UUID = UUID.fromString("00002a07-0000-1000-8000-00805f9b34fb");
    public static final UUID CCCD = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    public static final UUID FIRMWARE_REVISON_UUID = UUID.fromString("00002a26-0000-1000-8000-00805f9b34fb");
    public static final UUID DIS_UUID = UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    public static final UUID RX_SERVICE_UUID = UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    public static final UUID RX_CHAR_UUID = UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
    public static final UUID TX_CHAR_UUID = UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e");


    public final static int ACTION_GATT_CONNECTED = 0;
    public final static int ACTION_GATT_DISCONNECTED = 1;
    public final static int ACTION_GATT_SERVICES_DISCOVERED = 2;
    public final static int ACTION_DATA_AVAILABLE = 3;
    public final static int ACTION_BLE_DEVICE_FOUND = 4;
    public final static int ACTION_NO_DEVICE_FOUND = 5;

    public final static int EXTRA_DATA = 4;
    public final static int DEVICE_DOES_NOT_SUPPORT_UART = 5;

    public static final int REQUEST_ENABLE_BT = 1;
    public static final int BLUETOOTH_TIMEOUT_SECONDS = 60;

    public static final String BLE_DEVICE_NAME = "RAM";
    public static final int BLE_CONNECTION_TIMEOUT = 10000;
    public static final int BLE_CONNECTION_RETRY_PERIOD = 5;

    //BLE States
    public static final int BLE_STATE_DISCONNECTED = 0;
    public static final int BLE_STATE_CONNECTING = 1;
    public static final int BLE_STATE_CONNECTED = 2;
    private static int mConnectionState = BLE_STATE_DISCONNECTED;

    //Stop Scanning after 5 seconds
    public static final long BLE_SCAN_PERIOD = 5000;

    public static final int BLE_SLEEP_PERIOD = 1000;
    public static final int BLE_WRITE_DELAY = 50;
    public static final int BLE_READ_DELAY = 5;

    // Message types sent from the BLEManager
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;



    //BLE GATT callback
    private static final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            setState(newState);
            if(newState == BLE_STATE_CONNECTED)
            {
                showMessage("Connected to GATT server");
                handleAction(ACTION_GATT_CONNECTED);
                gatt.discoverServices();
            }
            else if( newState == BLE_STATE_DISCONNECTED){
                showMessage("Disconnected from GATT server. Status="+status);
                handleAction(ACTION_GATT_DISCONNECTED);
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status)
        {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d("BLE", "onServicesDiscovered received Success: " + status);
                handleAction(ACTION_GATT_SERVICES_DISCOVERED);
            } else {
                Log.d("BLE", "onServicesDiscovered received: " + status);
            }

        }

        @Override
        // Result of a characteristic read operation
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d("BLE","ACTION_DATA_AVAILABLE"+characteristic);
                handleAction(ACTION_DATA_AVAILABLE, characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic){
            Log.d("BLE", "Charachteristic change = "+characteristic.getValue());
            handleAction(ACTION_DATA_AVAILABLE, characteristic);
        }
    };

    //Device scan callback function
    //Note... you can scan for BLE or BT devices but NOT both
    private static ScanCallback mLeScanCallback = new ScanCallback() {
        @Override
        public void onScanResult (int callbackType, ScanResult result) {
            if (!mBleDeviceList.contains(result.getDevice())) {
                Log.d("BLE", "Found Device! " + result.getDevice().toString());
                Log.d("BLE", "Results: " + result.toString());
                mBleDeviceList.add(result.getDevice());
            } else {
                if (DEBUG_BLE) {
                    Log.d("BLE", "Found duplicate. " + result.getDevice().toString());
                }
            }
        }
        @Override
        public void onBatchScanResults(List<ScanResult> results)
        {
            for(ScanResult result : results)
            {
                Log.d("BLE", "Found Batch Device! "+result.getDevice().getName());
                mBleDeviceList.add(result.getDevice());
            }
        }
        @Override
        public void onScanFailed(int errorCode)
        {
            Log.d("BLE","LE Scan Failed "+errorCode);
        }
    };

    //Binder
    public class LocalBinder extends Binder {
        BLEManager getService() {
            return BLEManager.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        // After using a given device, you should make sure that BluetoothGatt.close() is called
        // such that resources are cleaned up properly.  In this particular example, close() is
        // invoked when the UI is disconnected from the Service.
        close();
        return super.onUnbind(intent);
    }

    private final IBinder mBinder = new LocalBinder();

    public static void checkPermissions(Activity activity)
    {
        //Get location permission
        int permissionCheck = ContextCompat.checkSelfPermission(activity, Manifest.permission.ACCESS_FINE_LOCATION);
        if (permissionCheck != PackageManager.PERMISSION_GRANTED){
            if (ActivityCompat.shouldShowRequestPermissionRationale(activity, Manifest.permission.ACCESS_FINE_LOCATION)){
                Toast.makeText(activity, "The permission to get BLE location data is required", Toast.LENGTH_SHORT).show();
            }else{
                if(Build.VERSION.SDK_INT < 23){
                    ActivityCompat.requestPermissions(activity, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION}, 1);
                }
                else {
                    activity.requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION}, 1);
                }
            }
        }
    }

    public static void enableBluetooth(Activity activity)
    {
        BluetoothAdapter ba = BLEManager.getBluetoothAdapter(activity);
        // Ensures Bluetooth is available on the device and it is enabled. If not,
        // displays a dialog requesting user permission to enable Bluetooth.
        if (ba == null || !ba.isEnabled())
        {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            activity.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    //Gets the system bluetooth adapter... assigning it the the global variable
    public static BluetoothAdapter getBluetoothAdapter(Context context)
    {
        if(mBluetoothAdapter == null) {
            final BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
            mBluetoothAdapter = bluetoothManager.getAdapter();
        }

        return mBluetoothAdapter;
    }


    public static void startDeviceScan(final boolean enable) {
        if(mBluetoothAdapter == null)
        {
            Log.d("BLE", "Adapter is NULL!");
            return;
        }
        if(!mBleDeviceList.isEmpty()) {
            mBleDeviceList.clear();
        }
        final BluetoothLeScanner scanner = mBluetoothAdapter.getBluetoothLeScanner();
        if(scanner == null)
        {
            return;
        }
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            Thread stopThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    Delay.delay((int)BLE_SCAN_PERIOD);
                    Log.d("BLE","STOPPING BLE SCAN");
                    mScanning = false;
                    scanner.stopScan(mLeScanCallback);
                }
            });
            stopThread.start();
            mScanning = true;
            Log.d("BLE","STARTING BLE SCAN");
            scanner.startScan(mLeScanCallback);
        } else {
            mScanning = false;
            scanner.stopScan(mLeScanCallback);
        }
    }

    private static void broadcastUpdate(final int action)
    {
        final Intent intent = new Intent(Integer.toString(action));
        LocalBroadcastManager.getInstance(getBleInstance()).sendBroadcast(intent);
    }

    public static BLEManager getBleInstance() {
        if (ourInstance == null) {
            ourInstance = new BLEManager();
        }
        return ourInstance;
    }

    private static void handleAction(int action)
    {

        //Broadcast the action
        broadcastUpdate(action);

        switch(action)
        {
            case ACTION_GATT_CONNECTED:
                Delay.delay(BLE_SLEEP_PERIOD);
                break;
            case ACTION_GATT_DISCONNECTED:
                break;
            case ACTION_GATT_SERVICES_DISCOVERED:
                Delay.delay(BLE_SLEEP_PERIOD);
                enableTXNotification();
                break;
        }
    }

    private static void handleAction(int action, BluetoothGattCharacteristic characteristic)
    {
        switch(action)
        {
            case ACTION_DATA_AVAILABLE:
                try {
                    if (TX_CHAR_UUID.equals(characteristic.getUuid())) {
                        final byte[] incomingData = characteristic.getValue();
                        for (Byte incomingByte : incomingData) {
                            if (decodePacket(incomingByte) == 1 && packet.size() > 1) {

                                byte[] buffer = new byte[packet.size() - 1];
                                //Copy packet to byte array (-1 to leave out checksum)
                                for (int i = 0; i < packet.size() - 1; i++) {
                                    buffer[i] = packet.get(i);
                                }
                                //Test checksum
                                int check = ((~((byte)checksum)+1) & 255);
                                if(check == 0) {
                                    if (DEBUG_BLE) {
                                        Log.d("BLE", "Sending packet to DH");
                                    }
                                    DataHandler.getInstance().obtainMessage(MESSAGE_READ, bytes - 1, -1, buffer).sendToTarget();
                                }
                                else{
                                    Log.d("BLE ERROR", "Incorrect checksum");
                                    packet.clear();
                                    bytes = 0;
                                    checksum = 0;
                                }
                                packet.clear();
                                bytes = 0;
                                checksum = 0;
                            }
                        }

                    } else {
                        showMessage("Wrong TX UUID");
                    }
                }
                catch(Exception e)
                {
                    Log.e("BLE", "Data error", e );
                    packet.clear();
                    bytes = 0;
                    checksum = 0;
                }
        }
    }

    private static synchronized void setState(int state){
        if(mConnectionState != state) {
            mConnectionState = state;
            //DataHandler.getInstance().obtainMessage(MESSAGE_STATE_CHANGE, state, -1).sendToTarget();
        }
    }

    public static int getState()
    {
        return mConnectionState;
    }

    public static BluetoothGatt connectGatt(Context context, BluetoothDevice device)
    {
        mBluetoothGatt = device.connectGatt(context, true, mGattCallback);
        return mBluetoothGatt;
    }

    public static ArrayList<BluetoothDevice> getDeviceList()
    {
        if(mBleDeviceList.isEmpty())
        {
            Log.d("BLE", "No Devices Found");
        }
        if(mBleDeviceList == null)
        {
            Log.d("BLE", "No Devices Found. Device list is null");
            mBleDeviceList = new ArrayList<>();
        }
        return mBleDeviceList;
    }

    public static void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.d("BLE", "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.disconnect();
        // mBluetoothGatt.close();
    }


    public static void close()
    {
        if(mBluetoothGatt == null)
        {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;

        //Send the info to the activities
        //ActivityManager.broadcastAction(ACTION_GATT_DISCONNECTED);
    }

    public static boolean isScanning()
    {
        return mScanning;
    }

    public static boolean isConnecting()
    {
        return mConnecting;
    }

    public static void connectToBleDevice(Context context, final String deviceName)
    {

        mConnecting = true;
        //Scan for BLE devices
        startDeviceScan(true);
        final Context fContext = context;
        Thread connectThread = new Thread(new Runnable() {
            @Override
            public void run() {
                //Wait for scan to finish
                while(mScanning);
                Log.d("BLE", "Scan Complete");
                for(BluetoothDevice device : mBleDeviceList){
                    Log.d("BLE", "Device address = "+device.getAddress());
                    if(device.getName() != null){
                        Log.d("BLE", "NAME = "+device.getName());
                        if(device.getName().equals(deviceName))
                        {
                            Log.d("BLE","Connecting to GATT");
                            connectGatt(fContext, device);
                            break;
                        }
                    }
                }
            }
        });

        connectThread.start();
    }



    public static void searchForBleDevices(final Activity activity)
    {
        //Scan for BLE devices
        startDeviceScan(true);
        Thread searchThread = new Thread(new Runnable() {
            @Override
            public void run() {
                //Wait for scan to finish
                while(mScanning);
                Log.d("BLE", "Scan Complete");
                boolean foundDeviceFlag = false;
                for(BluetoothDevice device : mBleDeviceList){
                    Log.d("BLE", "Device address = "+device.getAddress());
                    if(device.getName() != null){
                        Log.d("BLE", "NAME = "+device.getName());
                    }
                    foundDeviceFlag = true;
                }
                if(foundDeviceFlag)
                {
                    broadcastUpdate(ACTION_BLE_DEVICE_FOUND);
                }
                else
                {
                    broadcastUpdate(ACTION_NO_DEVICE_FOUND);
                }
            }
        });

        searchThread.start();
    }

    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read result is reported
     * asynchronously through the {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.d("BLE", "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    /**
     * Enables or disables notification on a give characteristic.
     *
     */

    /**
     * Enable Notification on TX characteristic
     *
     * @return
     */
    public static void enableTXNotification()
    {
         /*
        if (mBluetoothGatt == null) {
            showMessage("mBluetoothGatt null" + mBluetoothGatt);
            broadcastUpdate(DEVICE_DOES_NOT_SUPPORT_UART);
            return;
        }
            */
        BluetoothGattService RxService = mBluetoothGatt.getService(RX_SERVICE_UUID);
        if (RxService == null) {
            showMessage("Rx service not found!");
            ActivityManager.openDialog("Bluetooth connection problem. Rx service not found.");
            return;
        }

        Delay.delay(BLE_WRITE_DELAY);

        BluetoothGattCharacteristic TxChar = RxService.getCharacteristic(TX_CHAR_UUID);
        if (TxChar == null) {
            showMessage("Tx charateristic not found!");
            ActivityManager.openDialog("Bluetooth connection problem. Tx characteristic not found.");
            return;
        }
        mBluetoothGatt.setCharacteristicNotification(TxChar,true);

        Delay.delay(BLE_WRITE_DELAY);

        BluetoothGattDescriptor descriptor = TxChar.getDescriptor(CCCD);
        descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);

        mBluetoothGatt.writeDescriptor(descriptor);

    }

    public static void writeRXCharacteristic(final byte[] value)
    {

        Thread writeRxCharachteristicThread = new Thread(new Runnable(){
            @Override
            public void run(){

                Delay.delay(BLE_WRITE_DELAY);

                if(mBluetoothGatt == null)
                {
                    return;
                }

                BluetoothGattService RxService = mBluetoothGatt.getService(RX_SERVICE_UUID);
                showMessage("mBluetoothGatt null"+ mBluetoothGatt);
                if (RxService == null) {
                    showMessage("Rx service not found!");
                    ActivityManager.openDialog("Bluetooth connection problem. Rx service not found.");
                    return;
                }

                Delay.delay(BLE_WRITE_DELAY);

                BluetoothGattCharacteristic RxChar = RxService.getCharacteristic(RX_CHAR_UUID);
                if (RxChar == null) {
                    Log.d("BLE","Rx charateristic not found!");
                    ActivityManager.openDialog("Bluetooth connection problem. Rx characteristic not found.");
                    return;
                }

                Delay.delay(BLE_WRITE_DELAY);

                RxChar.setValue(value);
                boolean status = mBluetoothGatt.writeCharacteristic(RxChar);

                Log.d("BLE", "write TXchar - status=" + status);
            }
        });

        writeRxCharachteristicThread.start();


    }

    public static void sendMessage(byte[] message)
    {
        List<Byte> packetBytes = encodePacket(message, message.length);
        byte[] packetData = new byte[packetBytes.size()];
        int j=0;
        for(Byte b : packetBytes)
        {
            packetData[j++] = b;
        }
        writeRXCharacteristic(packetData);
    }

    private static int decodePacket(byte b){

        if(DEBUG_BLE){
            Log.d("PROCESSING BYTE", Integer.toString((int)b)+" Packet Length = "+packet.size());
        }

        Delay.delay(BLE_READ_DELAY);

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
            Log.d("BLE ERROR", "Byte: "+Integer.toString((int)b)+" not handled");
        }

        return -1;
    }

    private static void showMessage(String msg)
    {
        Log.d("BLE", msg);
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

        //Carriage return for BLE uart app
        //packet.add(CR);
        //packetIndex++;

        return packet;
    }


}
