REM Sign applications if certification utility exists
IF NOT EXIST %USERPROFILE%\DESKTOP\DigiCertUtil.exe GOTO NOUTIL
%USERPROFILE%\DESKTOP\DigiCertUtil.exe sign /kernelDriverSigning ^
.\objfre_wxp_x86\i386\vjoy.cat*^
.\objfre_wxp_x86\i386\vJoy.sys*^
.\objfre_wxp_x86\i386\hidkmdf.sys*^
.\objfre_wxp_x86\i386\vJoyInstall.exe*^
.\objfre_wlh_amd64\amd64\vjoy.cat*^
.\objfre_wlh_amd64\amd64\vJoy.sys*^
.\objfre_wlh_amd64\amd64\hidkmdf.sys*^
.\objfre_wlh_amd64\amd64\vJoyInstall.exe
EXIT
:NOUTIL