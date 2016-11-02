package com.example.parth.titans;

import android.app.Activity;
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
import android.widget.TextView;
import android.widget.Toast;
import android.os.AsyncTask;
import org.json.*;

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
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
import android.R.color;
import java.util.List;
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
    private  boolean scanning;
    private BluetoothDevice bleChip;
    private BluetoothLeService btLeService;
    private GoogleMap mMap;
    private TextView connectionState;
    private Button startButton;
    private boolean connectedFlag;

    boolean Marker_Set=false;
    LatLng SU=null;
    LatLng Destination=null;
    Marker marker;
    Polyline line=null;
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        //polyline=new PolylineOptions();
        //line=new Polyline();
        // Add a marker in Student Union and move the camera
        SU = new LatLng(37.336022, -121.881344);
        //polyline.add(SU);
        mMap.addMarker(new MarkerOptions().position(SU).title("Starting position"));
        CameraUpdate location = CameraUpdateFactory.newLatLngZoom(SU,17);
        mMap.animateCamera(location);

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
        String mode = "walking";
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

                   points.add(position);
               }

               polyLineOptions.addAll(points);
               polyLineOptions.width(10);
               polyLineOptions.color(Color.RED);
           }
           Log.i("Reached", "Reached");
            if(line!=null)
                line.remove();
           line=mMap.addPolyline(polyLineOptions);


       }
   }









/* End of Google Maps API related code*/







        private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                Log.i("TITANS","Connected");
                connectionState.setText("Connected");
                connectedFlag=true;
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                connectionState.setText("Not Connected");
                connectedFlag=false;
                while (!connectedFlag) {
                    try {
                        wait(2000);
                    } catch (Exception e) {

                    }
                    scanLeDevice(true);
                }
                invalidateOptionsMenu();

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
                    invalidateOptionsMenu();
                }
            }, SCAN_PERIOD);

            scanning = true;
            btAdapter.startLeScan(mLeScanCallback);
        } else {
            scanning = false;
            btAdapter.stopLeScan(mLeScanCallback);
        }
        invalidateOptionsMenu();
    }



    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        connectedFlag=false;
        setContentView(R.layout.activity_main);
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
        connectionState=(TextView)findViewById(R.id.textView7);
        startButton=(Button)findViewById(R.id.button4);
        handler=new Handler();
        new Thread(new Runnable() {
            public void run() {
                Log.i("TitansThread:","thread running");
                if(btLeService!=null) {
                    btLeService.readCustomCharacteristic();
                }
            }
        }).start();

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
        scanLeDevice(true);


        startButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(startButton.getText().toString().equalsIgnoreCase("START")){
                    if(btLeService != null) {
                        btLeService.writeCustomCharacteristic(1);
                        Log.i("TITANS:","Write done");
                        startButton.setText("STOP");
                    }
                }
                else{
                    if(btLeService != null) {
                        btLeService.writeCustomCharacteristic(0);
                        Log.i("TITANS:","Write done");
                        startButton.setText("START");
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
    public void onStart() {
        super.onStart();

      
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        AppIndex.AppIndexApi.start(client, getIndexApiAction());
    }

    @Override
    public void onStop() {
        super.onStop();
        if(connectedFlag){
            unbindService(btServiceConnection);
        }
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        AppIndex.AppIndexApi.end(client, getIndexApiAction());
        client.disconnect();
    }


}
