// Test vGenInterface - Unified API for vJoy and vXbox
#include "../stdafx.h"
#include <Xinput.h>
#include "../../../inc/public.h"
#include "../vGenInterface.h"

// C_xxxx Functions: Common API
void C_DisplayStatus();
HDEVICE C_AcqDevice();
void C_PressButton(HDEVICE hDev);
void C_TestPov(HDEVICE hDev);
void C_TestAxis(HDEVICE hDev);
void C_Test(HDEVICE hDev);
void C_DisplayDeviceInfo(HDEVICE hDev);

// B_xxxx Functions: Backward compatibility API
void B_DisplayStatus();


int main()
{
	DWORD dwRes;

#if 1
	// Clean-up vXbox devices
	dwRes = UnPlugForce(1);
	dwRes = UnPlugForce(2);
	dwRes = UnPlugForce(3);
	dwRes = UnPlugForce(4);

#endif // 0

	// Display status and wait
	C_DisplayStatus();
	B_DisplayStatus();
	getchar();

	UCHAR nSlots = 100;
	dwRes = GetNumEmptyBusSlots(&nSlots);

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
	dwRes = PlugIn(1);
	dwRes = isControllerPluggedIn(1, &Plugged);
	dwRes = ResetController(1);

	C_DisplayStatus();
	B_DisplayStatus();
	getchar();

	// Interactive: Ask user which device to acquire
	HDEVICE h1 = C_AcqDevice();
	C_DisplayDeviceInfo(h1);
	getchar();

	C_DisplayStatus();
	B_DisplayStatus();
	getchar();

	// Loop through interactive tests
	C_Test(h1);

	//dwRes = ResetControllerDPad(4);
	getchar();
	return (0);	
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
	printf("vJoy Devices: [M]issing, [O]wned, [F]ree\n");
	printf(" 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16\n");
	for (int i = 1; i <= 16; i++)
	{
		VjdStat stat = GetVJDStatus(i);
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
	printf("vXbox Devices: [O]wned\n");
	printf(" 1  2  3  4 \n");
	for (int i = 1; i <= 4; i++)
	{
		if (isDevOwned(i, vXbox))
			printf(" O ");
		else
			printf(" ? ");
	}
	printf("\n");
}

HDEVICE C_AcqDevice()
{
	printf("Acquire device: Enter device type [J]/[X] \n");
	CHAR type;
	scanf("%c", &type);

	printf("Enter device ID\n");
	UINT DevId;
	scanf("%d", &DevId);

	if (tolower(type) == 'j')
	{ 
		if (ValidDev(GetDevHandle(DevId, vJoy)))
			printf("vJoy device %d already acquired\n", DevId);
		else if (!isVJDExists(DevId))
			printf("vJoy device %d does not exist", DevId);

		return AcquireDev( DevId, vJoy);
	}
	else
	{
		if (ValidDev(GetDevHandle(DevId, vXbox)))
			printf("vXbox device %d already acquired\n", DevId);

		return AcquireDev( DevId, vXbox);
	}
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
		scanf("%u%c", &Axis);

		printf("0-100 (Q to quit)?\n");
		char s_val[20];
		scanf("%s", s_val);
		if (tolower(s_val[0]) == 'q')
			break;

		FLOAT f_val;
		sscanf(s_val, "%f", &f_val);

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
	DevType	type = GetDevType(hDev);
	UINT n = GetDevNumber(hDev);
	UINT id = GetDevId(hDev);

	// Print
	if (type == vJoy)
	{
		printf("Type: vJoy; ID: %u; Device Number: %u\n", id, n);
		return;
	};

	if (type == vXbox)
	{
		printf("Type: vXbox; ID: %u; LED Number: %u\n", id, n);
		return;
	};

	printf("Invalid Device type\n");
	return;
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
