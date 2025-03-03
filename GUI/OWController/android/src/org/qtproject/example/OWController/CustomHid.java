package org.qtproject.example.OWController;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.net.ConnectivityManager;
import java.util.HashMap;
import java.util.Iterator;
import java.lang.Object;
import android.os.Bundle;

public class CustomHid
{
    private static int fileDescriptor = 0;
    private static UsbManager usbManager;
    private static UsbDevice usbDevice;

    public static int findUsbDevice(Context context, int vid, int pid)
    {
        usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> deviceList = usbManager.getDeviceList();
        Iterator<UsbDevice> deviceIterator = deviceList.values().iterator();
        usbDevice = null;
        while(deviceIterator.hasNext()) {
            UsbDevice device = deviceIterator.next();
            if ((device.getVendorId() == vid) && (device.getProductId() == pid)) {
                usbDevice = device;
                break;
            }
        }
        if (usbDevice != null) {
            UsbDeviceConnection usbDeviceConnection = usbManager.openDevice(usbDevice);
            fileDescriptor = usbDeviceConnection.getFileDescriptor();
        }
        return fileDescriptor;
    }
}
