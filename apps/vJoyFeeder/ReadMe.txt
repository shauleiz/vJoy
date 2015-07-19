========================================================================
    APPLICATION : vJoyDemo Project Overview

This project demonstrate writing of a feeder for vJoy. It has a minimal GUI that
enables the user to move eight axes in the range of 0 to 33000, to press 
and release up to 32 buttons and to operate one POV Hat cap.
The GUI part is separated from the vJoy-related part with the small seam code.


vJoyClient.cpp
    This is the file that you should be interested with. It contains all the functions you need to control and feed the vJoy device.
    Here are the main points of interest:
    - HANDLE hDevice: Global handle to the device. Obtained once then used for all accesses
    - OpenJoystickDevice(): Get the global handle to the vJoy device
    - openDevice(): Wrapper for OpenJoystickDevice(). The global handle updated (not returned)
    - isInstalled(): Tests if vJoy device is installed
    - getDeviceAttrib(): Gets a string holding device version
    - update_device(): Feeds vJoy device with position data.


vJoyDemo.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

vJoyDemo.cpp
    This is the main application source file.
    Contains the code to display the form.

Form1.h
    Contains the implementation of your form class and InitializeComponent() function.
Also includes functions that call functions from vJoyClient.cpp.

AssemblyInfo.cpp
    Contains custom attributes for modifying assembly metadata.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named vJoyDemo.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
