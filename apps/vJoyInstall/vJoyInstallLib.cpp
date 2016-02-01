#include "stdafx.h"
#include "vJoyInstall.h"

FILE *stream;

StatusMessageFunc StatusMessage = StatusMessageToStream;
TCHAR prt[MAX_PATH];

BOOL FindInstalled(LPCTSTR hwid, TCHAR *InstanceId)
{
	return ChangeInstalled(hwid, InstanceId, TRUE, TRUE);
}

BOOL EnableInstalled(LPCTSTR hwid, TCHAR *InstanceId)
{
	return ChangeInstalled(hwid, InstanceId, TRUE, FALSE);
}

BOOL DisableInstalled(LPCTSTR hwid, TCHAR *InstanceId)
{
	return ChangeInstalled(hwid, InstanceId, FALSE, TRUE);
}


BOOL ChangeInstalled(LPCTSTR hwid, TCHAR *InstanceId, BOOL Enable, BOOL Disable)
{
	TCHAR ErrMsg[1000];
	BOOL out = FALSE;
	//SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;

	if (!hwid)
	{
		_stprintf_s(prt, MAX_PATH, "FindInstalled: HWID cannot be (null)");
		StatusMessage( NULL, prt,  FATAL);
		return TRUE;
	};

	if (!InstanceId)
	{
		_stprintf_s(prt, MAX_PATH, "FindInstalled: Instance ID cannot be (null)");
		StatusMessage( NULL, prt,  FATAL);
		return TRUE;
	};

	//_ftprintf(stream,">> FindInstalled: Serching for HWID %s\n", hwid);
	_stprintf_s(prt, MAX_PATH, "FindInstalled: Searching for HWID %s", hwid);
	StatusMessage( NULL, prt,  INFO);


	HDEVINFO devs = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT );
	if (devs == INVALID_HANDLE_VALUE)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] FindInstalled: Function SetupDiGetClassDevs failed with error: %s\n", ErrMsg);
		_stprintf_s(prt, MAX_PATH, "FindInstalled: Function SetupDiGetClassDevs failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	};

	//devInfoListDetail.cbSize = sizeof(devInfoListDetail);
	//if(!SetupDiGetDeviceInfoListDetail(devs,&devInfoListDetail)) {
	//	GetErrorString(ErrMsg,1000);
	//	//_ftprintf(stream,"[E] FindInstalled: Function SetupDiGetDeviceInfoListDetail failed with error: %s\n", ErrMsg);
	//	_stprintf_s(prt, MAX_PATH, "FindInstalled: Function SetupDiGetDeviceInfoListDetail failed with error: %s", ErrMsg);
	//	StatusMessage( NULL, prt,  ERR);
	//	return FALSE;
	//};

	SP_DEVINFO_DATA devInfo;
	devInfo.cbSize = sizeof(devInfo);
	for(int devIndex=0;SetupDiEnumDeviceInfo(devs,devIndex,&devInfo);devIndex++)
	{
		LPTSTR *hwIds = NULL;
		if(CM_Get_Device_ID(devInfo.DevInst,InstanceId,MAX_DEVICE_ID_LEN,0)!=CR_SUCCESS) {
			InstanceId[0] = TEXT('\0');
		};
		hwIds = GetDevMultiSz(devs,&devInfo,SPDRP_HARDWAREID);
		if (!hwIds || !(*hwIds))
			continue;
		    
		// Test is this is the vJoy device (Comparing HWid)
		// If it is then restart it and return TRUE
		int cmp = _tcsnicmp(*hwIds,hwid, _tcslen(hwid));
		DelMultiSz((PZPWSTR)hwIds);
		if (!cmp)
		{
			 _stprintf_s(prt, MAX_PATH, "FindInstalled: DevID[%d] is %s", devIndex, InstanceId);
			StatusMessage( NULL, prt,  INFO);
			// This is done for rare case where the child was removed manually
			ChangeDeviceStat(devs, &devInfo, Enable, Disable);
			out = TRUE;
			break;
		}

	};

	return out;
}


UINT Install(LPCTSTR inf, LPCTSTR hwid, TCHAR *InstanceId, BOOL quiet)
/*
	Install current vJoy driver according to the given inf file and HW id
	If this exact driver is installed then fail

*/
{
    HDEVINFO DeviceInfoSet = INVALID_HANDLE_VALUE;
    SP_DEVINFO_DATA DeviceInfoData;
    GUID ClassGUID;
    TCHAR ClassName[MAX_CLASS_NAME_LEN];
    TCHAR hwIdList[LINE_LEN+4];
    TCHAR InfPath[MAX_PATH];
    UINT failcode = EXIT_FAIL;
	TCHAR ErrMsg[1000];


    if(!inf[0]) 
	{
		 _stprintf_s(prt, MAX_PATH, "Install: Illegal Inf file");
		StatusMessage( NULL, prt,  ERR);
		return EXIT_FAIL;
	}

    if(!hwid[0]) 
	{
		_stprintf_s(prt, MAX_PATH, "Install: Illegal  Hardware Id");
		StatusMessage( NULL, prt,  ERR);

		return EXIT_FAIL;
	}

    //
    // Inf must be a full pathname
    //
    if(GetFullPathName(inf,MAX_PATH,InfPath,NULL) >= MAX_PATH) 	
	{
		_stprintf_s(prt, MAX_PATH, "Install: InfPath too long");
		StatusMessage( NULL, prt,  ERR);
		return EXIT_FAIL;
	}
	_stprintf_s(prt, MAX_PATH, "Install: GetFullPathName --> %s ", InfPath);
	StatusMessage( NULL, prt,  INFO);

    //
    // List of hardware ID's must be double zero-terminated
    //
    ZeroMemory(hwIdList,sizeof(hwIdList));
    if (FAILED(StringCchCopy(hwIdList,LINE_LEN,hwid))) 
	{
		_stprintf_s(prt, MAX_PATH, "Install: Function StringCchCopy failed");
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	 _stprintf_s(prt, MAX_PATH, "Install: hwIdList --> %s", hwIdList);
	StatusMessage( NULL, prt,  INFO);

    //
    // Use the INF File to extract the Class GUID.
    //
    if (!SetupDiGetINFClass(InfPath,&ClassGUID,ClassName,sizeof(ClassName)/sizeof(ClassName[0]),0))
	{
		 GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "SetupDiGetINFClass failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	};
	_stprintf_s(prt, MAX_PATH, "Install: SetupDiGetINFClass --> Class Name %s", ClassName);
	StatusMessage( NULL, prt,  INFO);

    //
    // Create the container for the to-be-created Device Information Element.
    //
    DeviceInfoSet = SetupDiCreateDeviceInfoList(&ClassGUID,0);
    if(DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		 GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "SetupDiCreateDeviceInfoList failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	};
	_stprintf_s(prt, MAX_PATH, "Install: SetupDiCreateDeviceInfoList OK");
	StatusMessage( NULL, prt,  INFO);

    //
    // Now create the element.
    // Use the Class GUID and Name from the INF file.
    //
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiCreateDeviceInfo(DeviceInfoSet, ClassName, &ClassGUID, NULL,  0, DICD_GENERATE_ID, &DeviceInfoData))
	{
		 GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "SetupDiCreateDeviceInfo failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	};
	//_ftprintf(stream,">> Install: SetupDiCreateDeviceInfo  OK\n");
	_stprintf_s(prt, MAX_PATH, "Install: SetupDiCreateDeviceInfo  OK");
	StatusMessage( NULL, prt,  INFO);

    //
    // Add the HardwareID to the Device's HardwareID property.
    //
    if(!SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,(LPBYTE)hwIdList, (lstrlen(hwIdList)+1+1)*sizeof(TCHAR)))
    {
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] SetupDiSetDeviceRegistryProperty failed with error: %s\n", ErrMsg);
		_stprintf_s(prt, MAX_PATH, "SetupDiSetDeviceRegistryProperty failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	//_ftprintf(stream,">> Install: SetupDiSetDeviceRegistryProperty  OK\n");
	_stprintf_s(prt, MAX_PATH, "Install: SetupDiSetDeviceRegistryProperty  OK");
	StatusMessage( NULL, prt,  INFO);

    //
    // Transform the registry element into an actual devnode
    // in the PnP HW tree.
    //
    if (!SetupDiCallClassInstaller(DIF_REGISTERDEVICE, DeviceInfoSet, &DeviceInfoData))
    {
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] SetupDiCallClassInstaller failed with error: %s\n", ErrMsg);
		_stprintf_s(prt, MAX_PATH, "SetupDiCallClassInstaller failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	//_ftprintf(stream,">> Install: SetupDiCallClassInstaller  OK\n");
	_stprintf_s(prt, MAX_PATH, "Install: SetupDiCallClassInstaller  OK");
	StatusMessage( NULL, prt,  INFO);


#define UPDATE
#ifdef UPDATE
	//
	// update the driver for the device we just created
	//
	//_ftprintf(stream,">> Install: Starting cmdUpdate\n");
	_stprintf_s(prt, MAX_PATH, "Install: Starting cmdUpdate");
	StatusMessage( NULL, prt,  INFO);
	failcode = cmdUpdate( INSTALLFLAG_FORCE, inf,  hwid, quiet);
	//_ftprintf(stream,">> Install: Finished cmdUpdate\n");
	_stprintf_s(prt, MAX_PATH, "Install: Finished cmdUpdate");
	StatusMessage( NULL, prt,  INFO);
#endif //UPDATE

	// Get Instence ID
	if (!SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, InstanceId, 1000, NULL))
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] SetupDiGetDeviceInstanceId failed with error: %s\n", ErrMsg);
		_stprintf_s(prt, MAX_PATH, "SetupDiGetDeviceInstanceId failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	//_ftprintf(stream,">> Install: SetupDiGetDeviceInstanceId (Device Instance Path=%s) OK\n", InstanceId);
	_stprintf_s(prt, MAX_PATH, "Install: SetupDiGetDeviceInstanceId (Device Instance Path=%s) OK", InstanceId);
	StatusMessage( NULL, prt,  INFO);

#ifndef  UPDATE
	InstallDriverOnDevice(InstanceId, inf);
#endif // !UPDATE

final:

	if (DeviceInfoSet != INVALID_HANDLE_VALUE) {
		SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	}

    return failcode;
}


int AssignAllCompatibleId(TCHAR * ParentDeviceNode, TCHAR * CompatibleId)
{
	int		assigned=0;
	DEVINST	ParentDevInst;

	BOOL res = GetParentDevInst(ParentDeviceNode, CompatibleId, &ParentDevInst);
	if (!res)
		return 0;

	res = AssignCompatibleId(CompatibleId, &ParentDevInst, TRUE);

	while (res)
	{
		res = AssignCompatibleId(CompatibleId, &ParentDevInst, FALSE);
		assigned++;
	};

	return assigned;

}
BOOL GetParentDevInst(TCHAR * ParentDeviceNode, TCHAR * CompatibleId, DEVINST * ParentDevInst)
{
	TCHAR buf[MAX_DEVICE_ID_LEN]; // Place the Device Node here
	//DEVINST  dnDevInst;
	CONFIGRET  rType;
	//TCHAR ErrMsg[1000];
	//SP_DEVINFO_DATA  DeviceInfoData;
	//BOOL	rDi;

	if (!ParentDeviceNode)
	{
		_stprintf_s(prt, MAX_PATH, "GetParentDevInst: Empty ParentDeviceNode");
		StatusMessage( NULL, prt,  ERR);
		return NULL;
	};

	_stprintf_s(prt, MAX_PATH, "GetParentDevInst: ParentDeviceNode = %s , CompatibleId = %s", ParentDeviceNode, CompatibleId);
	StatusMessage( NULL, prt,  INFO);

	_tcsncpy_s(buf, MAX_DEVICE_ID_LEN, ParentDeviceNode, MAX_DEVICE_ID_LEN);
	// Obtains device instance handle to the parent's device node (dnDevInst)
	// that is associated with a specified device instance identifier (buf)
	rType = CM_Locate_DevNode(ParentDevInst, buf ,0);
	if (rType != CR_SUCCESS)
	{
		_stprintf_s(prt, MAX_PATH, "GetParentDevInst: Function CM_Locate_DevNode failed with error: %08X", rType);
		StatusMessage( NULL, prt,  ERR);
		return NULL;
	}
	else
	{
		_stprintf_s(prt, MAX_PATH, "GetParentDevInst: Function CM_Locate_DevNode OK");
		StatusMessage( NULL, prt,  INFO);
	};

	return TRUE;
}


BOOL AssignCompatibleId(TCHAR * CompatibleId, DEVINST  * pdnDevInst , BOOL First)
{
	DEVINST  childDevInst=0, dnDevInst = *pdnDevInst;
	CONFIGRET  rType;
	TCHAR ErrMsg[1000];
	SP_DEVINFO_DATA  DeviceInfoData;
	BOOL	rDi;


#if 0
	if (!ParentDeviceNode)
	{
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Empty ParentDeviceNode");
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	};

	_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: ParentDeviceNode = %s , CompatibleId = %s", ParentDeviceNode, CompatibleId);
	StatusMessage( NULL, prt,  INFO);
	_tcsncpy_s(buf, MAX_DEVICE_ID_LEN, ParentDeviceNode, MAX_DEVICE_ID_LEN);
	// Obtains device instance handle to the parent's device node (dnDevInst)
	// that is associated with a specified device instance identifier (buf)
	rType = CM_Locate_DevNode(&dnDevInst, buf ,0);
	if (rType != CR_SUCCESS)
	{
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X", rType);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
	{
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Locate_DevNode OK");
		StatusMessage( NULL, prt,  INFO);
	};
#endif


	if (First)
	{
		// Retrieve a device instance handle to the first child node of the parent device node
		// Using the instance handle, retrieve the device instance ID for a child
		rType = CM_Get_Child(&childDevInst, dnDevInst, 0);
		if (rType != CR_SUCCESS)
		{
			_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Child failed with error: %08X", rType);
			StatusMessage( NULL, prt,  ERR);
			return FALSE;
		}
		else
		{
			_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Child OK");
			StatusMessage( NULL, prt,  INFO);
		};
	}
	else
	{
		// Retrieve a device instance handle to the next child node of the parent device node
		// Using the instance handle, retrieve the device instance ID for a child
		rType = CM_Get_Sibling(&childDevInst, dnDevInst, 0);
		if (rType != CR_SUCCESS)
		{
			_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Sibling failed with error: %08X", rType);
			StatusMessage( NULL, prt,  ERR);
			return FALSE;
		}
		else
		{
			_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Sibling OK");
			StatusMessage( NULL, prt,  INFO);
		};
	}

	*pdnDevInst = childDevInst;

	ULONG  Len;
	rType = CM_Get_Device_ID_Size(&Len, childDevInst, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,"[E] AssignCompatibleId: Function CM_Get_Device_ID_Size failed with error: %08X\n", rType);
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Device_ID_Size failed with error: %08X", rType);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}	
	else
	{
		//_ftprintf(stream,">> AssignCompatibleId: Function CM_Get_Device_ID_Size OK\n");
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Device_ID_Size OK");
		StatusMessage( NULL, prt,  INFO);
	};


	Len+=2;
	TCHAR * DeviceInstanceId = new TCHAR[Len];
	rType = CM_Get_Device_ID(childDevInst, DeviceInstanceId, Len, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,"[E] AssignCompatibleId: Function CM_Get_Device_ID failed with error: %08X\n", rType);
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Device_ID failed with error: %08X", rType);
		StatusMessage( NULL, prt,  ERR);
		delete[] DeviceInstanceId;
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,">> AssignCompatibleId: Function CM_Get_Device_ID (Device Instance Path = %s) OK\n",DeviceInstanceId);
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Get_Device_ID (Device Instance Path = %s) OK",DeviceInstanceId);
		StatusMessage( NULL, prt,  INFO);
	};

	/*
		TODO: Make sure that it is a child that represents a top-level collection
		Such child Device Instance Path (DeviceInstanceId) will be "HID\HIDCLASS\*" or "HID\HIDCLASS&COL*"
		Raw PDO (for sideband communication will be of type "{D6E55CA0-1A2E-4234-AAF3-3852170B492F}\VJOYRAWPDO\*VJOYINSTANCE*"
		If this is a Raw PDO exit with TRUE so that the next sibleng will be called.
	*/
	TCHAR * start = _tcsstr(DeviceInstanceId, "HID\\HIDCLASS");
	if (start != DeviceInstanceId)
	{
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Device %s is not a joystick device - skip",DeviceInstanceId);
		StatusMessage( NULL, prt,  INFO);
		return TRUE;
	}



	// Refresh child devnode
	rType = CM_Reenumerate_DevNode(childDevInst, CM_REENUMERATE_RETRY_INSTALLATION );
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,"[E] AssignCompatibleId: Function CM_Reenumerate_DevNode[1] failed with error: %08X\n", rType);
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Reenumerate_DevNode[1] failed with error: %08X", rType);
		StatusMessage( NULL, prt,  ERR);
	}
	else
	{
		//_ftprintf(stream,">> AssignCompatibleId: Function CM_Reenumerate_DevNode[1] OK\n");
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Reenumerate_DevNode[1] OK");
		StatusMessage( NULL, prt,  INFO);
	};

	// Test the existance of the child devnode
		rType = CM_Locate_DevNode(&childDevInst, DeviceInstanceId, CM_LOCATE_DEVNODE_NORMAL);

#if 0	// No point using childDevInst if function CM_Locate_DevNode() fails
		if (rType == CR_NO_SUCH_DEVNODE)
		{
			_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: DeviceInstanceId %s is missing", DeviceInstanceId);
			StatusMessage(NULL, prt, INFO);
			rType = CM_Reenumerate_DevNode(childDevInst, CM_REENUMERATE_RETRY_INSTALLATION);
			if (rType != CR_SUCCESS)
			{
				_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Reenumerate_DevNode failed with error: %08X", rType);
				StatusMessage(NULL, prt, ERR);
				delete DeviceInstanceId;
				return FALSE;
			}
			else
			{
				//_ftprintf(stream,">> AssignCompatibleId: Function CM_Reenumerate_DevNode  OK\n");
				_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Reenumerate_DevNode  OK");
				StatusMessage(NULL, prt, INFO);
			};

		}
		else
#endif // 0

			if (rType != CR_SUCCESS)
			{
				//_ftprintf(stream,"[E] AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X\n", rType);
				_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X", rType);
				StatusMessage(NULL, prt, ERR);
				delete[] DeviceInstanceId;
				return FALSE;
			}
			else
			{
				//_ftprintf(stream,">> AssignCompatibleId: Function CM_Locate_DevNode (Device Instance Path = %s) OK\n",DeviceInstanceId);
				_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function CM_Locate_DevNode (Device Instance Path = %s) OK", DeviceInstanceId);
				StatusMessage(NULL, prt, INFO);
			};



		// Accessing the HW registry key
		HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
		if (DeviceInfoSet == INVALID_HANDLE_VALUE)
		{
			GetErrorString(ErrMsg, 1000);
			//_ftprintf(stream,"[E] SetupDiCreateDeviceInfoList failed with error: %s\n", ErrMsg);
			_stprintf_s(prt, MAX_PATH, "SetupDiCreateDeviceInfoList failed with error: %s", ErrMsg);
			StatusMessage(NULL, prt, ERR);
			delete[] DeviceInstanceId;
			return FALSE;
		}
		else
		{
			//_ftprintf(stream,">> AssignCompatibleId: Function SetupDiCreateDeviceInfoList OK\n");
			_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function SetupDiCreateDeviceInfoList OK");
		StatusMessage( NULL, prt,  INFO);
	};


	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	rDi = SetupDiOpenDeviceInfo(DeviceInfoSet, DeviceInstanceId, NULL, 0, &DeviceInfoData);
	if (!rDi)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] SetupDiOpenDeviceInfo failed with error: %s\n", ErrMsg);
		_stprintf_s(prt, MAX_PATH, "SetupDiOpenDeviceInfo failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		delete[] DeviceInstanceId;
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,">> AssignCompatibleId: Function SetupDiOpenDeviceInfo OK\n");
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function SetupDiOpenDeviceInfo OK");
		StatusMessage( NULL, prt,  INFO);
	};


	delete[] DeviceInstanceId;

	// Setup Compatible ID (No need?)
	TCHAR PropBufferClass[LINE_LEN];
	ZeroMemory(PropBufferClass,sizeof(PropBufferClass));
    StringCchCopy(PropBufferClass,LINE_LEN,CompatibleId);

	rDi = SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, (LPBYTE)PropBufferClass, sizeof(PropBufferClass));
	if (!rDi)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] SetupDiSetDeviceRegistryProperty failed with error: %s\n", ErrMsg);
		_stprintf_s(prt, MAX_PATH, "SetupDiSetDeviceRegistryProperty failed with error: %s", ErrMsg);
		StatusMessage(NULL, prt, WARN);
		//return FALSE;	  -- Windows 10 always fails here!
	}
	else
	{
		//_ftprintf(stream,">> AssignCompatibleId: Function SetupDiSetDeviceRegistryProperty OK\n");
		_stprintf_s(prt, MAX_PATH, "AssignCompatibleId: Function SetupDiSetDeviceRegistryProperty OK");
		StatusMessage( NULL, prt,  INFO);
	};

	//_ftprintf(stream,">> AssignCompatibleId was OK\n");
	_stprintf_s(prt, MAX_PATH, "AssignCompatibleId was OK");
	StatusMessage( NULL, prt,  INFO);
	return TRUE;

}


int cmdUpdateNI( __in DWORD Flags, LPCTSTR inf, LPCTSTR hwid )
/*++

Routine Description:
    UPDATE (non interactive version)
    update driver for existing device(s)

Arguments:

    BaseName  - name of executable
    Machine   - machine name, must be NULL
    argc/argv - remaining parameters

Return Value:

    EXIT_xxxx

--*/
{
    //
    // turn off interactive mode while doing the update
    //
    HMODULE setupapiMod = NULL;
    SetupSetNonInteractiveModeProto SetNIFn;
    int res;
    BOOL prev;
	TCHAR InfPath[MAX_PATH];

	//
	// Inf must be a full pathname
	//
	if(GetFullPathName(inf,MAX_PATH,InfPath,NULL) >= MAX_PATH) 	
	{
		//_ftprintf(stream,"[E] cmdUpdateNI: InfPath too long\n");
		return FALSE;
	}
	//_ftprintf(stream,">> cmdUpdateNI: GetFullPathName --> %s \n", InfPath);
	_stprintf_s(prt, MAX_PATH, "cmdUpdateNI: GetFullPathName --> %s", InfPath);
	StatusMessage( NULL, prt,  INFO);
    setupapiMod = LoadLibrary(TEXT("setupapi.dll"));
    if(!setupapiMod) 
	{
		//_ftprintf(stream,"[E] cmdUpdateNI: LoadLibrary failed to load setupapi.dll\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdateNI: LoadLibrary failed to load setupapi.dll");
		StatusMessage( NULL, prt,  ERR);
        return cmdUpdate(Flags,InfPath,  hwid, FALSE);
    }
    SetNIFn = (SetupSetNonInteractiveModeProto)GetProcAddress(setupapiMod,SETUPSETNONINTERACTIVEMODE);
    if(!SetNIFn)
    {
        FreeLibrary(setupapiMod);
		//_ftprintf(stream,"[E] cmdUpdateNI: GetProcAddress failed to get SETUPSETNONINTERACTIVEMODE\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdateNI: GetProcAddress failed to get SETUPSETNONINTERACTIVEMODE");
		StatusMessage( NULL, prt,  ERR);
        return cmdUpdate(Flags,InfPath,  hwid, FALSE);
    }

    prev = SetNIFn(TRUE);
	//_ftprintf(stream,">> cmdUpdateNI: SetNIFn(TRUE) returned %d\n", prev);
	_stprintf_s(prt, MAX_PATH, "cmdUpdateNI: SetNIFn(TRUE) returned %d", prev);
	StatusMessage( NULL, prt,  INFO);
	//_ftprintf(stream,">> cmdUpdateNI: Calling cmdUpdate with InfPath=%s\n", InfPath);
	_stprintf_s(prt, MAX_PATH, "cmdUpdateNI: Calling cmdUpdate with InfPath=%s", InfPath);
	StatusMessage( NULL, prt,  INFO);
    res = cmdUpdate(Flags,TEXT(InfPath),  hwid, FALSE);
	//_ftprintf(stream,">> cmdUpdateNI: cmdUpdate returned %d\n", res);
	_stprintf_s(prt, MAX_PATH, "cmdUpdateNI: cmdUpdate returned %d", res);
	StatusMessage( NULL, prt,  INFO);
    SetNIFn(prev);
    FreeLibrary(setupapiMod);
	//_ftprintf(stream,">> cmdUpdateNI returns value %d\n", res);
	_stprintf_s(prt, MAX_PATH, "cmdUpdateNI returns value %d", res);
	StatusMessage( NULL, prt,  INFO);
    return res;
}

int cmdUpdate( __in DWORD Flags, LPCTSTR inf, LPCTSTR hwid, BOOL quiet)
/*++

Routine Description:
    UPDATE
    update driver for existing device(s)

Arguments:

    BaseName  - name of executable
    Machine   - machine name, must be NULL
    argc/argv - remaining parameters

Return Value:

    EXIT_xxxx

--*/
{
    HMODULE newdevMod = NULL;
    int failcode = EXIT_FAIL;
    UpdateDriverForPlugAndPlayDevicesProto UpdateFn;
    BOOL reboot = FALSE;
    DWORD flags = 0;
    DWORD res;
    TCHAR InfPath[MAX_PATH];
	TCHAR ErrMsg[1000];
	CONFIGRET ret;

    UNREFERENCED_PARAMETER(Flags);

    //
    // Inf must be a full pathname
    //
    res = GetFullPathName(TEXT(inf),MAX_PATH,InfPath,NULL);
    if((res >= MAX_PATH) || (res == 0)) {
		//
		// inf pathname too long
		//
		//_ftprintf(stream,"[E] cmdUpdate:  GetFullPathName for file %s returned: %d\n", InfPath, res);
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  GetFullPathName for file %s returned: %d", InfPath, res);
		StatusMessage( NULL, prt,  ERR);

        return EXIT_FAIL;
    }
	//_ftprintf(stream,">> cmdUpdate: GetFullPathName --> %s \n", InfPath);
	_stprintf_s(prt, MAX_PATH, "cmdUpdate: GetFullPathName --> %s ", InfPath);
	StatusMessage( NULL, prt,  INFO);

    if(GetFileAttributes(InfPath)==(DWORD)(-1)) {
        //
        // inf doesn't exist
        //
		//_ftprintf(stream,"[E] cmdUpdate:  GetFileAttributes failed\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  GetFileAttributes failed");
		StatusMessage( NULL, prt,  ERR);
        return EXIT_FAIL;
    }
	//_ftprintf(stream,">> cmdUpdate: Install: Starting cmdUpdate \n");
	_stprintf_s(prt, MAX_PATH, "cmdUpdate: Install: Starting cmdUpdate");
	StatusMessage( NULL, prt,  INFO);

    inf = InfPath;
    flags |= INSTALLFLAG_FORCE;

    //
    // make use of UpdateDriverForPlugAndPlayDevices
    //
    newdevMod = LoadLibrary(TEXT("newdev.dll"));
    if(!newdevMod) 
	{
		//_ftprintf(stream,"[E] cmdUpdate:  Failed to load file newdev.dll\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  Failed to load file newdev.dll");
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	else
	{
		//_ftprintf(stream,">> cmdUpdate:  File newdev.dll loaded OK\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  File newdev.dll loaded OK");
		StatusMessage( NULL, prt,  INFO);
	};

    UpdateFn = (UpdateDriverForPlugAndPlayDevicesProto)GetProcAddress(newdevMod,UPDATEDRIVERFORPLUGANDPLAYDEVICES);
    if(!UpdateFn)
    {
		//_ftprintf(stream,"[E] cmdUpdate:  Failed to get UPDATEDRIVERFORPLUGANDPLAYDEVICES\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  Failed to get UPDATEDRIVERFORPLUGANDPLAYDEVICES");
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	else
	{
		//_ftprintf(stream,">> cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES got OK\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES got OK");
		StatusMessage( NULL, prt,  INFO);
	};


	// Update Driver
	ret = CMP_WaitNoPendingInstallEvents(30000);
	if (ret == WAIT_OBJECT_0)
	{
		//_ftprintf(stream,">> cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_OBJECT_0\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_OBJECT_0");
		StatusMessage( NULL, prt,  INFO);
	}
	else if (ret == WAIT_TIMEOUT)
	{
		//_ftprintf(stream,">> cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_TIMEOUT\n");
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_TIMEOUT");
		StatusMessage( NULL, prt,  INFO);
	}
	else
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] cmdUpdate:  CMP_WaitNoPendingInstallEvents failed with error: %s\n", ErrMsg);
 		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  CMP_WaitNoPendingInstallEvents failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
       goto final;
    }

	PBOOL preboot = NULL;
	if (quiet)
		preboot = &reboot;

    if(!UpdateFn(NULL,hwid,InfPath,flags, preboot))
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,"[E] cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) failed with error: %s\n", hwid, InfPath, ErrMsg);
 		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) failed with error: %s", hwid, InfPath, ErrMsg);
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	else
	{
		//_ftprintf(stream,">> cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) executed OK\n", hwid, InfPath);
		_stprintf_s(prt, MAX_PATH, "cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) executed OK", hwid, InfPath);
		StatusMessage( NULL, prt,  INFO);
	};

	if (quiet)
		failcode = reboot ? EXIT_REBOOT : EXIT_OK;
	else
		failcode = EXIT_OK;

final:

    if(newdevMod) {
        FreeLibrary(newdevMod);
	}

	//_ftprintf(stream,">> cmdUpdate returns code %d\n", failcode);
	_stprintf_s(prt, MAX_PATH, "cmdUpdate returns code %d", failcode);
	StatusMessage( NULL, prt,  INFO);
	return failcode;
}



// Remove vJoy device-node and its children
int RemoveDevice(TCHAR *ParentDeviceNode, BOOL DelInf)
{
	DEVINST  dnDevInst;
	CONFIGRET  rType;


	/////////////////////////////////////////////////////////////////////////////////////////////////
	_stprintf_s(prt, MAX_PATH, "RemoveDevice: ParentDeviceNode = %s", ParentDeviceNode);
	StatusMessage(NULL, prt, INFO);

	// Obtains device instance handle to the device node (dnDevInst)
	// that is associated with a specified device instance identifier
	rType = hlp_LocateDevNode(&dnDevInst, ParentDeviceNode);
	if (rType != CR_SUCCESS)
		return -102;


	// Remove all devices under this device node (including this device node)
	return RemoveAllDevices(dnDevInst, TRUE, DelInf);
}


// Remove all device nodes starting with hDevInst
// 
// hDevInst: Handle to device node to remove
// isRoot: TRUE if this device node is the root of the operation. 
//         If this is the root, Remove all of its children then remove the root
//         If this is NOT the root, Remove its sibling then remove this device node
// DelInf: If TRUE - remove driver from driver store
//
// Return 0 if success.
// Return negative error code id failure
int   RemoveAllDevices(DEVINST hDevInst, BOOL isRoot, BOOL DelInf)
{

	DEVINST h;
	CONFIGRET cr;

	// If Device does not exist just return
	if (!hDevInst)
		return 0;

	// Enable vJoy device node because the removal works only on enabled devices
	cr = CM_Enable_DevNode(hDevInst, 0);
	if (cr != CR_SUCCESS)
		return -1010;

	// If this is a root device - get the first child.
	// If this is NOT the root device - get the next sibling.
	if (isRoot)
		cr = CM_Get_Child(&h, hDevInst, 0);
	else
		cr = CM_Get_Sibling(&h, hDevInst, 0);

	// If the next device node has been obtained - remove it
	if (cr == CR_SUCCESS)
		RemoveAllDevices(h, FALSE, FALSE);

	// Remove THIS device node
	return 	RemDev( hDevInst,  DelInf);
}

int RemDev(DEVINST hDevInst, BOOL DelInf)
{

	TCHAR * DeviceInstanceId;
	SP_DEVINFO_DATA  DeviceInfoData;
	BOOL rDi;
	CONFIGRET cr;


	// Creating a clean Device Info Set
	HDEVINFO DeviceInfoSet = hlp_CreateDeviceInfoList();
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
		return -101;


	// Obtains Device ID from Device Handle
	int iRet = hlp_DeviceHandle2ID(hDevInst, &DeviceInstanceId);
	if (iRet)
		return iRet;


	// Get the Device Info Data
	rDi = hlp_OpenDeviceInfo(DeviceInfoSet, DeviceInstanceId, &DeviceInfoData);
	if (!rDi)
		return -105;
	delete[] DeviceInstanceId;

	// If Requested to Delete INF file from inf directoy - Get the name of file OEMx.INF  and remove it
	if (DelInf)
	{		
		rDi = hlp_UninstallOEMInf(DeviceInfoSet, DeviceInfoData);
		if (!rDi)
			return -116;
	}

	// Remove
	rDi = hlp_RemoveDevice(DeviceInfoSet, &DeviceInfoData);
	if (!rDi)
		return -106;


	return 0;
}

BOOL ChangeDeviceStat(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in BOOL Enable, __in BOOL Disable)
/*++

Routine Description:

Based on Decxon Callback for use by Enable/Disable/Restart
Invokes DIF_PROPERTYCHANGE with correct parameters
uses SetupDiCallClassInstaller so cannot be done for remote devices
Don't use CM_xxx API's, they bypass class/co-installers and this is bad.

In Enable case, we try global first, and if still disabled, enable local

Arguments:

Devs    )_ uniquely identify the device
DevInfo )
Index    - index of device
Context  - GenericContext

Return Value:

EXIT_xxxx

--*/
{
	SP_PROPCHANGE_PARAMS pcp;
	//GenericContext *pControlContext;
	SP_DEVINSTALL_PARAMS devParams;

	_stprintf_s(prt, MAX_PATH, "ChangeDeviceStat: Starting ...");
	StatusMessage(NULL, prt, INFO);


	//////////////// DISABLE ///////////////////////////////
	//
	// operate on config-specific profile
	//
	if (Disable)
	{
		pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		pcp.StateChange = DICS_DISABLE; // DICS_STOP;
		pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
		pcp.HwProfile = 0;


		if (!SetupDiSetClassInstallParams(Devs, DevInfo, &pcp.ClassInstallHeader, sizeof(pcp)) ||
			!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, Devs, DevInfo))
			// failed to invoke DIF_PROPERTYCHANGE
			return FALSE;

		if (!Enable)
		{
			// Let client release handle  -  Temporary solution 
			Sleep(1000);
			devParams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
			SetupDiGetDeviceInstallParams(Devs, DevInfo, &devParams);
			return TRUE;
		}
	}

	//////////////// ENABLE ///////////////////////////////
	//
	// enable both on global and config-specific profile
	// do global first and see if that succeeded in enabling the device
	// (global enable doesn't mark reboot required if device is still
	// disabled on current config whereas vice-versa isn't true)
	//
#if 0
	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.StateChange = DICS_START ;// DICS_ENABLE;
	pcp.Scope = DICS_FLAG_GLOBAL;
	pcp.HwProfile = 0;
	//
	// don't worry if this fails, we'll get an error when we try config-
	// specific.
	if(SetupDiSetClassInstallParams(Devs,DevInfo,&pcp.ClassInstallHeader,sizeof(pcp))) 
		BOOL ok = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,Devs,DevInfo);
#endif

	//
	// now enable on config-specific
	//
	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.StateChange = DICS_ENABLE;// DICS_START ;
	pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
	pcp.HwProfile = 0;

	// Let client release handle  -  Temporary solution 
	Sleep(1000);

	if(!SetupDiSetClassInstallParams(Devs,DevInfo,&pcp.ClassInstallHeader,sizeof(pcp)) ||
		!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,Devs,DevInfo))
		// failed to invoke DIF_PROPERTYCHANGE
		return FALSE;
	else
	{
		devParams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
		SetupDiGetDeviceInstallParams(Devs,DevInfo, &devParams);
		return TRUE;
	};

}

BOOL isPPJoyInstalled()
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];

	BOOL installed = FindInstalled(HWID_PPJOY0, InstanceId);
	return installed;
}

BOOL isvJoyInstalled()
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];

	GetDevHwId(0, NULL, DeviceHWID);
	BOOL installed = FindInstalled(DeviceHWID, InstanceId);
	return installed;
}

BOOL isvJoyInstalledSpecific(USHORT Revision)
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];
	BOOL installed = TRUE;

	_stprintf_s(DeviceHWID, MAX_PATH, HWID_TMPLT,VENDOR_N_ID, PRODUCT_N_ID, Revision);
	installed = FindInstalled(DeviceHWID, InstanceId);
	// Repair(DeviceHWID, _T("vjoy.inf")); // Test it
	return installed;
}

BOOL Enable(USHORT Revision)
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];
	BOOL en = TRUE;

	if (Revision)
		_stprintf_s(DeviceHWID, MAX_PATH, HWID_TMPLT, VENDOR_N_ID, PRODUCT_N_ID, Revision);
	else
		_stprintf_s(DeviceHWID, MAX_PATH, HWID_TMPLT1, VENDOR_N_ID, PRODUCT_N_ID);

	en = EnableInstalled(DeviceHWID, InstanceId);
	return en;
}

BOOL Disable(USHORT Revision)
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];
	BOOL dis = TRUE;

	_stprintf_s(DeviceHWID, MAX_PATH, HWID_TMPLT, VENDOR_N_ID, PRODUCT_N_ID, Revision);
	dis = DisableInstalled(DeviceHWID, InstanceId);
	return dis;
}


void RefreshvJoy(void)
{
	//TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];

	if (GetDevHwId(0, NULL, DeviceHWID))
		Repair(DeviceHWID, INFFILE);
}

void RefreshvJoySpecific(USHORT Revision)
{
	//TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];

	_stprintf_s(DeviceHWID, MAX_PATH, HWID_TMPLT,VENDOR_N_ID, PRODUCT_N_ID, Revision);
	Repair(DeviceHWID, INFFILE);
	return;
}



/* Major Functions */
int Installation(LPCTSTR DeviceHWID, TCHAR * InfFile, BOOL quiet)
{
	//BOOL	rDi;
	int		assigned;
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	GUID ClassGUID = GUID_NULL;
	TCHAR InfPath[MAX_PATH];

	//////////////// Test if PPJoy (virtual joystick 1)///////////////////////////
	/*if (isPPJoyInstalled())
	{
		//_ftprintf(stream,"[E] Install failed: PPJoy (virtual joystick 1) is already installed \n");
		_stprintf_s(prt, MAX_PATH, "Install failed: PPJoy (virtual joystick 1) is already installed");
		StatusMessage( NULL, prt,  ERR);
		return -7;
		
	}*/
	//////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////
	// Test if device already installed.
	// Older device is not recognised as THIS device
	InstanceId[0] = '\0';
	if (FindInstalled(DeviceHWID,InstanceId) && !quiet)
	{
		_stprintf_s(prt, MAX_PATH, "Device already installed - Install failed");
		StatusMessage( NULL, prt,  WARN);
		return -2;
	}
	else
	{	// Not installed (or older device is installed) so install it

		// Find if an older device is already installed
		TCHAR DeviceHWID_NoRev[MAX_PATH] = {NULL};
		RemoveRevStr(DeviceHWID, DeviceHWID_NoRev);
		if (FindInstalled(DeviceHWID_NoRev,InstanceId))
		{
			_stprintf_s(prt, MAX_PATH, "Different vJoy installed - Removing it");
			StatusMessage( NULL, prt,  WARN);
			RemoveDevice(InstanceId, TRUE);
		};

		UINT Install_res = Install(InfFile, DeviceHWID, InstanceId, quiet);

		if (Install_res == EXIT_OK)
		{
			_stprintf_s(prt, MAX_PATH, "Install() OK - No need to reboot");
			StatusMessage(NULL, prt, INFO);
		}

		else if (Install_res == EXIT_REBOOT)
 		{
			_stprintf_s(prt, MAX_PATH, "Install() OK - Must reboot");
			StatusMessage( NULL, prt,  WARN);
			return -8;
		}

		else if ((Install_res != EXIT_OK) && (Install_res != EXIT_REBOOT))
		{
			_stprintf_s(prt, MAX_PATH, "Install failed");
			StatusMessage( NULL, prt,  ERR);
			return -1;
		};


	};
	//
	/////////////////////////////////////

	assigned = AssignAllCompatibleId(InstanceId, COMPATIBLE_ID);
	if (!assigned)
	{
		RemoveDevice(InstanceId, TRUE);
		return -5;
	};
	GetInputInfFullPath(InfPath);
	 _stprintf_s(prt, MAX_PATH, "installation(): InputInfFullPath --> %s", InfPath);
	StatusMessage( NULL, prt,  INFO);
	cmdUpdateNI( INSTALLFLAG_FORCE ,InfPath, COMPATIBLE_ID );
	return 0;
}

int Removal(TCHAR * DeviceHWID, TCHAR * InfFile, BOOL DelInf)
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	UNREFERENCED_PARAMETER(InfFile);

	/////////////////////////////////////
	// Test if device already installed. 
	InstanceId[0] = '\0';
	if (!FindInstalled(DeviceHWID,InstanceId))
	{
		//_ftprintf(stream,">> Removal(): Device %s not installed\n",DeviceHWID);
		_stprintf_s(prt, MAX_PATH, "Removal(): Device %s not installed",DeviceHWID);
		StatusMessage( NULL, prt,  WARN);
		return -100;
	};

	// Installed - Continue;
	//_ftprintf(stream,">> Removal(): Device %s is installed (InstanceId %s) -- Removing\n",DeviceHWID, InstanceId);
	_stprintf_s(prt, MAX_PATH, "Removal(): Device %s is installed (InstanceId %s) -- Removing",DeviceHWID, InstanceId);
	StatusMessage( NULL, prt,  INFO);
	int result = RemoveDevice(InstanceId, DelInf);
	return result;
}
int Repair(TCHAR * DeviceHWID, TCHAR * InfFile)
{
	//BOOL	rDi;
	int assigned;
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	GUID ClassGUID = GUID_NULL;
	TCHAR InfPath[MAX_PATH];

	//////////////// Test if PPJoy (virtual joystick 1)///////////////////////////
	//if (isPPJoyInstalled())
	//{
	//	//_ftprintf(stream,"[E] Repair failed: PPJoy (virtual joystick 1) is already installed \n");
	//	_stprintf_s(prt, MAX_PATH, "Repair failed: PPJoy (virtual joystick 1) is already installed ");
	//	StatusMessage( NULL, prt,  ERR);
	//	return -7;
	//}
	//////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////
	// Test if device already installed. 
	InstanceId[0] = '\0';
	if (FindInstalled(DeviceHWID,InstanceId))
	{
		_stprintf_s(prt, MAX_PATH, "Device already installed - Repairing");
		StatusMessage( NULL, prt,  WARN);
	}
	else
	{	// Not installed so install it
		_stprintf_s(prt, MAX_PATH, "Device not installed - Installing");
		StatusMessage( NULL, prt,  INFO);
		if (!Install(InfFile, DeviceHWID,InstanceId, FALSE))
		{
			_stprintf_s(prt, MAX_PATH, "Install failed");
			StatusMessage( NULL, prt,  ERR);
			return -1;
		};
	};
	//
	/////////////////////////////////////

	assigned = AssignAllCompatibleId(InstanceId, COMPATIBLE_ID);
	if (!assigned)
	{
		RemoveDevice(InstanceId, FALSE);
		return -5;
	};
	GetInputInfFullPath(InfPath);
	//_ftprintf(stream,">> Repair(): InputInfFullPath --> %s \n", InfPath);
	_stprintf_s(prt, MAX_PATH, "Repair(): InputInfFullPath --> %s", InfPath);
	StatusMessage( NULL, prt,  INFO);
	cmdUpdateNI( INSTALLFLAG_FORCE ,InfPath, COMPATIBLE_ID );
	return 0;
}

/* Helper Functions */
BOOL GetErrorString(TCHAR * Msg, int Size)
{
	LPTSTR s;
	memset(Msg,' ', Size);
	DWORD errorcode = GetLastError();
	int nTChars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,  NULL, errorcode, 0, (LPTSTR)&s,  0, NULL);
	if (!nTChars)
	{
		_stprintf_s(Msg, MAX_PATH, TEXT("Unknown Error: %08x"),errorcode);
		return FALSE;
	};

	_tcsncpy_s(Msg, Size, s, Size);
	LocalFree(s);
	return TRUE;
}
BOOL GetInputInfFullPath(TCHAR * Str)
{

	//size_t ReturnValue;
	TCHAR  tbuffer[MAX_PATH];
	TCHAR * buffer;

	buffer = tbuffer;
	//buffer = _tgetenv(TEXT("WINDIR"));
	size_t	RequiredCount = 0;
	_tgetenv_s(&RequiredCount, buffer, MAX_PATH, TEXT("WINDIR"));

	_stprintf_s(Str, MAX_PATH, TEXT("%s\\Inf\\Input.inf"),buffer);
	return TRUE;
}

#if (KMDF_MINOR_VERSION != 005 &&  KMDF_MINOR_VERSION != 007)
__drv_allocatesMem(object)
LPTSTR * GetMultiSzIndexArray(__in __drv_aliasesMem LPTSTR MultiSz)
#else
LPTSTR * GetMultiSzIndexArray(__in  LPTSTR MultiSz)
#endif
/*++

Routine Description:

    Get an index array pointing to the MultiSz passed in

Arguments:

    MultiSz - well formed multi-sz string

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR scan;
    LPTSTR * array;
    int elements;

    for(scan = MultiSz, elements = 0; scan[0] ;elements++) {
        scan += lstrlen(scan)+1;
    }
    array = new LPTSTR[elements+2];
    if(!array) {
        return NULL;
    }
    array[0] = MultiSz;
    array++;
    if(elements) {
        for(scan = MultiSz, elements = 0; scan[0]; elements++) {
            array[elements] = scan;
            scan += lstrlen(scan)+1;
        }
    }
    array[elements] = NULL;
    return array;
}

#if (KMDF_MINOR_VERSION != 005 &&  KMDF_MINOR_VERSION != 007)
__drv_allocatesMem(object)
#endif
LPTSTR * GetDevMultiSz(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop)
/*++

Routine Description:

    Get a multi-sz device property
    and return as an array of strings

Arguments:

    Devs    - HDEVINFO containing DevInfo
    DevInfo - Specific device
    Prop    - SPDRP_HARDWAREID or SPDRP_COMPATIBLEIDS

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR buffer = NULL;
    DWORD reqSize = 16000;
    DWORD dataType;
    LPTSTR * array;
    DWORD szChars;
	BOOL bRes;


	// Getting the size of required buffer
#if 0
	bRegProp = SetupDiGetDeviceRegistryProperty(Devs, DevInfo, Prop, NULL, NULL, 0, &reqSize);
	DWORD err = GetLastError();
	if (err != ERROR_INSUFFICIENT_BUFFER)
		return NULL;

#endif // 0


	// Allocate buffer according to required size
    buffer = new TCHAR[(reqSize /sizeof(TCHAR))+2];
    if(!buffer)
		return NULL;

	// Get the string into the buffer 
	bRes = SetupDiGetDeviceRegistryProperty(Devs, DevInfo, Prop, &dataType, (LPBYTE)buffer, reqSize, &reqSize);
	if (!bRes || ((dataType != REG_SZ) && (dataType != REG_MULTI_SZ)))
		return NULL;

	szChars = reqSize / sizeof(TCHAR);
	buffer[szChars] = TEXT('\0');
	buffer[szChars + 1] = TEXT('\0');
	array = GetMultiSzIndexArray(buffer);
    if(array)
        return array;

    if(buffer) {
        delete [] buffer;
    }
    return NULL;
}

#if (KMDF_MINOR_VERSION != 005 &&  KMDF_MINOR_VERSION != 007)
void DelMultiSz(__in_opt __drv_freesMem(object) PZPWSTR Array)
#else
void DelMultiSz(__in_opt  PZPWSTR Array)
#endif

/*++

Routine Description:

    Deletes the string array allocated by GetDevMultiSz/GetRegMultiSz/GetMultiSzIndexArray

Arguments:

    Array - pointer returned by GetMultiSzIndexArray

Return Value:

    None

--*/
{
    if(Array) {
        Array--;
        if(Array[0]) {
            delete [] Array[0];
        }
        delete [] Array;
    }
}


VERBTYPE GetVerb(int argc, PZPWSTR argv)
{
	// Default
	if (argc == 1)
		return INSTALL;

	if ((((TCHAR *)argv[1])[0] == 'i') || (((TCHAR *)argv[1])[0] == 'I'))
		return INSTALL;
	if ((((TCHAR *)argv[1])[0] == 'u') || (((TCHAR *)argv[1])[0] == 'U'))
		return REMOVE;
	if ((((TCHAR *)argv[1])[0] == 'c') || (((TCHAR *)argv[1])[0] == 'C'))
		return CLEAN;
	if ((((TCHAR *)argv[1])[0] == 'r') || (((TCHAR *)argv[1])[0] == 'R'))
		return REPAIR;
	if ((((TCHAR *)argv[1])[0] == 'q') || (((TCHAR *)argv[1])[0] == 'Q'))
		return QUIET_I;

	return INVALID;
}
BOOL GetInfFile(int argc, PZPWSTR argv, TCHAR * InfFile)
{

	if (argc >=3)
		_stprintf_s(InfFile, MAX_PATH, (TCHAR *)argv[2]);
	else
		_stprintf_s(InfFile, MAX_PATH, INFFILE);

	return TRUE;
}


BOOL GetDevHwId(int argc, PZPWSTR argv, TCHAR * DeviceHWID)
{
	if (argc >=4)
		_stprintf_s(DeviceHWID, MAX_PATH, (TCHAR *)argv[3]);
	else
		_stprintf_s(DeviceHWID, MAX_PATH, HWID_TMPLT,VENDOR_N_ID, PRODUCT_N_ID, VERSION_N);

	return TRUE;
}

#pragma warning( push )
#pragma warning( disable : 4996 )
void PrintHeader(FILE * dst)
{
	#define PRODUCT_PROFESSIONAL 0x00000030
	typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
	time_t long_time;
	//char timebuf[26], tmpbuf[128];
	//errno_t err;
	struct tm newtime;
	OSVERSIONINFOEX osvi;
	LPOSVERSIONINFOEX lposvi;
	SYSTEM_INFO sysinfo;
	DWORD ProdType;
	PGPI pGPI;
	char timebuf[128];



	_ftprintf(dst, "+++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ \n");

	//// Time of log file ////////////////////////////////////////////
	// Get time as 64-bit integer.
	time( &long_time ); 
	// Convert to local time.
	//newtime = localtime( &long_time );
	localtime_s(&newtime, &long_time);
	// Use strftime to build a customized time string.
	asctime_s(timebuf, &newtime);
	_ftprintf(dst, "+++++++ +++++++ %s", timebuf /*asctime( &newtime )*/ );

	//// System information /////////////////////////////////////////
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	lposvi = &osvi;
#pragma warning(suppress: 28159)
	GetVersionEx((LPOSVERSIONINFO)lposvi);
	_ftprintf(dst, "+++++++ +++++++ OS: %d.%d %s ", osvi.dwMajorVersion,   osvi.dwMinorVersion, osvi.szCSDVersion );

	GetNativeSystemInfo(&sysinfo);
	if (sysinfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
		_ftprintf(dst, " (x64)" );
	else if (sysinfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
		_ftprintf(dst, " (x86)" );
	else
	_ftprintf(dst, " (ARCH?)" );

	// XP
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
	{
		_ftprintf(dst, TEXT("[ Windows XP "));
		if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
			_ftprintf(dst,  TEXT( "Home Edition ]" ));
		else _ftprintf(dst,  TEXT( "Professional ]" ));
	};

	// Vista/W7
	if ( osvi.dwMajorVersion == 6 )
	{
		HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32.dll"));
		if (!hKernel32)
			return;

		LPVOID pProc = GetProcAddress(hKernel32, "GetProductInfo");
		pGPI = (PGPI)pProc;
		pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &ProdType);

        switch( ProdType )
         {
            case PRODUCT_ULTIMATE:
               _ftprintf(dst, TEXT(" [Ultimate Edition]" ));
               break;
            case PRODUCT_PROFESSIONAL:
               _ftprintf(dst, TEXT(" [Professional]" ));
               break;
            case PRODUCT_HOME_PREMIUM:
               _ftprintf(dst, TEXT(" [Home Premium Edition]" ));
               break;
            case PRODUCT_HOME_BASIC:
               _ftprintf(dst, TEXT(" [Home Basic Edition]" ));
               break;
            case PRODUCT_ENTERPRISE:
               _ftprintf(dst, TEXT(" [Enterprise Edition]" ));
               break;
            case PRODUCT_BUSINESS:
               _ftprintf(dst, TEXT(" [Business Edition]" ));
               break;
            case PRODUCT_STARTER:
               _ftprintf(dst, TEXT(" [Starter Edition]" ));
               break;
            case PRODUCT_CLUSTER_SERVER:
               _ftprintf(dst, TEXT(" [Cluster Server Edition]" ));
               break;
            case PRODUCT_DATACENTER_SERVER:
               _ftprintf(dst, TEXT(" [Datacenter Edition]" ));
               break;
            case PRODUCT_DATACENTER_SERVER_CORE:
               _ftprintf(dst, TEXT(" [Datacenter Edition (core installation)]" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER:
               _ftprintf(dst, TEXT(" [Enterprise Edition]" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
               _ftprintf(dst, TEXT(" [Enterprise Edition (core installation)]" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
               _ftprintf(dst, TEXT(" [Enterprise Edition for Itanium-based Systems]" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER:
               _ftprintf(dst, TEXT(" [Small Business Server]" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
               _ftprintf(dst, TEXT(" [Small Business Server Premium Edition]" ));
               break;
            case PRODUCT_STANDARD_SERVER:
               _ftprintf(dst, TEXT(" [Standard Edition]" ));
               break;
            case PRODUCT_STANDARD_SERVER_CORE:
               _ftprintf(dst, TEXT(" [Standard Edition (core installation)]" ));
               break;
            case PRODUCT_WEB_SERVER:
               _ftprintf(dst, TEXT(" [Web Server Edition]" ));
               break;
         }
	};

	 _ftprintf(dst,"\n");
}
#pragma warning( pop )

BOOL WINAPI StatusMessageToStream(void * reserved, TCHAR * buffer,  ERRLEVEL level)
{
	TCHAR *Prefix;
	UNREFERENCED_PARAMETER(reserved);

	switch (level)
	{
	case INFO:
		Prefix = "[I] ";
		break;
	case WARN:
		Prefix = "[W] ";
		break;
	case ERR:
		Prefix = "[E] ";
		break;
	case FATAL:
		Prefix = "[F] ";
		break;
	default:
		return FALSE;
	}
	 
	_ftprintf(stream,"%s %s\n", Prefix, buffer);
	return TRUE;
}

BOOL GetOEMInfFileName( HDEVINFO DeviceInfoSet, SP_DEVINFO_DATA DeviceInfoData, TCHAR * OEMInfFileName)
/*
	Given DeviceInfoSet and DeviceInfoData
	This function updates string OEMInfFileName with the OEMxxx.INF file name (No path) and returns TRUE
	Otherwise returns FALSE
	*/
{
	TCHAR ErrMsg[1000];
	BOOL bRes=TRUE;

	StatusMessage( NULL, "GetOEMInfFileName: Starting",  INFO);

	//// Preparations
	SP_DEVINSTALL_PARAMS deviceInstallParams;
	ZeroMemory(&deviceInstallParams, sizeof(deviceInstallParams));
	deviceInstallParams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
	bRes = SetupDiGetDeviceInstallParams(DeviceInfoSet, &DeviceInfoData, &deviceInstallParams);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiGetDeviceInstallParams failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
		StatusMessage( NULL, "GetOEMInfFileName: Function SetupDiGetDeviceInstallParams OK",  INFO);

	deviceInstallParams.FlagsEx |= (DI_FLAGSEX_INSTALLEDDRIVER | DI_FLAGSEX_ALLOWEXCLUDEDDRVS);
	bRes = SetupDiSetDeviceInstallParams(DeviceInfoSet, &DeviceInfoData, &deviceInstallParams);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiSetDeviceInstallParams failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
		StatusMessage( NULL, "GetOEMInfFileName: Function SetupDiSetDeviceInstallParams OK",  INFO);

	bRes = 	SetupDiBuildDriverInfoList(DeviceInfoSet, &DeviceInfoData, SPDIT_CLASSDRIVER);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiBuildDriverInfoList failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
		StatusMessage( NULL, "GetOEMInfFileName: Function SetupDiBuildDriverInfoList OK",  INFO);

	// Get some details about the driver:
	SP_DRVINFO_DATA driverInfoData;
	ZeroMemory(&driverInfoData,sizeof(driverInfoData));
    driverInfoData.cbSize = sizeof(driverInfoData);
	bRes = 	SetupDiEnumDriverInfo(DeviceInfoSet, &DeviceInfoData, SPDIT_CLASSDRIVER,0, &driverInfoData);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiEnumDriverInfo failed with error: %s", ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
	{
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiEnumDriverInfo for \"%s\" OK", driverInfoData.Description);
		StatusMessage( NULL, prt,  INFO);
	};

	// Retrieve driver information detail for  a particular device information element in the device information set.
	SP_DRVINFO_DETAIL_DATA driverInfoDetail;
	driverInfoDetail.cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);
	bRes = 	SetupDiGetDriverInfoDetail(DeviceInfoSet, &DeviceInfoData,&driverInfoData,&driverInfoDetail,sizeof(SP_DRVINFO_DETAIL_DATA),NULL);
	if (!bRes)
	{
		DWORD errorcode = GetLastError();
		if (errorcode == ERROR_INSUFFICIENT_BUFFER)
		{
			_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiGetDriverInfoDetail OK. INF file is %s", driverInfoDetail.InfFileName);
			StatusMessage( NULL, prt,  INFO);
		}
		else
		{
			GetErrorString(ErrMsg,1000);
			_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiGetDriverInfoDetail failed with error: %s", ErrMsg);
			StatusMessage( NULL, prt,  ERR);
			return FALSE;
		};
	}
	else
	{
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function SetupDiGetDriverInfoDetail OK. INF file is %s", driverInfoDetail.InfFileName);
		StatusMessage( NULL, prt,  INFO);
	};

	// Extract file name from path
	TCHAR Buffer[MAX_PATH], *p;
	p = NULL;
	DWORD size = GetFullPathName(driverInfoDetail.InfFileName, MAX_PATH, Buffer, &p);
	if (size > 0 && size < MAX_PATH && (p != NULL))
	{
		_tcsncpy_s(OEMInfFileName, size, p, size);
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function GetFullPathName OK. INF file is %s", p);
		StatusMessage(NULL, prt, INFO);
	}
	else
	{
		GetErrorString(ErrMsg, 1000);
		_stprintf_s(prt, MAX_PATH, "GetOEMInfFileName: Function GetFullPathName failed with error: %s", ErrMsg);
		StatusMessage(NULL, prt, ERR);
		return FALSE;
	};

	return TRUE;
}
BOOL RemoveRevStr(LPCTSTR DeviceHWID, LPTSTR DeviceHWID_NoRev)
/*
	This function returns a device HW id that does not include revision string
	Given a valid device HW id.

	Example:
	[IN]	DeviceHWID:			root\VID_1234&PID_BEAD&REV_0203
	[OUT]	DeviceHWID_NoRev:	root\VID_1234&PID_BEAD
*/
{
	// Valid input?
	if (!DeviceHWID || !_tcslen(DeviceHWID))
		return FALSE;

	// Get pointer to begining of "&REV" (Upper or lower)
	LPCTSTR revstr;
	revstr = _tcsstr(DeviceHWID, "&R");
	if (!revstr)
	{
		revstr = _tcsstr(DeviceHWID, "&r");
		if (!revstr)
			return FALSE;
	};

	// Trim
	size_t lenall = _tcslen(DeviceHWID);
	size_t lenrev = _tcslen(revstr);
	//_tcsncpy(DeviceHWID_NoRev, DeviceHWID, lenall-lenrev);
	_tcsncpy_s(DeviceHWID_NoRev, _tcslen(DeviceHWID), DeviceHWID, lenall - lenrev);
	return TRUE;
}

// Accessing the HW registry key
HDEVINFO hlp_CreateDeviceInfoList(void)
{
	TCHAR ErrMsg[1000];

	HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		GetErrorString(ErrMsg, 1000);
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupDiCreateDeviceInfoList failed with error: %s", ErrMsg);
		StatusMessage(NULL, prt, ERR);
		return INVALID_HANDLE_VALUE;
	}

	_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupDiCreateDeviceInfoList OK");
	StatusMessage(NULL, prt, INFO);
	return DeviceInfoSet;
}

// Obtains device instance handle to the device node (dnDevInst)
// that is associated with a specified device instance identifier (buf)
CONFIGRET hlp_LocateDevNode(PDEVINST pdnDevInst, DEVINSTID pDeviceID)
{
	CONFIGRET rType = CM_Locate_DevNode(pdnDevInst, pDeviceID, 0);
	if (rType != CR_SUCCESS)
	{
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function CM_Locate_DevNode failed with error: %08X", rType);
		StatusMessage(NULL, prt, ERR);
		return rType;
	}

	_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function CM_Locate_DevNode failed with error: %08X", rType);
	StatusMessage(NULL, prt, INFO);
	return rType;
}

// Get the Device Info Data
BOOL hlp_OpenDeviceInfo(HDEVINFO DeviceInfoSet, PCTSTR  DeviceInstanceId, PSP_DEVINFO_DATA DeviceInfoData)
{
	TCHAR ErrMsg[1000];

	DeviceInfoData->cbSize = sizeof(SP_DEVINFO_DATA);
	BOOL rDi = SetupDiOpenDeviceInfo(DeviceInfoSet, DeviceInstanceId, NULL, 0, DeviceInfoData);
	if (!rDi)
	{
		GetErrorString(ErrMsg, 1000);
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupDiOpenDeviceInfo failed with error: %s", ErrMsg);
		StatusMessage(NULL, prt, ERR);
		return FALSE;
	}
 
	_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupDiOpenDeviceInfo OK");
	StatusMessage(NULL, prt, INFO);
	return TRUE;
}

// Remove node
BOOL hlp_RemoveDevice(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData)
{
	TCHAR ErrMsg[1000];

	BOOL rDi = SetupDiRemoveDevice(DeviceInfoSet, DeviceInfoData);
	if (!rDi)
	{
		GetErrorString(ErrMsg, 1000);
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupDiRemoveDevice failed with error: %s", ErrMsg);
		StatusMessage(NULL, prt, ERR);
		return FALSE;
	}
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupDiRemoveDevice OK");
		StatusMessage(NULL, prt, INFO);
		return TRUE;
}

// Delete INF file from inf directoy
BOOL hlp_UninstallOEMInf(HDEVINFO DeviceInfoSet, SP_DEVINFO_DATA DeviceInfoData)
{
	TCHAR ErrMsg[1000];
	TCHAR OEMInfFileName[MAX_PATH];

	BOOL FileNameRes = GetOEMInfFileName(DeviceInfoSet, DeviceInfoData, OEMInfFileName);
	if (!FileNameRes)
	{
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Failed to obtain OEM INF file name - Perhaps system device");
		StatusMessage(NULL, prt, INFO);
		return TRUE;
	}
	_stprintf_s(prt, MAX_PATH, "RemoveDevice: Going to remove file %s", OEMInfFileName);
	StatusMessage(NULL, prt, INFO);
	BOOL rDi = SetupUninstallOEMInf(OEMInfFileName, SUOI_FORCEDELETE, NULL);
	if (!rDi)
	{
		GetErrorString(ErrMsg, 1000);
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function SetupUninstallOEMInf failed with error: %s", ErrMsg);
		StatusMessage(NULL, prt, ERR);
		return FALSE;
	};
	_stprintf_s(prt, MAX_PATH, "RemoveDevice: File %s removed", OEMInfFileName);
	StatusMessage(NULL, prt, INFO);
	return TRUE;
}


// Get the ID of the device instance
int hlp_DeviceHandle2ID(DEVINST dnDevInst, TCHAR **  DeviceId)
{	
	ULONG  Len;
	
	CONFIGRET rType = CM_Get_Device_ID_Size(&Len, dnDevInst, 0);
	if (rType != CR_SUCCESS)
	{
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function CM_Get_Device_ID_Size failed with error: %08X", rType);
		StatusMessage(NULL, prt, ERR);
		return -103;
	}
	else
	{
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function CM_Get_Device_ID_Size OK");
		StatusMessage(NULL, prt, INFO);
	};


	if (!DeviceId)
		return -1031;

	Len += 2;
	TCHAR * DeviceInstanceId = new TCHAR[Len];
 	if (!DeviceInstanceId)
		return -1032;

	rType = CM_Get_Device_ID(dnDevInst, DeviceInstanceId, Len, 0);
	if (rType != CR_SUCCESS)
	{
		_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function CM_Get_Device_ID failed with error: %08X", rType);
		StatusMessage(NULL, prt, ERR);
		delete[] DeviceInstanceId;
		return -104;
	}
	_stprintf_s(prt, MAX_PATH, "RemoveDevice: Function CM_Get_Device_ID (Device Instance Path = %s) OK", DeviceInstanceId);
	StatusMessage(NULL, prt, INFO);
	*DeviceId = DeviceInstanceId;
	return 0;
}