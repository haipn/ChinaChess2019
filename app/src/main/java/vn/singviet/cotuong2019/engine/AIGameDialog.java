package vn.singviet.cotuong2019.engine;


import android.app.Dialog;
import android.content.Context;
import android.content.res.Resources.Theme;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.RadioGroup;

import vn.singviet.cotuong2019.R;

/**
 * (c) D09CN2 - PTIT - Ha Noi (c) DROIDSX
 * 
 * @author Nguyen Hoang Truong<truongnguyenptit@gmail.com>
 * @since 8:36:33 PM Feb 3, 2014 Tel: 0974 878 244
 * 
 */
public class AIGameDialog extends Dialog {
	private GameListener listener = null;

	public AIGameDialog(Context context) {
		super(context);
		getContext().setTheme(android.R.style.Theme_Dialog);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
	}

	public void setListener(GameListener listener) {
		this.listener = listener;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.ai_menu);
		Button btnStart = (Button) findViewById(R.id.btn_start_game);
		btnStart.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				int player = 0;
				float searchSeconds = 0.5f;
				RadioGroup radioColor = (RadioGroup) findViewById(R.id.rad_player_color);
				if (radioColor.getCheckedRadioButtonId() == R.id.radio_red) {
					player = 0;
				} else {
					player = 1;
				}
				RadioGroup radioLevel = (RadioGroup) findViewById(R.id.radio_level);
				if (radioLevel.getCheckedRadioButtonId() == R.id.radio_easy) {
					searchSeconds = 0.5f;
				} else if (radioLevel.getCheckedRadioButtonId() == R.id.radio_normal) {
					searchSeconds = 1.5f;
				} else {
					searchSeconds = 2.5f;
				}
				AIEngine engine = new AIEngine(player, searchSeconds);

				dismiss();
				if (listener != null)
					listener.onCreateEngine(engine, false);
			}
		});
	}
}
