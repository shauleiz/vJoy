# vJoy
Virtual Joystick
---

This project supports Windows 7 to Windows 10 1803

For newer versions of Windows go to Fork: https://github.com/jshafer817/vJoy



For much more go to vJoy web site: http://vjoystick.sourceforge.net

To build the project:

1. Pull the project

2. Run batch file BuildAll.bat to get installer (in install/ directory)

3. Run batch file CreateSDK.bat to copy SDK files (in SDK/ directory)

Caution:
You will need to install Visual Studio 2022 Community (Free) + WDK and MFC/ATL libraires with Spectre mitigation and InnoSetup 6.0.
To build the entire project including the driver you need to install the SDK and puchase a license to sign drivers.

Justin's notes are now at the end of this document.

## Benjamin's compiling notes

1. Install VS2022 Community + Runtime/MFC/ATL Spectre mitigated librairies,
all for v142.
2. Install last WDK (10.0.22621.0) and last Windows 10 SDK (10.0.22621.0).
3. Install Inno Setup 6.x.
4. DigiCert utility is provided in the git tree for now, but you need a valid EV
certificate. If you want to testsign the software, creates a WDKTestCertificate 
from VS2022 and register it in the certificate store (see chapter below)
5. Open BuildAll.bat and make sure all paths are correct.
6. Launch Developer Command Prompt for VS2022
7. Run BuildAll.bat from command line. This will compile all software and build
the installer.
8. To build the SDK, run SDK\CreateSdk.bat from command line.

## When install failed (notes from Shaul's vJoy website):

In case the installation failed, please post your problem in the forum. Attach two log files:

1) The log file for the Inno Setup wrapper is called Setup Log 2011-11-01 #xxx.txt and is located under %TEMP% folder. (xxx can be any number in the range 001-999).

2) The log file for the executable is called vJoyInstall.log and located in the vJoy folder (C:/Program Files/vJoy).

## Know a bit more what is vJoy and how it works

Shaul wrote very good guides about vJoy's internal, please see:

_[http://vjoystick.sourceforge.net/site/index.php/dev216/system-architecture]_
https://web.archive.org/web/20190509030210/http://vjoystick.sourceforge.net/site/index.php/dev216/system-architecture


_[http://vjoystick.sourceforge.net/site/index.php/dev216/internal-architecture]_
https://web.archive.org/web/20190509030210/http://vjoystick.sourceforge.net/site/index.php/dev216/internal-architecture


### How to remove completly vJoy when it is a complete mess:

Shaul's notes here:
_[http://vjoystick.sourceforge.net/site/index.php/download-a-install/77-vjoy]_
https://web.archive.org/web/20180602003145/http://vjoystick.sourceforge.net/site/index.php/download-a-install/77-vjoy

This is an extract from the page :
>1. Enter `pnputil -e` in the command-line window.
>2. Search for `Shaul` in the text you receive. 
>
> &nbsp;
> If you don't find then vJoy is not installed
>
> &nbsp;
> If you find entries of the following type:
>![Driver store 2](docs/readme/Driver_store_2.png)
>
>3. Write down the names of the OEM files (e.g. oem86.inf) associated with vJoy.
>
>4. Remove all OEM files associated with vJoy, one by one.
>5. To remove an OEM file  (e.g. oem86.inf) you need to enter 
>`pnputil -f -d oemXX.inf` where XX stands for the appropriate number.
>(e.g `pnputil -f -d oem86.inf`)



## Building a Debug or Release version for testing on a test computer

Since you need a EV certificate to build the public driver (which is expensive),
there is still a solution for developping, debugging and testing.

You have to create a local test certificate from within Visual Studio in Driver
Signing page, then select "Test Sign" as the signing mode instead of "Production 
Sign".
If you don't have yet a test certificate, simply creates one from within Visual
Studio dialog box. Once created, or if already created, use "select from store"
to select the certificat and export it to a file (*.PFX recommended by 
Microsoft).
Once you have build a full setup with the test-signed driver you can install it
on a test computer with the test certificate.

To build the solution, the easiest solution is to run the provided batch file 
from either a cmd.exe prompt, or from within visual Studio, see:

https://web.archive.org/web/20210119130340/http://geekswithblogs.net/SoftwareDoneRight/archive/2010/01/28/running-batch-files-from-the-solution-explorer-in-visual-studio.aspx

The test computer must enable loading of test signed driver, see:

https://docs.microsoft.com/en-us/windows-hardware/drivers/install/the-testsigning-boot-configuration-option

Basically, you have to remove UEFI secure boot from your Bios, then within 
Windows you must run cmd.exe with administrative provilieges and type:

`C:\WINDOWS\system32> bcdedit.exe -set TESTSIGNING ON`

Then reboot your computer. You shoud see a watermark in the bottom right corner
telling you that the system is in test mode. Worth noting that doing that this
is only for testing and not for permanent use.

Install the test certificate in the trusted root store, see:

https://www.tbs-certificates.co.uk/FAQ/en/174.html

https://docs.microsoft.com/en-us/windows-hardware/drivers/develop/signing-a-driver-during-development-and-testing

https://docs.microsoft.com/en-us/windows-hardware/drivers/install/installing-a-test-certificate-on-a-test-computer

## Updating the internal vJoy version

The version is automatically retrieved from git tag by checkversion.exe (see C++ project) 
which calls internally `git describe --all` to retrieve version from last
annoted tag stored in the git tree. How to update the version:
- commit your changes
- perform an annoted tag with git : git tag -a v2.1.9.2 -m "v2.1.9.2". To
perform an annoted tag operation with TortoiseGit, simply add a message like 
"v2.1.9.2" and Tortoise will automatically deduce it is an annoted tag.
- **update the "vjoy.inx" file!!**: look for _%vjoy% = vjoy.Inst.Win7, 
root\VID_1234&PID_0FFB&REV_0222_ and replace 0222 by the correct version.

## Building a release version for public release

The same procedure as below, but you need a valid EV certificate and select 
"Production Sign" in the Driver Signing page in each VS project file.

### Compiling and redistibuting vJoy's internal apps - Installer

When updating your build system (like update from VSXX to VSYY), place
the C++ dll dependencies in apps\External\DLL

Shaul gave a detailled explanation how the installer works:
_[http://vjoystick.sourceforge.net/site/index.php/dev216/installer]_
https://web.archive.org/web/20190509030210/http://vjoystick.sourceforge.net/site/index.php/dev216/installer

_[http://vjoystick.sourceforge.net/site/index.php/download-a-install/deployment216/vjoy-installer]_
https://web.archive.org/web/20190509030211/http://vjoystick.sourceforge.net/site/index.php/download-a-install/deployment216/vjoy-installer

### Apps documentation

**vJoyConf:**
https://web.archive.org/web/20190509030211/http://vjoystick.sourceforge.net/site/index.php/utilities-218/vjoyconf

**vJoyConfig:**
https://web.archive.org/web/20190509030211/http://vjoystick.sourceforge.net/site/index.php/utilities-218/vjoyconfig

**Monitor:**
https://web.archive.org/web/20190509030211/http://vjoystick.sourceforge.net/site/index.php/utilities-218/monitor

**vJoyList:**
https://web.archive.org/web/20190509030208/http://vjoyhttp://vjoystick.sourceforge.net/site/index.php/utilities-218/vjoyfeederstick.sourceforge.net/site/index.php/utilities-218/vjoylist

**vJoyFeeder:**
https://web.archive.org/web/20190509030211/http://vjoystick.sourceforge.net/site/index.php/utilities-218/vjoyfeeder

## Developping and debugging vJoy

Once you have configured your Visual Studio to use a test certificate, you
can deploy and debug your driver in a targtet - or test - computer.
The target computer must run the same x86 or x64 architecture as your host.

Tutorial on Windows driver development:

https://www.harald-rosenfeldt.de/2019/05/25/windows-driver-development/

https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/writing-a-very-small-kmdf--driver

A good tutorial on how to setup your target computer and start a debug session
is available here:

https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/provision-a-target-computer-wdk-8-1

https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debug-universal-drivers---step-by-step-lab--echo-kernel-mode-

For network kernel debugging (KDNET), see also:

https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/setting-up-a-network-debugging-connection-automatically

The recommanded debugger is WinDbg, that is available in the Microsoft 
Store.

https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/

A tutorial is given here:

https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/getting-started-with-windbg--kernel-mode-


### Build in Debug mode and display traces messages

To build in Debug mode, you need to change the BuildAll.bat script:

```
REM SET BuildMode=Release
SET BuildMode=Debug
```


### Usefull WinDbg commands

If you want to see the traces in the driver's code, you need to remove all 
filtering in the debugger:

`kd> ed nt!Kd_DEFAULT_MASK  0xFFFFFFFF`

Please also check this page:

https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/reading-and-filtering-debugging-messages

To load symbols, use the already builtin sympath, or add Microsoft's symbol 
server 

`.sympath srv*`


`.sympath srv*https://msdl.microsoft.com/download/symbols`

To add your own symbols path (for example vJoy *.pdb files in c:\dev\vjoy\2.1.9.1\vJoy path),
use

`.sympath+ C:\dev\vjoy\2.1.9.1\vJoy`

To reload symbols, use

`.reload`  (I do not recommand to add `/f` to force retrieving, as lazy evaluation is faster)

To get information about vJoy module, type :

`!drvobj vJoy`



# Older Justin's notes:
1. Install Visual Studio 2013
2. Install Visual Studio 2015 Update 3
3. Install Windows 10 SDK 1511
4. Install Windows 10 WDK 1511
5. Install Windows 10 SDK 1607
6. Install Windows 10 WDK 1607
7. Install DotNet Framework 3.5 SP1
8. Install DotNet Framework 3.5 SDK (6.0.6001.18000.367-KRMSDK_EN.iso)
9. Install Innosetup 5
10. Place the DigiCert Utility on the desktop.
11. Copy the cfg_support_v1.lib file from C:\Program Files (x86)\Windows Kits\10\Lib\10.0.14393.0\km\x86 and x64 to C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\km\x86 and x64
