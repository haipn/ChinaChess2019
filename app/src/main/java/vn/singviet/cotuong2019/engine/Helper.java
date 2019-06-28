package vn.singviet.cotuong2019.engine;

import android.app.Activity;
import android.util.DisplayMetrics;
/**
 * (c) D09CN2 - PTIT - Ha Noi (c) DROIDSX
 * 
 * @author Nguyen Hoang Truong<truongnguyenptit@gmail.com>
 * @since 8:36:33 PM Feb 9, 2013 Tel: 0974 878 244
 * 
 */
class Size {
	public int width;
	public int height;
	public Size() {
		width = 0;
		height = 0;
	}
	public Size(int w, int h) {
		width = w;
		height = h;
	}
}
public final class Helper {
	public static Size getScreenSize(Activity act) {
		DisplayMetrics metric = new DisplayMetrics();
		act.getWindowManager().getDefaultDisplay().getMetrics(metric);
        int width = metric.widthPixels;
        int height = metric.heightPixels;
        return new Size(width, height);
	}
}
