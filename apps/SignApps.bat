REM Sign applications (if certification utility exists)
IF NOT EXIST %USERPROFILE%\DESKTOP\DigiCertUtil.exe GOTO NOUTIL
%USERPROFILE%\DESKTOP\DigiCertUtil.exe sign .\vJoyConf\objfre_wlh_amd64\amd64\vJoyConf.exe*.\vJoyConf\objfre_wxp_x86\i386\vJoyConf.exe*.\vJoyFeeder\objfre_wlh_amd64\amd64\vJoyFeeder.exe*.\vJoyFeeder\objfre_wxp_x86\i386\vJoyFeeder.exe*.\External\JoyMonitor\x86\JoyMonitor.exe*.\External\JoyMonitor\x64\JoyMonitor.exe
:NOUTIL