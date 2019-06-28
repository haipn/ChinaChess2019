package vn.singviet.cotuong2019;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class SplashActivity extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.splash_activity);
		Timer time = new Timer();
		time.schedule(new TimerTask() {
			
			@Override
			public void run() {
				startActivity(new Intent(SplashActivity.this, MainActivity.class));
			}
		}, 2000);
	}
}
