package vn.singviet.cotuong2019.engine;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
/**
 * (c) D09CN2 - PTIT - Ha Noi (c) DROIDSX
 *
 * @author Nguyen Hoang Truong<truongnguyenptit@gmail.com>
 * @since 8:36:33 PM Feb 3, 2013 Tel: 0974 878 244
 *
 */
public class ChessApplication extends Application {
	private static Context context;
	private final static String CONFIG_FILE = "chess.config";

    public void onCreate(){
//        VMRuntime.getRuntime().setMinimumHeapSize(2*1024*1024);
        super.onCreate();
        ChessApplication.context = getApplicationContext();
    }

    public static Context getAppContext() {
        return context;
    }

    public static void setSetting(String key, String value) {
    	SharedPreferences p =
    			context.getSharedPreferences(CONFIG_FILE, Context.MODE_PRIVATE);
    	Editor e = p.edit();
    	e.putString(key, value);
    	e.commit();
    }

    public static String getSetting(String key, String defValue) {
    	SharedPreferences p = context.getSharedPreferences(CONFIG_FILE, Context.MODE_PRIVATE);
    	return p.getString(key, defValue);
    }
}
