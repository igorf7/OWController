package org.qtproject.example;

import android.app.Activity;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbEndpoint;
import android.content.Context;
import android.hardware.usb.UsbDeviceConnection;

public class UsbBridge {
    private static UsbManager usbManager;
    private static UsbDeviceConnection connection;
    private static UsbEndpoint inEndpoint;
    private static UsbEndpoint outEndpoint;

    public static void init(Activity activity) {
        usbManager = (UsbManager) activity.getSystemService(Context.USB_SERVICE);
    }

    public static boolean openDevice(int vendorId, int productId) {
        for (UsbDevice device : usbManager.getDeviceList().values()) {
            if (device.getVendorId() == vendorId && device.getProductId() == productId) {
                UsbInterface intf = device.getInterface(0);
                UsbDeviceConnection conn = usbManager.openDevice(device);
                if (conn != null && conn.claimInterface(intf, true)) {
                    connection = conn;
                    for (int i = 0; i < intf.getEndpointCount(); i++) {
                        UsbEndpoint ep = intf.getEndpoint(i);
                        if (ep.getType() == UsbConstants.USB_ENDPOINT_XFER_INT) {
                            if (ep.getDirection() == UsbConstants.USB_DIR_IN) inEndpoint = ep;
                            else outEndpoint = ep;
                        }
                    }
                    return true;
                }
            }
        }
        return false;
    }

    public static synchronized byte[] read(int length) {
        byte[] buffer = new byte[length];
        if (inEndpoint != null) {
            int r = connection.bulkTransfer(inEndpoint, buffer, length, 50);
            if (r > 0) {
                byte[] result = new byte[r];
                System.arraycopy(buffer, 0, result, 0, r);
                return result;
            }
        }
        return null;
    }

    public static synchronized boolean write(byte[] data) {
        if (outEndpoint == null) return false;
        int r = connection.bulkTransfer(outEndpoint, data, data.length, 50);
        return r == data.length;
    }

    public static int getRxDataSize()
    {
        int res = inEndpoint.getMaxPacketSize();
        return res;
    }

    public static void closeConnection() {
        connection.close();
    }
}
