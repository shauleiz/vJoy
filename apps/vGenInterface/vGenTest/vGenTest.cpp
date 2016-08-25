// Test vGenInterface - Unified API for vJoy and vXbox
#include "../stdafx.h"
#include <Xinput.h>
#include "../../../inc/public.h"
#include "../vGenInterface.h"

// General Functions
void DisplayError(DWORD Err);

// C_xxxx Functions: Common API
void C_DisplayStatus();
HDEVICE C_AcqDevice();
void C_PressButton(HDEVICE hDev);
void C_TestPov(HDEVICE hDev);
void C_TestAxis(HDEVICE hDev);
void C_Test(HDEVICE hDev);
void C_DisplayDeviceInfo(HDEVICE hDev);
void C_DisplayDeviceCtrls(HDEVICE hDev);
void C_DisplayAllDeviceCtrls(void);

// B_xxxx Functions: Backward compatibility API
void B_DisplayStatus();


int main()
{
	DWORD dwRes;

#if 0
	// Clean-up vXbox devices
	dwRes = UnPlugForce(1);
	dwRes = UnPlugForce(2);
	dwRes = UnPlugForce(3);
	dwRes = UnPlugForce(4);

#endif // 0

	// Display status and wait
	C_DisplayStatus();
	getchar();

	UCHAR nSlots = 100;
	dwRes = GetNumEmptyBusSlots(&nSlots);
	DisplayError(dwRes);

	SHORT vJoyVer = GetvJoyVersion();
	BOOL exist = (STATUS_SUCCESS == isVBusExists());
	BYTE Led[4];
	XINPUT_VIBRATION Vib;
	LONG MaxX;
	char c[3];
	BOOL Plugged, UnPlugged, Pressed, GetN;
	UINT iSlot;


	// Plug-in vBox device 1, Display status and wait
	dwRes = isControllerPluggedIn(1, &Plugged);
	DisplayError(dwRes);
	dwRes = PlugIn(1);
	DisplayError(dwRes);
	dwRes = isControllerPluggedIn(1, &Plugged);
	DisplayError(dwRes);
	dwRes = ResetController(1);
	DisplayError(dwRes);


	C_DisplayStatus();
	getchar();

	// Interactive: Ask user which device to acquire
	HDEVICE h1 = C_AcqDevice();
	getchar();
	C_DisplayDeviceInfo(h1);
	getchar();
	getchar();
	C_DisplayStatus();
	C_DisplayAllDeviceCtrls();

	// Loop through interactive tests
	C_Test(h1);

	//dwRes = ResetControllerDPad(4);
	getchar();
	return (0);	
}

void DisplayError(DWORD Err)
{
	LPVOID lpMessageBuffer;
	HMODULE Hand = LoadLibrary("NTDLL.DLL");

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_FROM_HMODULE,
		Hand,
		Err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMessageBuffer,
		0,
		NULL);

	// Now display the string.
	printf("Error(0x%X): %s", Err, lpMessageBuffer);

	// Free the buffer allocated by the system.
	LocalFree(lpMessageBuffer);
	FreeLibrary(Hand);
}

void C_DisplayStatus()
{
	// Printout and Wait for any key
	printf("Common API: Print the state of the system - Press any key to continue\n");
	getchar();

	// Test if vXbox bus exists
	if (STATUS_SUCCESS == isVBusExists())
		printf("Virtual Xbox bus exists\n");
	else
		printf("Virtual Xbox bus does NOT exist\n");


	// Test if vJoy installed and get its version
	SHORT vJoyVer = GetvJoyVersion();
	if (vJoyVer)
		printf("vJoy Version %04X\n", vJoyVer);
	else
		printf("vJoy not installed\n", vJoyVer);

	// Scan which vJoy devices are installed
	BOOL Owned, Exist, Free;
	printf("vJoy Devices: [M]issing, [O]wned, [F]ree, [B]usy\n");
	printf(" 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16\n");
	for (int i = 1; i <= 16; i++)
	{
		if (SUCCEEDED(isDevOwned(i, vJoy, &Owned)) && Owned)
			printf(" O ");
		else if (SUCCEEDED(isDevFree(i, vJoy, &Free)) && Free)
			printf(" F ");
		else if (SUCCEEDED(isDevExist(i, vJoy, &Exist)) && Exist)
			printf(" B ");
		else
			printf(" M ");
	}
	printf("\n\n\n");

	// Scan which vXbox devices are installed
	printf("vXbox Devices: [O]wned, [P]lugged-in, [F]ree\n");
	printf(" 1  2  3  4 \n");
	for (int i = 1; i <= 4; i++)
	{
		if (SUCCEEDED(isDevOwned(i, vXbox, &Owned)) && Owned)
			printf(" O ");
		else
			if (SUCCEEDED(isDevExist(i, vXbox, &Exist)) && Exist)
				printf(" P ");
			else
			if (SUCCEEDED(isDevFree(i, vXbox, &Free)) && Free)
				printf(" F ");
			else
				printf(" ? ");
	}
	printf("\n");
}

HDEVICE C_AcqDevice()
{
	DWORD res;
	HDEVICE h;
	printf("Acquire device: Enter device type [J]/[X] \n");
	CHAR type;
	scanf("%c", &type);

	printf("Enter device ID\n");
	UINT DevId;
	scanf("%d", &DevId);

	if (tolower(type) == 'j')
	{ 
		res = GetDevHandle(DevId, vJoy, &h);
		if (SUCCEEDED(res) && ValidDev(h))
			printf("vJoy device %d already acquired\n", DevId);
		else if (!isVJDExists(DevId))
			printf("vJoy device %d does not exist\n", DevId);

		AcquireDev( DevId, vJoy, &h);
	}
	else
	{
		res = GetDevHandle(DevId, vXbox, &h);
		if (SUCCEEDED(res) && ValidDev(h))
			printf("vXbox device %d already acquired\n", DevId);

		AcquireDev( DevId, vXbox, &h);
	}

	return h;
}

void C_PressButton(HDEVICE hDev)
{
	printf("Testing Buttons - Press/Release\n");

	while (1)
	{
		printf("Enter button number\n");
		UINT btn;
		scanf("%u%c", &btn);

		printf("[P]ress or [R]elease (Q to quit)?\n");
		CHAR act[2];
		scanf("%s", &act);
		act[0] = tolower(act[0]);
		if ((act[0] != 'p') && (act[0] != 'r') && (act[0] != 'q'))
			continue;

		BOOL press = FALSE;
		if (act[0] == 'q')
			break;

		if (act[0] == 'p')
			press = TRUE;

		BOOL ok = SetDevButton( hDev, btn,  press);
	}

}

void C_TestPov(HDEVICE hDev)
{
	printf("Testing POVs - using range 0-100\n");

	while (1)
	{
		printf("Enter Pov number\n");
		UINT pov;
		scanf("%u%c", &pov);

		printf("Value -1, 0-100 (Q to quit)?\n");
		char s_val[20];
		scanf("%s", s_val);
		if (tolower(s_val[0]) == 'q')
			break;

		FLOAT f_val;
		sscanf(s_val, "%f", &f_val);


		BOOL ok = SetDevPov(hDev, pov, f_val);
	}

}

void C_TestAxis(HDEVICE hDev)
{
	printf("Testing Axes - using range 0-100\n");

	while (1)
	{
		printf("Enter Axis number\n");
		UINT Axis;
		scanf("%d%*c", &Axis);

		printf("0-100 (Q to quit)?\n");
		char s_val[20];
		scanf("%19s%*c", s_val);
		if (tolower(s_val[0]) == 'q')
			break;

		FLOAT f_val;
		sscanf(s_val, "%f%*c", &f_val);

		BOOL ok = SetDevAxis(hDev, Axis, f_val);
	}

}

void C_Test(HDEVICE hDev)
{
	printf("Testing [A]xes, [B]uttons or [P]OVs\n");

	char Test;
	scanf("%c", &Test);

	if (tolower(Test) == 'a')
		C_TestAxis(hDev);
	else if (tolower(Test) == 'b')
		C_PressButton(hDev);
	else if (tolower(Test) == 'p')
		C_TestPov(hDev);
}

void C_DisplayDeviceInfo(HDEVICE hDev)
{
	// Get device and test validity
	if (!ValidDev(hDev))
	{
		printf("Invalid Device\n");
		return;
	}

	// Valid, get type and Device Number
	DevType	type;
	UINT n, id;
	DWORD  rType = GetDevType(hDev, &type);
	if FAILED(rType)
	{
		printf("GetDevType() - ");
		DisplayError(rType);
		return;
	}

	DWORD nType = GetDevNumber(hDev, &n);
	if FAILED(nType)
	{
		printf("GetDevNumber() - ");
		DisplayError(nType);
		return;
	}

	DWORD iType = GetDevId(hDev, &id);
	if FAILED(iType)
	{
		printf("GetDevId() - ");
		DisplayError(iType);
		return;
	}


	// Print
	if ((type == vJoy) )
	{
		printf("Type: vJoy; ID: %u; Device Number: %u\n", id, n);
		return;
	};

	if ((type == vXbox) )
	{
		printf("Type: vXbox; ID: %u; LED Number: %u\n", id, n);
		return;
	};

	return;
}

void C_DisplayDeviceCtrls(HDEVICE hDev)
{
	DWORD dwRes;
	BOOL  Exist;
	UINT  nBtn, nHat;

	// Axes
	printf(" Axes: ");
	for (size_t i = 1; i <= 8; i++)
	{
		
		dwRes = isAxisExists(hDev, i, &Exist);
		if SUCCEEDED(dwRes)
		{
			if (Exist)
				printf(" %d ", i);
			else
				printf(" - ");
		}
	}

	// Buttons
	dwRes = GetDevButtonN(hDev, &nBtn);
	if SUCCEEDED(dwRes)
		printf(" nButtons: %d", nBtn);

	// POV
	dwRes = GetDevHatN(hDev, &nHat);
	if SUCCEEDED(dwRes)
		printf(" nHats: %d", nHat);
}

void C_DisplayAllDeviceCtrls(void)
{
	HDEVICE hDev;
	DWORD dwRes;
	BYTE Led;

	// vJoy Devices - Loop on all 16. Print only for owned
	printf("vJoy Devices:\n");
	for (int i=1; i<=16; i++)
	{
		dwRes = GetDevHandle(i, vJoy, &hDev);
		if FAILED(dwRes)
			continue;

		printf("Dev %d", i);
		C_DisplayDeviceCtrls(hDev);
		printf("\n");
	}

	// vXbox Devices - Loop on all 4. Print only for owned
	printf("\nvXbox Devices:\n");
	for (int i = 1; i <= 4; i++)
	{
		dwRes = GetDevHandle(i, vXbox, &hDev);
		if FAILED(dwRes)
			continue;

		GetLedNumber(i, &Led);
		printf("Dev %d (LED:%d)", i, Led);
		C_DisplayDeviceCtrls(hDev);
		printf("\n");
	}

}
void B_DisplayStatus()
{
	// Printout and Wait for any key
	printf("Compatible API: Print the state of the system - Press any key to continue\n");
	getchar();

	// Test if vXbox bus exists
	if (STATUS_SUCCESS == isVBusExists())
		printf("Virtual Xbox bus exists\n");
	else
		printf("Virtual Xbox bus does NOT exist\n");


	// Test if vJoy installed and get its version
	SHORT vJoyVer = GetvJoyVersion();
	if (vJoyVer)
		printf("vJoy Version %04X\n", vJoyVer);
	else
		printf("vJoy not installed\n", vJoyVer);

	// Scan which vJoy devices are installed
	VjdStat stat;
	printf("vJoy Devices: [M]issing, [O]wned, [F]ree\n");
	printf(" 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16\n");
	for (int i = 1; i <= 16; i++)
	{
		stat = GetVJDStatus(i);
		if (stat == VJD_STAT_OWN)
			printf(" O ");
		else if (stat == VJD_STAT_FREE)
			printf(" F ");
		else if (stat == VJD_STAT_BUSY)
			printf(" B ");
		else
			printf(" M ");
	}
	printf("\n\n\n");

	// Scan which vXbox devices are installed
	printf("vXbox Devices: [U]nknown, [O]wned\n");
	printf(" 1  2  3  4 \n");
	for (int i = 1001; i <= 1004; i++)
	{
		stat = GetVJDStatus(i);
		if (stat == VJD_STAT_OWN)
			printf(" O ");
		else
			printf(" ? ");
	}
	printf("\n");
}

