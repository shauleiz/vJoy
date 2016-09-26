REM Automate the creation of an SDK package

REM C#
COPY ..\X64\Release\vJoyInterface.dll ".\c#\x64\vJoyInterface.dll"
COPY ..\X64\Release\vJoyInterfaceWrap.dll ".\c#\x64\vJoyInterfaceWrap.dll"
COPY ..\Win32\Release\vJoyInterface.dll ".\c#\x86\vJoyInterface.dll"
COPY ..\Win32\Release\vJoyInterfaceWrap.dll ".\c#\x86\vJoyInterfaceWrap.dll"

REM Include Files
COPY ..\Inc\public.h .\Inc\public.h
COPY ..\apps\common\vJoyInterface\vjoyinterface.h .\Inc\vjoyinterface.h

REM Library Files
COPY ..\Win32\Release\vJoyInterface.dll .\LIB\vJoyInterface.dll
COPY ..\Win32\Release\vJoyInterface.lib .\LIB\vJoyInterface.lib
COPY ..\X64\Release\vJoyInterface.dll .\LIB\amd64\vJoyInterface.dll
COPY ..\X64\Release\vJoyInterface.lib .\LIB\amd64\vJoyInterface.lib
