// List of available commands
enum CMD {UNK, HLP, VER, RST, DEL, FRC, AXS, BTN, PVA, PVD, GUI, CRT, RPT,};

LPCTSTR HelpStr = L"\
\n\
vJoy Configuration Utility \n\
Syntax: \n\
 \n\
vJoyConfig   \n\
Launch GUI (if possible) \n\
 \n\
vJoyConfig -h \n\
Help text \n\
 \n\
vJoyConfig -t \n\
Report current configuration \n\
\n\
vJoyConfig  -v   \n\
Version information \n\
 \n\
vJoyConfig -r    \n\
Reset vJoy to default configuration \n\
 \n\
vJoyConfig -d Dn [Dn+1...] \n\
Delete one or more devices \n\
Example: vJoyConfig -d 1 3 12 (Delete device 1, 3 and 12) \n\
 \n\
vJoyConfig  Dn [-f] [-a Ai [Ai+1 ...]] [-b n] {[-p m] | [-s j]} \n\
Create a joystick device \n\
 \n\
  Dn     The index of the target joystick device in the range of 1-16 \n\
  -f     Force creation even if device exists. This will delete the current device before creating a new one. \n\
  -a Ai  Define axes (one or more). \n\
         Possible values are (Case insensitive): x, y, z, rx, ry, rz, sl0, sl1 \n\
         In the absence of this flag the default will be used (Default = all axes) \n\
  -b n   Set the number of buttons  (Default = 8 buttons) \n\
  -p m   Set  the number of analog  POV Switches  (Default = 0) \n\
  -s j   Set  the number of discrete POV Switches  (Default = 0)\n\
				   \n";