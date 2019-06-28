package vn.singviet.cotuong2019;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;

import vn.singviet.cotuong2019.engine.AIGameDialog;
import vn.singviet.cotuong2019.engine.ChessLayout;
import vn.singviet.cotuong2019.engine.Engine;
import vn.singviet.cotuong2019.engine.GameListener;
import vn.singviet.cotuong2019.engine.IChessEventListener;
import vn.singviet.cotuong2019.engine.NetGameDialog;
import com.google.android.gms.ads.MobileAds;


public class MainActivity extends Activity implements GameListener,
		IChessEventListener, ChessLayout.NewGameListener {
	public void onReturn(boolean needAsk) {
		returnMain(needAsk);
	}

	// Chess board
	private ChessLayout gameLayout = null;
	private View homeMenuLayout = null;
	private View currentView = null;
	private NetGameDialog netDialog = null;
	private MenuItem soundItem = null;
	// private MenuItem brightnessItem = null;
	private boolean nightMode = false;
	private int oldAutoBrightness = Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
	private int oldBrightness = 255;
	AdView adView;

	InterstitialAd interstitial;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// Set screen should always portrait.
		// this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		// getWindow().requestFeature(Window.FEATURE_ACTION_BAR);
		// getActionBar().hide();
		setContentView(R.layout.main_game);
		gameLayout = (ChessLayout) findViewById(R.id.mainLayout);
		gameLayout.setNewGameListener(this);
		gameLayout.setChessListener(this);

		setContentView(R.layout.activity_main);
		homeMenuLayout = findViewById(R.id.main_menu);
		currentView = homeMenuLayout;
		// Sample AdMob app ID: ca-app-pub-3940256099942544~3347511713
		MobileAds.initialize(this, getString(R.string.app_mod_id));

		adView = currentView.findViewById(R.id.adView);
		AdRequest request = new AdRequest.Builder().build();
		adView.loadAd(request);

		interstitial = new InterstitialAd(this);
		interstitial.setAdUnitId(getString(R.string.interstitial_id));

		// Begin loading your interstitial.
		interstitial.loadAd(request);
		initMainMenu();
	}

	public void displayInterstitial() {
		if (interstitial.isLoaded()) {
			interstitial.show();
		}
	}

	private void initMainMenu() {
		Button aiButton = (Button) findViewById(R.id.btn_ai_game);
		aiButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				AIGameDialog dialog = new AIGameDialog(MainActivity.this);
				dialog.setListener(MainActivity.this);
				dialog.show();
			}
		});

		Button netButton = (Button) findViewById(R.id.btn_net_game);
		netButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				netDialog = new NetGameDialog(MainActivity.this);
				netDialog.setListener(MainActivity.this);
				netDialog.show();
			}
		});
		Button moreApps = (Button) findViewById(R.id.btn_more_app);
		moreApps.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				startActivity(new Intent(Intent.ACTION_VIEW, Uri
						.parse("https://play.google.com/store/apps/developer?id=HoangTruong")));
			}
		});
	}

	public void onCreateEngine(Engine engine, boolean netServer) {
		setContentView(gameLayout);
		currentView = gameLayout;
		adView = (AdView) currentView.findViewById(R.id.adView);
		AdRequest request = new AdRequest.Builder().build();
		adView.loadAd(request);
		gameLayout.startGame(engine);
		displayInterstitial();
		if (netServer)
			gameLayout.startServer();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (currentView == homeMenuLayout)
				rateApp();
			else
				returnMain(true);
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	private void rateApp() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(R.string.exit);
		builder.setMessage(R.string.msg_exit_app);
		builder.setPositiveButton(android.R.string.ok,
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						finish();
					}
				});
		builder.setNeutralButton("Rate App",
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						final String appName = getPackageName();// your
																// application
																// package
						// name i.e play store
						// application url
						try {
							startActivity(new Intent(Intent.ACTION_VIEW, Uri
									.parse("market://details?id=" + appName)));
						} catch (android.content.ActivityNotFoundException anfe) {
							startActivity(new Intent(
									Intent.ACTION_VIEW,
									Uri.parse("http://play.google.com/store/apps/details?id="
											+ appName)));
						}

						dialog.dismiss();
					}
				});
		builder.setNegativeButton(android.R.string.cancel,
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						dialog.dismiss();
					}
				});
		builder.create().show();
	}

	public void stopAutoBrightness() {
		Settings.System.putInt(this.getContentResolver(),
				Settings.System.SCREEN_BRIGHTNESS_MODE,
				Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
	}

	private void adjustSoundMenu() {
		if (gameLayout.getEnableSound()) {
			soundItem.setIcon(R.drawable.mute);
			soundItem.setTitle(R.string.menu_no_sound);
		} else {
			soundItem.setIcon(R.drawable.music);
			soundItem.setTitle(R.string.menu_sound);
		}
	}

	public void finishGame() {
		gameLayout.finishGame();
		setContentView(homeMenuLayout);
		currentView = homeMenuLayout;
		adView = (AdView) currentView.findViewById(R.id.adView);
		AdRequest request = new AdRequest.Builder().build();
		adView.loadAd(request);
	}

	public void returnMain(boolean needAsk) {
		if (!needAsk) {
			finishGame();
			return;
		}
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(R.string.back_menu);
		builder.setMessage(R.string.msg_back_menu);
		builder.setPositiveButton(android.R.string.ok,
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						finishGame();
					}
				});
		builder.setNegativeButton(android.R.string.cancel,
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						return;
					}
				});
		builder.create().show();
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		switch (requestCode) {
		case 1:
			// When the request to enable Bluetooth returns
			if (resultCode == Activity.RESULT_OK) {
				// Bluetooth is now enabled, so set up a chat session
				netDialog.startDiscovery();
			}
			break;
		case 2:
			if (resultCode == 300) {
				netDialog.startNetServer();
			}
			break;
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		int id = item.getItemId();

		return super.onOptionsItemSelected(item);
	}

	@Override
	public void onNewGame() {
		AdRequest request = new AdRequest.Builder().build();

		interstitial.loadAd(request);
		interstitial.setAdListener(new AdListener() {
			@Override
			public void onAdLoaded() {
				displayInterstitial();
				super.onAdLoaded();
			}
		});
	}
}
