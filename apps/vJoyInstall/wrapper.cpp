#include "stdafx.h"
#include "vJoyInstall.h"
#include "wrapper.h"

extern StatusMessageFunc StatusMessage;
BOOL DummyStatusMessage(void * output, TCHAR * buffer,  ERRLEVEL level);
//void SetStatusMessageFunc( StatusMessageFunc f);

int WINAPI  DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	UNREFERENCED_PARAMETER(hModule);
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			StatusMessage = DummyStatusMessage; 
			break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }
			
   return 1;
}

/* External Functions */
extern "C" {
BOOL WINAPI  repair(TCHAR * DeviceHWID, TCHAR * InfFile)
{
	return(Repair(DeviceHWID, InfFile));
}

BOOL WINAPI  get_inf_file(TCHAR * InfFile)
{
	return GetInfFile(0,NULL,InfFile);
}

BOOL WINAPI  get_dev_hwid(TCHAR * DeviceHWID)
{
	return GetDevHwId(0, NULL , DeviceHWID);
}

int WINAPI  installation(TCHAR * DeviceHWID, TCHAR * InfFile)
{
	return Installation(DeviceHWID, InfFile);
}

int WINAPI  removal(TCHAR * DeviceHWID, TCHAR * InfFile, BOOL DelInf)
{
	return Removal(DeviceHWID, InfFile, DelInf);
}

BOOL WINAPI is_ppjoy_installed()
{
	return isPPJoyInstalled();
}

BOOL WINAPI is_vjoy_installed(void)
{
	return isvJoyInstalled();
}

BOOL WINAPI is_vjoy_installed_specific(USHORT Revision)
{
	return isvJoyInstalledSpecific(Revision);
}

BOOL WINAPI disable(USHORT Revision)
{
	return Disable(Revision);
}

BOOL WINAPI enable(USHORT Revision)
{
	return Enable(Revision);
}

void WINAPI refresh_vjoy(void)
{
	RefreshvJoy();
}

void WINAPI refresh_vjoy_specific(USHORT Revision)
{
	RefreshvJoySpecific(Revision);
}

void WINAPI  SetStatusMessageFunc( StatusMessageFunc f)
{
	StatusMessage = f;
};

int WINAPI  Func3(void)
{
	Beep(400,200);
	printf("This is function Func3 in file wrapper.c\n");
	return 12345;
}

} // Extern "C"

// Wrapper Internal functions
BOOL DummyStatusMessage(void * output, TCHAR * buffer,  ERRLEVEL level)
{
	UNREFERENCED_PARAMETER(level);
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(output);

	return FALSE;
}
