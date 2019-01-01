// Example code to retrieve vendor and device ID's for the primary display
// device.
//
// NOTE: Visual Studio 6 does not ship with a winuser.h that defines the
//       EnumDisplayDevices function so in order to compile this code, you'll 
//       need to install the Platform SDK. 

#include <windows.h>

#include <string>
#include <iostream>

using namespace std;

bool GetDeviceIdentification(string &vendorID, string &deviceID)
{
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);

    int i = 0;
    string id;

    // locate primary display device
    while (EnumDisplayDevices(NULL, i, &dd, 0))
    {
        if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        {
            id = dd.DeviceID;
            break;
        }

        i++;
    }

    if (id == "") return false;

    // get vendor ID
    vendorID = id.substr(8, 4);

    // get device ID
    deviceID = id.substr(17, 4);

    return true;
}

int main(void)
{
    string vendorID;
    string deviceID;

    if (GetDeviceIdentification(vendorID, deviceID))
    {
        cout << "Vendor ID: " << vendorID << endl;
        cout << "Device ID: " << deviceID << endl;
    }
    else
        cout << "Unable to locate device information" << endl;

    return 0;
}