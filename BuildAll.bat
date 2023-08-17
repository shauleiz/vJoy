echo off

SET BuildMode=Release
REM SET BuildMode=Debug

SET VS=2022\Community
SET BUILDER=%ProgramFiles%\Microsoft Visual Studio\%VS%\MSBuild\Current\Bin\MSBuild.exe
SET Target64=x64\%BuildMode%
SET Target32=Win32\%BuildMode%
SET DigiCertUtil=DigiCertUtil.exe
SET InnoCompiler=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe

REM Skip x86 for Windows 10 and above
goto build64

:build32
echo %DATE% %TIME%: Cleaning vJoy (x86) 
"%BUILDER%" vJoyAll.sln  /maxcpucount:1 /t:clean /p:Platform=Win32;Configuration=%BuildMode%
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

echo %DATE% %TIME%: Building vJoy (x86)
"%BUILDER%" vJoyAll.sln  /maxcpucount:4  /p:Platform=Win32;Configuration=%BuildMode%
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:build64
echo %DATE% %TIME%: Cleaning vJoy (x64)
"%BUILDER%" vJoyAll.sln  /maxcpucount:1 /t:clean /p:Platform=x64;Configuration=%BuildMode%
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

echo %DATE% %TIME%: Building vJoy (x64)
"%BUILDER%" vJoyAll.sln  /maxcpucount:4  /p:Platform=x64;Configuration=%BuildMode%
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:signapps
echo %DATE% %TIME%: Signing the applications
IF NOT EXIST %DigiCertUtil% GOTO NOUTIL
%DigiCertUtil% sign /sha1 "6b 05 e5 cc 0d b8 8a 0d 89 62 48 2e cb 9c d9 59 e1 b0 57 90" /noInput .\%Target64%\vJoyList.exe*.\%Target32%\vJoyList.exe*.\%Target64%\vJoyConf.exe*.\%Target32%\vJoyConf.exe*.\%Target64%\vJoyFeeder.exe*.\%Target32%\vJoyFeeder.exe
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
