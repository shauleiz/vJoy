echo off
SET VS=14.0
SET BUILDER=%ProgramFiles(x86)%\MSBuild\%VS%\Bin\MSBuild.exe
SET Target64=x64\Release
SET Target32=Win32\Release

goto signapps

:build32
echo Building vJoy (x86)
"%BUILDER%"  vjoy.sln  /t:rebuild /p:Platform=Win32;Configuration=Release
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:build64
echo Building vJoy (x64)
"%BUILDER%"  vjoy.sln  /t:rebuild /p:Platform=x64;Configuration=Release
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:signapps
echo Signing the applications
IF NOT EXIST %USERPROFILE%\DESKTOP\DigiCertUtil.exe GOTO NOUTIL
%USERPROFILE%\DESKTOP\DigiCertUtil.exe sign /sha1 "6d 54 71 df 6b bf af a5 d5 90 0c 88 c0 8d f0 e9 c5 13 69 0a" /noInput .\%Target64%\vJoyConf.exe*.\%Target32%\vJoyConf.exe*.\%Target64%\vJoyFeeder.exe*.\%Target32%\vJoyFeeder.exe*.\apps\External\JoyMonitor\x86\JoyMonitor.exe*.\apps\External\JoyMonitor\x64\JoyMonitor.exe
set SIGN_STATUS=%ERRORLEVEL%
if not %SIGN_STATUS%==0 goto fail
echo Signing the applications - OK

exit /b 0

:NOUTIL
echo Could not find DigiCertUtil on the desktop
goto fail

:fail
exit /b 1
