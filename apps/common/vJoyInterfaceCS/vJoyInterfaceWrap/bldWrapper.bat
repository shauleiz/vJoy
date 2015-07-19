echo on
SET BUILDER=%ProgramFiles(x86)%\MSBuild\12.0\Bin\MSBuild.exe
if /I "%BUILD_ALT_DIR%" == "fre_wxp_x86" (goto :fre_wxp_x86)
if /I "%BUILD_ALT_DIR%" == "chk_wxp_x86" (goto :chk_wxp_x86)
if /I "%BUILD_ALT_DIR%" == "fre_wlh_amd64" (goto :fre_wlh_AMD64)
if /I "%BUILD_ALT_DIR%" == "chk_wlh_amd64" (goto :chk_wlh_AMD64)
goto :eof



:fre_wxp_x86
SET OUTDIR=objfre_wxp_x86\i386\
mkdir %OUTDIR%
"%BUILDER%"  vJoyInterfaceWrap.csproj  /t:rebuild /p:IntDir=%OUTDIR%;OutDir=%OUTDIR%;Platform="Any CPU";Configuration=Release
goto :eof

:chk_wxp_x86
SET OUTDIR=objchk_wxp_x86\i386\
mkdir %OUTDIR%
"%BUILDER%"  vJoyInterfaceWrap.csproj  /t:rebuild /p:IntDir=%OUTDIR%;OutDir=%OUTDIR%;Platform=Win32;Configuration=Debug
goto :eof

:fre_wlh_AMD64
SET OUTDIR=objfre_wlh_amd64\amd64\
mkdir %OUTDIR%
"%BUILDER%"  vJoyInterfaceWrap.csproj  /t:rebuild /p:IntDir=%OUTDIR%;OutDir=%OUTDIR%;Platform=x64;Configuration=Release
goto :eof

:chk_wlh_AMD64
SET OUTDIR=objchk_wlh_amd64\amd64\
mkdir %OUTDIR%
"%BUILDER%"  vJoyInterfaceWrap.csproj  /t:rebuild /p:IntDir=%OUTDIR%;OutDir=%OUTDIR%;Platform=x64;Configuration=Debug
goto :eof


:eof