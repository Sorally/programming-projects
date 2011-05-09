package com.sorally.autohome;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;


public class ControlPanel extends Activity {
	//private EditText text;
	private Socket s;
	private OutputStream out;
	private PrintWriter output;
	private String response;
	String serverAddress = "192.168.11.2";
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (item.getTitle().equals("Kill Server")) {
            AlertDialog.Builder alertbox = new AlertDialog.Builder(this);
            alertbox.setMessage("Really?");
            alertbox.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface arg0, int arg1) {
        			die();
                    Toast.makeText(getApplicationContext(), "Server killed :O", Toast.LENGTH_SHORT).show();
                }
            });
                
            alertbox.setNegativeButton("No", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface arg0, int arg1) {
                    //Toast.makeText(getApplicationContext(), "", Toast.LENGTH_SHORT).show();
                }
            });
            alertbox.show();
		}
		return true;
	}
	

	public boolean connectTo() {
		
		ConnectivityManager connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
		NetworkInfo mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		if (mWifi.isConnected()) {
			serverAddress = "192.168.11.2";
		} else {
			serverAddress = "sorally.dyndns.org";
		}
		
		try {
      		s = new Socket(serverAddress, 27015);
	        //outgoing stream redirect to socket
	        out = s.getOutputStream();
	        output = new PrintWriter(out);
	        return true;
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			Toast.makeText(this, e.toString(), Toast.LENGTH_SHORT).show();
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Toast.makeText(this, e.toString(), Toast.LENGTH_SHORT).show();
			e.printStackTrace();
			return false;
		}
	}

	
	public void switchLight(View view) {
		
		sendMsg("switch light", 0);
	}

	public void toggleLight(View view) {
		
		sendMsg("toggle light", 0);
	}

	
	public void airconOn(View view) {

		sendMsg("airconON", 0);
	}

	public void airconOff(View view) {

		sendMsg("airconOFF", 0);
	}

	
	public void checkTemperature(View view) {
		sendMsg("temperature", 1);
	}	
	
	
	public void die() {
		
		sendMsg("die", 0);
		
	}
	public void sendMsg(String string, int getResponse) {
			
		if (!connectTo()) {
			Toast.makeText(this, "could not connect to " + serverAddress, Toast.LENGTH_SHORT).show();
			return;
		}
		output.println(string);
		output.flush();
		
		
		try {
			if (getResponse == 1) {
				BufferedReader input = new BufferedReader(new InputStreamReader(s.getInputStream()));
				response = input.readLine();
				//Toast.makeText(getApplicationContext(), response.substring(0, 2), Toast.LENGTH_SHORT).show();
				TextView t= (TextView)findViewById(R.id.textView1);
				t.setText(response + (char) 0x00B0 + "C");
				input.close();
			}

			out.close();
			output.close();
			
			out = null;
			output = null;
			
			//Close connection
	        s.close();
		} catch (UnknownHostException e) {
	    	Toast.makeText(getApplicationContext(), e.toString(), Toast.LENGTH_SHORT).show();
	    } catch (IOException e) {
	    	Toast.makeText(getApplicationContext(), e.toString(), Toast.LENGTH_SHORT).show();
	    }
	}
}