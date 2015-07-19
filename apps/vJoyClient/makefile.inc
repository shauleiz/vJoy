	
# Copy vJoyInterface.dll to output sub-directory 
.\$O\vJoyInterface.dll:
	copy /y ..\..\lib\$O\vJoyInterface.dll  $@
	
# Copy vJoyInterface.lib to output sub-directory
.\$O\vJoyInterface.lib:
	copy /y ..\..\lib\$O\vJoyInterface.lib  $@
	
