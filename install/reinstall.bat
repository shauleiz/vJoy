REM Uninstall then install driver hidusbfx2
devcon -f dp_delete oem18.inf
devcon  remove "root\VID_0547&PID_1002"
copy /y C:\WinDDK\7600.16385.1\src\Dev\hidusbfx2\sys\objchk_win7_x86\i386\hidusbfx2.sys .
pause
devcon install hidusbfx2.inf "root\VID_0547&PID_1002"

