package com.example.parth.titans;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.ColorFilter;
import android.net.Uri;
import android.os.IBinder;
import android.os.ParcelUuid;
import android.support.annotation.Nullable;
import android.support.v4.app.FragmentActivity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import android.os.AsyncTask;
import org.json.*;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.HashMap;

import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.*;
import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
import android.R.color;
import java.util.List;
import java.util.Random;
import java.util.Set;
import java.util.UUID;
import android.os.Handler;


public class MainActivity extends FragmentActivity implements OnMapReadyCallback {
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;
    // ATTENTION: This was auto-generated to implement the App Indexing API.
    // See https://g.co/AppIndexing/AndroidStudio for more information.


    // Variable declarations
    private BluetoothAdapter btAdapter;
    private BluetoothManager btManager;

    private Handler handler;
    private final int SCAN_PERIOD=1000;
    private boolean scanning;
    private BluetoothDevice bleChip;
    private BluetoothLeService btLeService;
    private GoogleMap mMap;
    private GoogleMap Starting_Marker;
    private TextView connectionState;
    private TextView speedView;
    private TextView gpsView;
    private Button startButton;
    private Button connectButton;
    private boolean onceConnectedFlag;
    private boolean goingFlag;
    private ProgressBar leftProgress,rightProgress,centerProgress,rearProgress;
    private AlertDialog.Builder builder;
    private AlertDialog dialog;
    private int leftSensorVal,rightSensorVal,centerSensorVal,rearSensorVal;
    private String receivedData;
    boolean Marker_Set=false;
    boolean send_lat_long=false;
    Double LatCoord=0.000000;
    Double LonCoord=0.000000;
    Double prev_LatCoord=0.000000;
    Double prev_LngCoord=0.000000;
    private boolean UpdateLatCamLocation=false;
    private boolean UpdateLngCamLocation=false;
    //int Starting_position=0;
    LatLng SU=null;
    LatLng Destination=null;
    Marker marker;
    Marker start_marker;
    Polyline line=null;
    private ArrayList<String> Lati;
    private ArrayList<String> Longi;
    DecimalFormat decimalFormat = new DecimalFormat("#.000000");
    String sending_string = new String();
    public void onMapReady(GoogleMap googleMap) {
        gpsView.setText("0.000000,0.000000");
        mMap = googleMap;
        Starting_Marker=googleMap;
        //polyline=new PolylineOptions();
        //line=new Polyline();
        // Add a marker in Student Union and move the camera
        SU = new LatLng(37.336022, -121.881344);
        //polyline.add(SU);
        start_marker=mMap.addMarker(new MarkerOptions().position(SU).title("Starting position").icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_GREEN)));
        CameraUpdate location = CameraUpdateFactory.newLatLngZoom(SU,17);
        //mMap.animateCamera(location);

        mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener(){
            @Override
            public void onMapClick(LatLng latLng) {
                if(!Marker_Set)
                {
                    Destination=latLng;
                    marker=mMap.addMarker(new MarkerOptions().position(latLng));
                    Marker_Set=true;
                    String url1 = getMapsApiDirectionsUrl();
                    ReadTask downloadTask = new ReadTask();
                    downloadTask.execute(url1);

                }
                else
                {
                    Destination=latLng;
                    marker.remove();
                    marker=mMap.addMarker(new MarkerOptions().position(latLng));
                    String url1 = getMapsApiDirectionsUrl();
                    ReadTask downloadTask = new ReadTask();
                    downloadTask.execute(url1);

                }
            }

        });


    }

    private String getMapsApiDirectionsUrl() {
        String origin_lat = Double.toString(SU.latitude);
        String origin_long= Double.toString(SU.longitude);
        String dest_lat = Double.toString(Destination.latitude);
        String dest_long=Double.toString(Destination.longitude);
        String mode = "bicycling";
        String alternatives="false";
        String url = "https://maps.googleapis.com/maps/api/directions/json?origin="+origin_lat+","+origin_long+"&destination="+dest_lat+","+dest_long+"&mode="+mode+"&alternatives="+alternatives+"&sensor=false";
        return url;
    }



    private class ReadTask extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... url) {
            String data = "";
            try {
                HttpConnection http = new HttpConnection();
                data = http.readUrl(url[0]);
            } catch (Exception e) {
                Log.d("Background Task", e.toString());
            }
            return data;
        }


        @Override
        protected void onPostExecute(String result) {
            super.onPostExecute(result);
            new ParserTask().execute(result);
        }

    }


    private class ParserTask extends AsyncTask<String, Integer, List<List<HashMap<String, String>>>> {
        @Override
        protected List<List<HashMap<String, String>>> doInBackground(String... jsonData) {
            JSONObject jObject;
            List<List<HashMap<String, String>>> routes = null;
            try {
                jObject = new JSONObject(jsonData[0]);
                PathJSONParser parser = new PathJSONParser();
                routes = parser.parse(jObject);
            } catch (Exception e) {
                e.printStackTrace();
            }
            return routes;
        }

        @Override
        protected void onPostExecute(List<List<HashMap<String, String>>> routes) {
            Lati = new ArrayList<String>();
            Longi = new ArrayList<String>();
            ArrayList<LatLng> points = null;
            PolylineOptions polyLineOptions = null;
            // traversing through routes
            for (int i = 0; i < routes.size(); i++) {
                points = new ArrayList<LatLng>();
                polyLineOptions = new PolylineOptions();
                List<HashMap<String, String>> path = routes.get(i);

                for (int j = 0; j < path.size(); j++) {
                    HashMap<String, String> point = path.get(j);

                    double lat = Double.parseDouble(point.get("lat"));
                    double lng = Double.parseDouble(point.get("lng"));
                    LatLng position = new LatLng(lat, lng);
                    //Lati.add(decimalFormat.format(lat));
                    //Longi.add(decimalFormat.format(lng));
                    Lati.add(decimalFormat.format(lat).substring(decimalFormat.format(lat).indexOf('.')+1));
                    Longi.add(decimalFormat.format(Math.abs(lng)).substring(decimalFormat.format(Math.abs(lng)).indexOf('.')+1));
                    //Log.w(Double.toString(lat),Double.toString(lng));
                    mMap.addMarker(new MarkerOptions().position(position).title("Starting position").title("37."+Lati.get(+Lati.size()-1)+","+"-121."+Longi.get(Longi.size()-1)));
                    points.add(position);
                }
                Log.w("Waypoints="+Lati.toString(),Longi.toString());
                polyLineOptions.addAll(points);
                polyLineOptions.width(10);
                polyLineOptions.color(Color.RED);
            }
            //Log.i("Reached", "Reached");
            if(line!=null)
                line.remove();
            line=mMap.addPolyline(polyLineOptions);

        }
    }


    public void SendData()
    {
        try {
            Thread.sleep(100);
            btLeService.writeLatLong("Size");
            Thread.sleep(100);
            btLeService.writeLatLong(Integer.toString(Lati.size()+1));

        }
        catch (Exception e)
        {
            Log.w("Error",e.toString());
        }
        for(int i=0 ; i<Lati.size(); i++)
        {
            if(btLeService!=null)
            {
                try {
                    //if(i%2==0)
                    //Thread.sleep(1000);
                    //Thread.sleep(10);
                    //btLeService.writeLatLong("Latitude");
                    Thread.sleep(100);
                    //btLeService.writeLatLong("#"+Lati.get(i));
                    sending_string="#"+Lati.get(i)+"%"+Longi.get(i);
                    btLeService.writeLatLong(sending_string);
                    Log.w("Sent data=",sending_string);
                    //Thread.sleep(10);
                    //btLeService.writeLatLong("Longitude");
                    //Thread.sleep(10);
                    //btLeService.writeLatLong("%"+Longi.get(i));
                    //Log.w("Longitude=",Longi.get(i));
                }
                catch (Exception e)
                {
                    Log.w("Error:",e.toString());
                }

            }
        }
        try {
            //Thread.sleep(10);
            //btLeService.writeLatLong("Latitude");
            Thread.sleep(100);
            //btLeService.writeLatLong("#"+decimalFormat.format(Destination.latitude));
            sending_string="#"+decimalFormat.format(Destination.latitude).substring(decimalFormat.format(Destination.latitude).indexOf('.')+1);
            sending_string=sending_string+"%"+Double.toString(Math.abs(Destination.longitude)).substring(decimalFormat.format(Math.abs(Destination.longitude)).indexOf('.')+1);
            btLeService.writeLatLong(sending_string);
            Log.w("Sent data=",sending_string);
            //Log.w("Latitude",decimalFormat.format(Destination.latitude));
            //Thread.sleep(10);
            //btLeService.writeLatLong("Longitude");
            //Thread.sleep(10);
            //btLeService.writeLatLong("%"+decimalFormat.format(Math.abs(Destination.longitude)));
            //Log.w("Longitude",Double.toString(Math.abs(Destination.longitude)));
            Thread.sleep(100);
            btLeService.writeLatLong("Last");

            //send_lat_long=false;
        }
        catch (Exception e)
        {
            Log.w("Error:",e.toString());
        }


    }

    private void UpdateCamLocation(Double LatCo, Double LngCo)
    {
        Log.w("Entered","Method");
        start_marker.remove();
        SU=new LatLng(LatCo,LngCo);
        start_marker=Starting_Marker.addMarker((new MarkerOptions().position(SU).title("Current position")).icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_GREEN)));
        CameraUpdate location = CameraUpdateFactory.newLatLngZoom(SU,17);
        if(!goingFlag)
            //Starting_Marker.moveCamera(location);
            Starting_Marker.animateCamera(location);
        //else
            //Starting_Marker.moveCamera(location);
    }






/* End of Google Maps API related code*/







    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            Log.i("Parth","In Rec...");
            if(btLeService!=null){
                Log.i("Parth","In thread ...");
                if(btLeService.getConnectionState()==0) {
                    updateConnectionState("Disconnected");
                    connectButton.setText("Search");
                }
                if(btLeService.getConnectionState()==1) {
                    updateConnectionState("Connecting");
                }
                if(btLeService.getConnectionState()==2) {
                    updateConnectionState("Connected");

                    connectButton.setText("Disconnect");



                }

            }
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                Log.i("TITANS","Connected");
                try {
                    btLeService.readCustomCharacteristic();
                    Thread.sleep(100);
                    btLeService.readCustomCharacteristic();
                    Thread.sleep(100);
                    btLeService.readCustomCharacteristic();
                }catch(Exception e){

                }
                onceConnectedFlag=true;

            }else if(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {

            }else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {


                //if(send_lat_long)
                //SendData();

                receivedData = intent.getStringExtra(BluetoothLeService.EXTRA_DATA);
                if (!receivedData.equals("")) {
                    if (receivedData.contains("L")) {
                        leftSensorVal = Integer.parseInt(receivedData.substring(receivedData.indexOf('L') + 1,receivedData.indexOf('R')));
                    }  if (receivedData.contains("C")) {
                        centerSensorVal = Integer.parseInt(receivedData.substring(receivedData.indexOf('C') + 1,receivedData.indexOf('B')));
                    }  if (receivedData.contains("R")) {
                        rightSensorVal = Integer.parseInt(receivedData.substring(receivedData.indexOf('R') + 1,receivedData.indexOf('C')));
                    }  if (receivedData.contains("B")) {
                        rearSensorVal = Integer.parseInt(receivedData.substring(receivedData.indexOf('B') + 1,receivedData.indexOf('S')));
                    }  if (receivedData.contains("SPD")){
                        speedView.setText(receivedData.substring(receivedData.indexOf('D')+1)+" mph");
                    }   if(receivedData.contains("@")){
                        LatCoord=Double.parseDouble(receivedData.substring(receivedData.indexOf('@')+1,receivedData.indexOf('$')));
                        LatCoord=LatCoord/1000000;
                        //Log.w("Board Latitude=",LatCoord.toString());
                        if(LatCoord.toString().compareTo(prev_LatCoord.toString())!=0)
                        {
                            UpdateLatCamLocation=true;
                        }
                        else
                        {
                            //Log.w("Lat","False");
                            UpdateLatCamLocation=false;
                        }
                        prev_LatCoord=LatCoord;
                        Log.w(LatCoord.toString(),prev_LatCoord.toString());
                            /*if(LatCoord!=0 && Starting_position!=0)
                                Starting_position++;*/
                    }
                    if(receivedData.contains("$")){
                        LonCoord=Double.parseDouble(receivedData.substring(receivedData.indexOf('$')+1));
                        LonCoord=LonCoord/1000000;
                        //Log.w("Board Longitude=",LonCoord.toString());
                        if (LonCoord.toString().compareTo(prev_LngCoord.toString())!=0)
                        {
                            UpdateLngCamLocation=true;
                        }
                        else
                        {
                            //Log.w("Long","False");
                            UpdateLngCamLocation=false;
                        }
                        prev_LngCoord=LonCoord;
                        Log.w(LonCoord.toString(),prev_LngCoord.toString());
                    }
                        /*if(Starting_position==1)
                        {
                            start_marker.remove();
                            SU=new LatLng(LatCoord,LonCoord);
                            start_marker=Starting_Marker.addMarker((new MarkerOptions().position(SU).title("Starting position").icon(BitmapDescriptorFactory.fromResource(R.mipmap.titans))));
                            CameraUpdate location = CameraUpdateFactory.newLatLngZoom(SU,17);
                            Starting_Marker.animateCamera(location);
                        }*/
                }
                leftProgress.setProgress(leftSensorVal);
                centerProgress.setProgress(centerSensorVal);
                rightProgress.setProgress(rightSensorVal);
                rearProgress.setProgress(rearSensorVal);
                gpsView.setText(LatCoord.toString()+","+LonCoord.toString());
                if(UpdateLatCamLocation || UpdateLngCamLocation)
                {
                    UpdateCamLocation(LatCoord,LonCoord);
                }
                Log.i("TITANS/Received", receivedData);


            }

        }
    };

    private ServiceConnection btServiceConnection = new ServiceConnection(){
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            btLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!btLeService.initialize()) {
                Log.e("TITANS:", "Bluetooth Service not initialized.");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            btLeService.connect(bleChip.getAddress());
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            btLeService=null;
        }
    };



    // BLE Scanning callback which is called when device is scanning
    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {

                @Override
                public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            bleChip=device;
                            if(bleChip.getAddress().equalsIgnoreCase("00:0E:0B:0B:3E:28"))
                                if (scanning) {
                                    btAdapter.stopLeScan(mLeScanCallback);
                                    Log.i("TITANS:",bleChip.getName());
                                    Intent gattServiceIntent = new Intent(getApplicationContext(), BluetoothLeService.class);
                                    bindService(gattServiceIntent, btServiceConnection, BIND_AUTO_CREATE);
                                    connectButton.setText("Connect");
                                    scanning = false;
                                }
                        }
                    });
                }
            };


    private void scanLeDevice(final boolean enable) {
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    scanning = false;
                    btAdapter.stopLeScan(mLeScanCallback);

                }
            }, SCAN_PERIOD);

            scanning = true;
            btAdapter.startLeScan(mLeScanCallback);
        } else {
            scanning = false;
            btAdapter.stopLeScan(mLeScanCallback);
        }

    }



    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        onceConnectedFlag=false;
        receivedData="";
        setContentView(R.layout.activity_main);
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
        builder=new AlertDialog.Builder(this);
        handler=new Handler();
        leftProgress=(ProgressBar)findViewById(R.id.leftProgressBar);
        centerProgress=(ProgressBar)findViewById(R.id.centerProgressBar);
        rightProgress=(ProgressBar)findViewById(R.id.rightProgressBar);
        rearProgress=(ProgressBar)findViewById(R.id.rearProgressBar);
        leftProgress.setMax(99);
        centerProgress.setMax(99);
        rightProgress.setMax(99);
        rearProgress.setMax(99);
        goingFlag=false;
        connectionState=(TextView)findViewById(R.id.textView7);
        speedView=(TextView)findViewById(R.id.speedView);
        gpsView=(TextView)findViewById(R.id.gpsView);
        startButton=(Button)findViewById(R.id.button4);
        connectButton=(Button)findViewById(R.id.button2);


        // To check whether device supports BLE or not
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, "Device doesn't support BLE", Toast.LENGTH_SHORT).show();
            finish();
        }

        // Initialize Bluetooth Adapter
        btManager =(BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        btAdapter = btManager.getAdapter();

        // To check whether device supports Bluetooth or not
        if (btAdapter == null) {
            Toast.makeText(this, "Bluetooth is not Supported", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
    }




    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (!btAdapter.isEnabled()) {
            if (!btAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, 0);
            }
        }
        //scanLeDevice(true);


        startButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Marker_Set) {
                    if (startButton.getText().toString().equalsIgnoreCase("START")) {
                        if (btLeService != null) {
                            Log.w("Written total size:",Integer.toString(Lati.size()+1));
                            btLeService.writeStartStop("start");
                            try {
                                Thread.sleep(100);
                            }
                            catch (Exception e)
                            {

                            }
                            SendData();
                            Log.i("TITANS:", "Start Write done");
                            startButton.setText("STOP");
                            goingFlag=true;
                            send_lat_long=true;
                            //stopRead=false;
                        }
                    } else {
                        if (btLeService != null) {
                            //Starting_position=0;
                            goingFlag=false;
                            btLeService.writeStartStop("stop");
                            Log.i("TITANS:", "Stop Write done");
                            startButton.setText("START");

                        }
                    }
                }
                else{
                    builder.setMessage("Please Select Destination").setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                        }
                    });
                    dialog=builder.create();

                    dialog.show();


                }
            }
        });

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!btAdapter.isEnabled()) {
                    if (!btAdapter.isEnabled()) {
                        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(enableBtIntent, 0);
                    }
                }
                if(bleChip==null) {
                    scanLeDevice(true);
                }

                if(btLeService!=null && bleChip!=null){
                    if(btLeService.getConnectionState()==0){
                        btLeService.connect(bleChip.getAddress());
                    }
                    if(btLeService.getConnectionState()==2){
                        if(onceConnectedFlag){
                            btLeService.disconnect();
                            bleChip=null;
                        }
                    }
                }
            }
        });

    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    public Action getIndexApiAction() {
        Thing object = new Thing.Builder()
                .setName("Main Page") // TODO: Define a title for the content shown.
                // TODO: Make sure this auto-generated URL is correct.
                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
                .build();
        return new Action.Builder(Action.TYPE_VIEW)
                .setObject(object)
                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
                .build();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(onceConnectedFlag){
            unbindService(btServiceConnection);
        }
    }


    void updateConnectionState(final String msg){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                connectionState.setText(msg);
            }
        });
    }
}
