// CompatTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Xinput.h>
//#include "../../../../inc/public.h"
#include "../../vGenInterface.h"

void CM_DisplayStatus();
int CM_AcqDevice();
void CM_DisplayDeviceCtrls(UINT id);
void CM_DisplayAllDeviceCtrls(void);
void CM_Test(UINT id);
void CM_PressButton(UINT id);
void CM_TestPov(UINT id);
void CM_TestAxis(UINT id);

int main()
{
	CM_DisplayStatus();
	getchar();

	// Acquire vJoy Device #3
	AcquireVJD(3);
	CM_DisplayStatus();
	getchar();

	// Relinquish vJoy Device #3
	RelinquishVJD(3);
	CM_DisplayStatus();
	getchar();

	// Acquire vXbox Device #3
	AcquireVJD(1003);
	CM_DisplayStatus();
	getchar();

	// Relinquish vXbox Device #3
	RelinquishVJD(1003);
	CM_DisplayStatus();
	getchar();

	int iDev = CM_AcqDevice();
	CM_DisplayDeviceCtrls((UINT)iDev);
	scanf_s("%*c");

	CM_DisplayAllDeviceCtrls();
	scanf_s("%*c");

	CM_Test(iDev);
	scanf_s("%*c");

    return 0;
}

void CM_DisplayStatus()
{

	// Test if vJoy installed and get its version
	SHORT vJoyVer = GetvJoyVersion();
	if (vJoyVer)
		printf("vJoy Version %04X\n", vJoyVer);
	else
		printf("vJoy not installed\n", vJoyVer);

	// Scan which vJoy devices are installed
	VjdStat DevStat;
	printf("vJoy Devices: [M]issing, [O]wned, [F]ree, [B]usy\n");
	printf(" 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16\n");
	for (UINT i = 1; i <= 16; i++)
	{
		DevStat = GetVJDStatus(i);
		switch (DevStat)
		{
			case VJD_STAT_OWN:
				printf(" O ");
				break;
			case VJD_STAT_FREE:
				printf(" F ");
				break;
			case VJD_STAT_BUSY:
				printf(" B ");
				break;
			case VJD_STAT_MISS:
				printf(" M ");
				break;
			default:
				printf(" ? ");
				break;
		}
	}
	printf("\n\n\n");

	// Scan which vXbox devices are installed
	printf("vXbox Devices: [O]wned, [P]lugged-in, [F]ree\n");
	printf(" 1  2  3  4 \n");
	for (UINT i = 1001; i <= 1004; i++)
	{
		DevStat = GetVJDStatus(i);
		switch (DevStat)
		{
			case VJD_STAT_OWN:
				printf(" O ");
				break;
			case VJD_STAT_FREE:
				printf(" F ");
				break;
			case VJD_STAT_BUSY:
				printf(" P ");
				break;
			case VJD_STAT_MISS:
				printf(" M ");
				break;
			default:
				printf(" ? ");
				break;
		}
	}
	printf("\n");
}

int CM_AcqDevice()
{
	printf("Acquire device: Enter device type [J]/[X] \n");
	CHAR type;
	scanf_s("%c", &type);

	printf("Enter device ID\n");
	UINT DevId;
	scanf_s("%d", &DevId);

	if (tolower(type) == 'x')
		DevId += 1000;


	if (isVJDExists(DevId))
		printf("vJoy device %d already acquired\n", DevId);
	else
		printf("vJoy device %d does not exist\n", DevId);

	AcquireVJD(DevId);

	if (isVJDExists(DevId))
		printf("vJoy device %d is acquired\n", DevId);
	else
		printf("vJoy device %d does not exist\n", DevId);

	return DevId;
}

void CM_DisplayDeviceCtrls(UINT id)
{
	BOOL bRes;
	INT  nBtn, nHat;

	// Axes
	printf(" Axes: ");
	for (size_t i = 0; i < 8; i++)
	{
		bRes = GetVJDAxisExist(id, (UINT)i+ HID_USAGE_X);
			if (bRes)
				printf(" %d ", i+1);
			else
				printf(" - ");
	}

	// Buttons
	nBtn = GetVJDButtonNumber(id);
	printf(" nButtons: %d", nBtn);

	// POV
	nHat = GetVJDDiscPovNumber(id);
	printf(" nHats (Discrete): %d", nHat);

	nHat = GetVJDContPovNumber(id);
	printf(" nHats (Continuous): %d", nHat);

	printf("\n");
}

void CM_DisplayAllDeviceCtrls(void)
{
	// vJoy Devices - Loop on all 16. Print only for owned
	printf("vJoy Devices:\n");
	for (int i = 1; i <= 16; i++)
	{
		printf("Dev %d", i);
		CM_DisplayDeviceCtrls(i);
		printf("\n");
	}

	// vXbox Devices - Loop on all 4. Print only for owned
	printf("\nvXbox Devices:\n");
	for (int i = 1; i <= 4; i++)
	{
		CM_DisplayDeviceCtrls(i+1000);
		printf("\n");
	}

}

void CM_Test(UINT id)
{
	char Test;


	Start:
	printf("Testing [A]xes, [B]uttons or [P]OVs ([Q]uit)\n");

	scanf_s("%c", &Test);

	if (tolower(Test) == 'q')
		return;

	if (tolower(Test) == 'a')
		CM_TestAxis(id);
	else if (tolower(Test) == 'b')
		CM_PressButton(id);
	else if (tolower(Test) == 'p')
		CM_TestPov(id);
	goto Start;
}

void CM_PressButton(UINT id)
{
	printf("Testing Buttons - Press/Release\n");

	while (1)
	{
		printf("Enter button number\n");
		UINT btn;
		scanf_s("%u%*c", &btn);

		printf("[P]ress or [u]n-press (Q to quit / R to reset)?\n");
		CHAR act[5];
		act[0] = getchar();
		act[0] = tolower(act[0]);
		if ((act[0] != 'p') && (act[0] != 'u') && (act[0] != 'r') && (act[0] != 'q'))
			continue;

		BOOL press = FALSE;
		if (act[0] == 'q')
			break;

		if (act[0] == 'r')
		{
			ResetButtons(id);
			continue;
		}


		if (act[0] == 'p')
			press = TRUE;

		SetBtn(press, id, btn);
	}

}

void CM_TestPov(UINT id)
{
	printf("Testing POVs\n");
	int nDisc=0, nCont=0;
	UINT pov;
	char s_val[20], s_pov[20];

	while (1)
	{
		// Test available POVs
		nDisc = GetVJDDiscPovNumber(id);
		if (!nDisc)
			nCont = GetVJDContPovNumber(id);

		// No POVs?
		if (!nDisc && !nCont)
		{
			printf("No POVs for device %d\n", id);
			break;
		}

		// Continuous?
		if (nCont)
		{
			if (nCont == 1)
				pov = 1;
			else
			{ 
				printf("Enter Continuous Pov number in the range 1-%d\n",nCont);
				scanf_s("%u%*c", &pov);
			}
			printf("Value -1, 0-35999 (Q to quit / R to reset)?\n");
		}
		else
		{
			if (nDisc == 1)
				pov = 1;
			else
			{
				printf("Enter Discrete Pov number in the range 1-%d\n", nDisc);
				scanf_s("%u%*c", &pov);
			}
			printf("Value -1, 0-3 (Q to quit)?\n");
		}
		

		scanf_s("%19s", s_val, (unsigned)_countof(s_val));
		if (tolower(s_val[0]) == 'q')
			break;

		if (tolower(s_val[0]) == 'r')
		{
			ResetPovs(id);
			continue;
		}


		INT i_val;
		sscanf_s(s_val, "%d", &i_val, sizeof(INT));

		if (nDisc)
			SetDiscPov(i_val, id, (UCHAR)pov);
		else
			SetContPov(i_val, id, (UCHAR)pov);

	}

}

void CM_TestAxis(UINT id)
{
	printf("Testing Axes\n");

	while (1)
	{
		printf("Enter Axis number\n");
		UINT Axis;
		scanf_s("%d%*c", &Axis);

		printf("Enter Value (Q to quit, R to reset)?\n");
		char s_val[20];
		scanf_s("%19s", s_val, (unsigned)_countof(s_val));
		if (tolower(s_val[0]) == 'q')
			break;

		if (tolower(s_val[0]) == 'r')
		{
			ResetVJD(id);
			continue;
		}

		LONG l_val;
		sscanf_s(s_val, "%d", &l_val);

		SetAxis(l_val, id, Axis+ HID_USAGE_X-1);
	}

}
