package com.example.parth.titans;

import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    boolean btConnected=false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final ImageButton bluetooth=(ImageButton)findViewById(R.id.imageButton3);
        bluetooth.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!btConnected) {
                    BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
                    if (btAdapter == null) {
                        Toast.makeText(getApplicationContext(), "Bluetooth is not supported", Toast.LENGTH_SHORT);
                    }
                    if (!btAdapter.isEnabled()) {
                        Intent enableBt = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(enableBt, 1);
                    }
                    btConnected = true;
                }
                else
                {
                    Toast.makeText(getApplicationContext(), "Bluetooth is on", Toast.LENGTH_SHORT);
                }
            }
        });
    }
}
