# vJoy
Virtual Joystick

This project supports Windows 7 to Windows 10 1803

For newer versions of Windows go to Fork: https://github.com/jshafer817/vJoy



For much more go to vJoy web site: http://vjoystick.sourceforge.net

To build the project:

1. Pull the project

2. Run batch file BuildAll.bat

Caution:
You will need to install Visual Studio 2015 (Free)
To build the entire project including the driver you need to install the SDK and puchase a license to sign drivers.

# What vJoy does

vJoy reates a virtual joystick that can be controlled by code. Various applications read a real joystick (or for example keyboard input), and send that output to the vJoy virtual stick.  It can only create virtual DirectInput sticks (including many flight sticks etc), not XInput (Xbox) controllers.

A common use scenario is for application writers who want to emulate joystick behaviour or want a PPJoy replacement.

## Features

- Virtual Device and device driver 32/64 bit
- Driver is signed
- Configurable:
  - Up to 8 axes
  - Up to 128 buttons and optional 4 POV Hat Switch
- Unified x86+x64 installer
