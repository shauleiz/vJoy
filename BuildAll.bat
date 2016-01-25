echo off
SET VS=14.0
SET BUILDER=%ProgramFiles(x86)%\MSBuild\%VS%\Bin\MSBuild.exe
SET Target64=x64\Release
SET Target32=Win32\Release
SET DigiCertUtil=%USERPROFILE%\DESKTOP\DigiCertUtil.exe
SET InnoCompiler=%ProgramFiles(x86)%\Inno Setup 5\ISCC.exe


:build32
echo %DATE% %TIME%: Cleaning vJoy (x86) 
"%BUILDER%"  vjoy.sln  /maxcpucount:1 /t:clean /p:Platform=Win32;Configuration=Release
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

echo %DATE% %TIME%: Building vJoy (x86)
"%BUILDER%"  vjoy.sln  /maxcpucount:4  /p:Platform=Win32;Configuration=Release
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:build64
echo %DATE% %TIME%: Cleaning vJoy (x64)
"%BUILDER%"  vjoy.sln  /maxcpucount:1 /t:clean /p:Platform=x64;Configuration=Release
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

echo %DATE% %TIME%: Building vJoy (x64)
"%BUILDER%"  vjoy.sln  /maxcpucount:4  /p:Platform=x64;Configuration=Release
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:signapps
echo %DATE% %TIME%: Signing the applications
IF NOT EXIST %DigiCertUtil% GOTO NOUTIL
%DigiCertUtil% sign /sha1 "6d 54 71 df 6b bf af a5 d5 90 0c 88 c0 8d f0 e9 c5 13 69 0a" /noInput .\%Target64%\vJoyConf.exe*.\%Target32%\vJoyConf.exe*.\%Target64%\vJoyFeeder.exe*.\%Target32%\vJoyFeeder.exe
set SIGN_STATUS=%ERRORLEVEL%
if not %SIGN_STATUS%==0 goto fail
echo %DATE% %TIME%: Signing the applications - OK

:inno
echo %DATE% %TIME%: Compiling the Inno Setup Script
IF NOT EXIST "%InnoCompiler%" GOTO NOINNO
"%InnoCompiler%" install\vJoyInstallerSigned.iss 
set INNO_STATUS=%ERRORLEVEL%
if not %INNO_STATUS%==0 goto fail
echo %DATE% %TIME%: Compiling the Inno Setup Script - OK
exit /b 0

:NOUTIL
echo %DATE% %TIME%: Could not find DigiCertUtil on the desktop
goto fail

:NOINNO
echo %DATE% %TIME%: Could not find Inno Setup Compiler
goto fail

:fail
exit /b 1
