// vJoyInstall.cpp : Defines the entry point for the console application.
//

#include "..\stdafx.h"
#include "..\vJoyInstall.h"

// Global
/**/extern FILE *stream;


int
__cdecl
_tmain(__in int argc, __in PZPWSTR argv)
{
	//BOOL	rDi;
	//TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	GUID ClassGUID = GUID_NULL;
	//TCHAR InfPath[MAX_PATH];
	TCHAR DeviceHWID[MAX_PATH];
	VERBTYPE verb;
	TCHAR InfFile[MAX_PATH];
	stream = NULL;

	////////////////////////////////////////////////////
	/// Parse Command line
	//
	//  First parameter is the Verb: "Install" (Default)/"Uninstall"/"Repair"
	//	Missing Verb is equivalent to "Install"
	//	Verb is case insensitive. First letter is sufficient
	verb = GetVerb(argc, argv);

	// Second parameter (optional) is inf file.
	// Missing inf file is equivalent to INFFILE
	GetInfFile(argc, argv, InfFile);

	// Third parameter (optional) is Device Hardware ID.
	// Missing data will be replaced by VENDOR_N_ID, PRODUCT_N_ID, VERSION_N
	GetDevHwId(argc, argv, DeviceHWID);

	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// Open file for writing.

	// Open logfile. If unable then steam log to console
	errno_t err;
	err = _tfopen_s(&stream, INSTALL_LOG, "a+");
	if (err)
		return -8;

	//stream = _tfopen( INSTALL_LOG, "a+" );
	if (!stream)
	{
		_ftprintf(stdout, ">> main: Cannot open log file %s \n", INSTALL_LOG);
		stream = stderr;
	}
	else
	{
		_ftprintf(stdout, "Starting.  Log file is %s \n", INSTALL_LOG);
		PrintHeader(stream);
	};
	_ftprintf(stream, ">> main: DeviceHWID --> %s ; InfFile --> %s\n", DeviceHWID, InfFile);

	//
	////////////////////////////////////////////////////

	//// Perform the actual operation //////////////////
	switch (verb)
	{
	case INSTALL:	return Installation(DeviceHWID, InfFile);
	case REMOVE:	return Removal(DeviceHWID, InfFile, FALSE);
	case CLEAN:		return Removal(DeviceHWID, InfFile, TRUE);
	case REPAIR:	return Repair(DeviceHWID, InfFile);
	case INVALID:
	default:
		_ftprintf(stderr,"\
Syntax:	vJoyInstall [I|U|C|R]\n\
	I: Install (default)\n\
	U: Uninstall\n\
	C: Clean (uninstall and delete files from system)\n\
	R: Refresh (Uninstall then Install)");
		return -9;
	};
}
