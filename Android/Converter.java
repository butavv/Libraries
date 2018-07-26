package com.phase1eng.nick.graham2;

import android.graphics.Color;
import android.util.Log;
import android.view.Display;

import org.nocrala.tools.gis.data.esri.shapefile.shape.PointData;

import java.math.BigDecimal;
import java.nio.ByteBuffer;

/**
 * Created by Nick on 9/28/2017.
 */

public class Converter {

    private static double earthRadiusMeters = 6371000.0;
    private static float mDpi;

    public final static long BYTES_PER_BOX = 40 + 40 + 8 + 8 + 32 + 64;
    
    public static long subBoxesPerBox = 64;
    public static long bytesPerMB = 1000000;
    public static long bytesPerKB = 1000;

    public static void setPixelDensity(int dpi){
        Log.d("PIXEL DENSITY", ""+dpi);
        mDpi = (float)dpi;
    }

    public static double coordinatesToMeters(double lat1, double lon1, double lat2, double lon2){
        final int R = 6371; // Radius of the earth

        double latDistance = Math.toRadians(lat2 - lat1);
        double lonDistance = Math.toRadians(lon2 - lon1);
        double a = Math.sin(latDistance / 2) * Math.sin(latDistance / 2)
                + Math.cos(Math.toRadians(lat1)) * Math.cos(Math.toRadians(lat2))
                * Math.sin(lonDistance / 2) * Math.sin(lonDistance / 2);
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
        double distance = R * c * 1000; // convert to meters

        distance = Math.pow(distance, 2);

        return Math.sqrt(distance);
    }



    public static double coordinatesToInches(double lat1, double lon1, double lat2, double lon2){
        return metersToInches(coordinatesToMeters(lat1, lon1, lat2, lon2));
    }

    public static double metersToInches(double meters){
        return meters * 39.3701;
    }

    public static double inchesToMeters(double inches){
        return inches * 0.0254;
    }

    public static long boxesToMb(int boxes){

        return boxes*BYTES_PER_BOX/bytesPerMB;
    }

    public static double distanceBetweenPoints(PointData p1, PointData p2){
        return Math.sqrt(Math.pow( (p1.getX()-p2.getX()) ,2) + Math.pow( (p1.getY()-p2.getY()) , 2));
    }

    public static double round(double value, int numberOfDigitsAfterDecimalPoint) {
        BigDecimal bigDecimal = new BigDecimal(value);
        bigDecimal = bigDecimal.setScale(numberOfDigitsAfterDecimalPoint,
                BigDecimal.ROUND_HALF_UP);
        return bigDecimal.doubleValue();
    }

    public static double galPerAcToLPerHa(double galPerAc){
        return galPerAc * 9.35396;
    }

     public static long boxesToKb(int boxes){

        return boxes*BYTES_PER_BOX/bytesPerKB;
    }
    public static long MBToBytes(int mb){
         return mb * bytesPerMB;
    }

    public static int bytesToMB(long bytes){
        return (int)((double)bytes/(double)bytesPerMB);
    }

    public static int kgToLbs(int kg){
        return (int) ((double)kg * 2.20462);
    }

    public static int lbsToKg(int lbs){
        return (int) ((double)lbs / 2.20462);
    }

    public static double coordinatesToBearing(double lat1, double lon1, double lat2, double lon2){
        lat1 = Math.toRadians(lat1);
        lat2 = Math.toRadians(lat2);
        lon1 = Math.toRadians(lon1);
        lon2 = Math.toRadians(lon2);
        double x = Math.cos(lat2)*Math.sin(lon1-lon2);
        double y = Math.cos(lat1)*Math.sin(lat2)-Math.sin(lat1)*Math.cos(lat2)*Math.cos(lon1-lon2);
        double bearing =  Math.toDegrees(Math.atan2(x,y));
        bearing = (bearing + 360) % 360;
        bearing = 360 - bearing;
        return bearing;
    }

    public static double pointToBearing(PointData p1, PointData p2){
        double angle = Math.atan2( (p2.getY()-p1.getY()), (p2.getX()-p1.getX()) );
        double bearing = Math.toDegrees(angle) - 90;
        bearing = (bearing + 360) % 360;
        bearing = 360 - bearing;
        return bearing;
    }

    //Bearing must be in degrees
    public static PointData getNewCoordinates(double lat, double lon, double dist, double brng){

        //Log.d("Point 1:","("+lat+", "+lon+") @"+brng+"degrees");

        double lat1R = Math.toRadians(lat);
        double lon1R = Math.toRadians(lon);
        double bR = Math.toRadians(brng);
        double dR = dist/earthRadiusMeters;
        double a = Math.sin(dR) * Math.cos(lat1R);
        double lat2 = Math.asin(Math.sin(lat1R) * Math.cos(dR) + a * Math.cos(bR));
        double lon2 = lon1R + Math.atan2(Math.sin(bR) * a, Math.cos(dR) - Math.sin(lat1R) * Math.sin(lat2));

        PointData coordinates = new PointData(Math.toDegrees(lon2), Math.toDegrees(lat2));
        //Log.d("Point 2:","("+coordinates.getY()+", "+coordinates.getX()+")");

        return coordinates;
    }

    public static PointData getNewPoint(PointData p1, double dist, double angle){
        double theta = Math.toRadians(angle);
        double dx = dist*Math.sin(theta);
        double dy = dist*Math.cos(theta);
        PointData p2 = new PointData(p1.getX()+dx, p1.getY()+dy);
        return p2;
    }

    public static double metersPerMsToKmPerHour(double metersPerMs){
        return metersPerMs*3600; 
    }

    public static double metersPerMsToMilesPerHour(double metersPerMs){
        return metersPerMs*2236.94;
    }

    public static double milesPerHourToMetersPerSecond(double milesPerHour){
        return milesPerHour * 0.44704;
    }

    public static double kmPerHourToMetersPerSecond(double kmPerHour){
        return kmPerHour * 0.277778;
    }

    //Angle 1 & 2 must be < 360
    public static double addAngles(double angle1, double angle2){
        double result = angle1+angle2;
        if(result>360){
            result -= 360;
        }
        else if(result < 0){
            result += 360;
        }

        return result;
    }



    public static double squareMetersToAcres(double squareMeters){
        return squareMeters*0.000247105;
    }

    public static double acresToHectares(double acres){
        return acres * 0.404686;
    }

    public static double squareMetersToHectares(double squareMeters){
        return squareMeters*0.0001;
    }

    //Angle 1 & 2 must be < 360
    public static double subtractAngles(double angle1, double angle2){
        double result = angle1-angle2;
        if(result>360){
            result -= 360;
        }
        else if(result < 0){
            result += 360;
        }

        return result;
    }

    public static int byteArrayToInt(byte[] b) {
        if (b.length == 4)
            return b[0] << 24 | (b[1] & 0xff) << 16 | (b[2] & 0xff) << 8
                    | (b[3] & 0xff);
        else if (b.length == 2)
            return 0x00 << 24 | 0x00 << 16 | (b[0] & 0xff) << 8 | (b[1] & 0xff);

        return 0;
    }

    public static final byte[] intToByteArray(int value) {
        return new byte[] {
                (byte)(value >>> 24),
                (byte)(value >>> 16),
                (byte)(value >>> 8),
                (byte)value};
    }

    public static byte[] intToByteArrayLe(int value){
        return new byte[] {
                (byte)(value),
                (byte)(value >>> 8),
                (byte)(value >>> 16),
                (byte)(value >>>24 )};
    }

    public static byte[] doubleToByteArray(double value) {
        byte[] bytes = new byte[8];
        ByteBuffer.wrap(bytes).putDouble(value);
        return bytes;
    }

    public static double byteArrayToDouble(byte[] bytes) {
        return ByteBuffer.wrap(bytes).getDouble();
    }


    public static float gmZoomToMetersPerPixel(float zoom, double latitude) {

        //Calculation for display resolution of 320 dpi (2048 x 1536) 8.0 inch screen
        float mppx = (float)(156543.03392 * Math.cos(latitude * Math.PI / 180) / Math.pow(2, zoom));
        float ratio = 320.0f/mDpi;

        return mppx*ratio;
    }

    public static float gmZoomToMeters(float zoom){
        int range = (int) (35200000/(Math.pow(2, zoom)));
        if (range < 300){
            range = 300; 
        } 
        return range; 
    }


    //Top 2 bits are color:
    // 0 = Red
    // 1 = Green
    // 2 = Blue
    // 3 = Yellow

    //Bottom 6 bits are shade
    // If shade = 0... alpha = 0
    public static byte intColorToByteColor(int pColor){

        byte retval = 0;

        int A = (pColor >>> 24) & 0xff; // or color >>> 24
        int R = (pColor >>> 16) & 0xff;
        int G = (pColor >>>  8) & 0xff;
        int B = (pColor      ) & 0xff;
        
        boolean debug = false;
        if(pColor != 0){
            //debug = true;
        }

        if(debug){
            
        }

        byte color = 0;
        byte shade = 0;


        if(R>MapController.coverageBrightness){
            color = 0;
            if(G>MapController.coverageBrightness){
                color = 3;
            }
            double ratio = (double)R/255.0;
            shade = (byte)((ratio*64.0));
        }
        else if(G>MapController.coverageBrightness){
            color = 1;
            double ratio = (double)G/255.0;
            shade = (byte)((ratio*64.0));
        }
        else if(B>MapController.coverageBrightness){
            color = 2;
            double ratio = (double)B/255.0;
            shade = (byte)((ratio*64.0));
        }

        if(shade > 63){
            shade = 63;
        }
        if(shade<5){
            shade = 5;
        }


        retval |= shade;
        retval |= (color << 6);

        if(debug){
            Log.d("Color", "A="+A+" R="+R+" G="+G+" B="+B+" Color="+color+" Shade="+shade);
        }

        return retval;


    }

    //Top 2 bits are color:
    // 0 = Red
    // 1 = Green
    // 2 = Blue
    // 3 = Yellow

    //Bottom 6 bits are shade
    // If shade = 0... alpha = 0
    public static int byteColorToIntColor(byte pColor){

        int retval = 0;

        boolean debug = false;

        int colorMask = 192;
        int shadeMask = 63;

        /*int A = ((pColor & aMask) >>> 7) ;
        int R = ((pColor & rMask) >>> 6);
        int G = ((pColor & gMask) >>> 5);
        int B = ((pColor & bMask) >>> 4);

        

        if(A == 1){
            A = 255;
        }
        if( R == 1){
            R = 255;
        }
        if( G == 1){
            G = 255;
        }
        if( B == 1){
            B = 255;
        }*/

        if(pColor != 0){
            //debug = true;
        }


        int shade = (pColor & shadeMask);
        int color = ((pColor & colorMask) >>> 6);
        //double ratio = (double)shade/63.0;

        /*int A=0;
        int R=0;
        int G=0;
        int B=0;*/

        if(debug){
            Log.d("IN", "Color="+color+" Shade="+shade);
        }

        if(shade == 0){
            return Color.TRANSPARENT;
        }

        switch(color){
            case 0:
                return ColorSchemes.red[shade];
            case 1:
                return ColorSchemes.green[shade];
            case 2: 
                return ColorSchemes.blue[shade];
            case 3:
                //return ColorSchemes.yellow[shade];
                return Color.YELLOW;
        }

        /*if(shade != 0){
            A = 255;
        }

        retval |= (A << 24);
        retval |= (R << 16);
        retval |= (G << 8);
        retval |= B;

        if(debug){
            Log.d("OUT", ""+A+", "+R+", "+G+", "+B);
        }*/

        return retval;

    }
}
