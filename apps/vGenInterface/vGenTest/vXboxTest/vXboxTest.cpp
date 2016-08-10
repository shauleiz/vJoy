#define _CRT_SECURE_NO_WARNINGS

#include "../../stdafx.h"
#include <Xinput.h>
#include "../../../../inc/public.h"
#include "../../vGenInterface.h"

void TopLoop(UINT iDev);
void PressButton(UINT iDev);
void PressDpad(UINT iDev);
void SetTrigger(UINT iDev);
void SetAxis(UINT iDev);
void PollVibration(UINT iDev);
void GetVXboxStat(void);

int main()
{

	DWORD dwRes;
	UINT iDefDev;

	// Clean-up vXbox devices
#if 0
	dwRes = UnPlugForce(1);
	dwRes = UnPlugForce(2);
	dwRes = UnPlugForce(3);
	dwRes = UnPlugForce(4);

#endif // 0

	// Get Status on start
	GetVXboxStat();

	// Install device
	dwRes = PlugIn(4);
	dwRes = PlugInNext(&iDefDev);

	// Get Status after plug-in
	GetVXboxStat();

	TopLoop(iDefDev);

	scanf("%*c");
	return 0;

	// Poll Vibration
	PollVibration(iDefDev);

	// Test Dpad
	PressDpad(iDefDev);

	// Test Buttons
	PressButton(iDefDev);


	// Test Axes
	SetAxis(iDefDev);

	// Test Triggers
	SetTrigger(iDefDev);


	scanf("%*c");
	return 0;
}

void TopLoop(UINT iDev)
{
	DWORD res;

	do
	{
		printf("What would you like to do? [Q]uit / [R]eset / [T]est\n");
		CHAR Opt[2];
		scanf("%s", &Opt);
		if (tolower(Opt[0] == 'q'))
			return;
		if (tolower(Opt[0] == 'r'))
			res = ResetController(iDev);

		printf("What would you like to test? [B]uttons / [A]xes / [D]-Pad / [V]ibration\n");
		scanf("%s", &Opt);
		if (tolower(Opt[0] == 'q'))
			return;

		if (tolower(Opt[0] == 'b'))
		{
			PressButton(iDev);
			continue;
		};

		if (tolower(Opt[0] == 'a'))
		{
			SetAxis(iDev);
			continue;
		};

		if (tolower(Opt[0] == 'd'))
		{
			PressDpad(iDev);
			continue;
		};

		if (tolower(Opt[0] == 'v'))
		{
			PollVibration(iDev);
			continue;
		};


	} while (true);

}
void GetVXboxStat(void)
{
	DWORD res;
	const UCHAR nSlots = 4;
	UCHAR nEmptySlots;
	BOOL Exist, Owned;
	BYTE Led;

	// Get Number of slots
	res = GetNumEmptyBusSlots(&nEmptySlots);
	if (res == STATUS_SUCCESS)
		printf("Number of empty vXbox slots: %d\n", nEmptySlots);

	// For every slot - get status
	for (int iSlot = 1; iSlot <= 4; iSlot++)
	{
		printf("vXbox Device %d: ", iSlot);
		// Exists?
		res = isControllerOwned(iSlot, &Exist);
		if (res != STATUS_SUCCESS)
			continue;

		// Free?
		if (!Exist)
		{
			printf("Free\n");
			continue;
		}

		// Owned?
		res = isControllerOwned(iSlot, &Owned);
		if (res != STATUS_SUCCESS)
			continue;

		// Owned or just exists?
		if (Owned)
			printf("Owned");
		else
			printf("Exists");

		res = GetLedNumber(iSlot, &Led);
		if (res != STATUS_SUCCESS)
			continue;

		printf(" - Led:%d\n", Led);
	}
}
void PressButton(UINT iDev)
{
	printf("Testing Buttons - Press/Release\n");

	while (1)
	{
		printf("Enter button number\n");
		WORD btn;
		char cTmp, sBtn[10];
		scanf("%s%c", &sBtn,&cTmp);
		btn = (WORD)atoi(sBtn);

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

		DWORD ok = SetBtn(iDev, btn, press);
	}

}
void SetTrigger(UINT iDev)
{
	printf("Testing Trigger - Value range 0-255\n");
	DWORD ok;

	while (1)
	{
		printf("Enter Trigger [L]eft / [R]ight / [Q]uit\n");
		CHAR Side[2];
		scanf("%s", &Side);
		Side[0] = tolower(Side[0]);
		if ((Side[0] != 'l') && (Side[0] != 'r') && (Side[0] != 'q'))
			continue;

		if (Side[0] == 'q')
			break;

		printf("0-255 (Q to quit)?\n");
		char s_val[20];
		scanf("%s", s_val);
		if (tolower(s_val[0]) == 'q')
			break;

		BYTE b_val;
		sscanf(s_val, "%u", &b_val);

		if (Side[0] == 'r')
			ok = SetTriggerR(iDev, b_val);
		else
			ok = SetTriggerL(iDev, b_val);
	}

}
void SetAxis(UINT iDev)
{
	printf("Testing Axis - Value range -32768 to 32767\n");
	DWORD ok;

	while (1)
	{
		printf("Enter Trigger LX/LY/RX/RY or [Q]uit\n");
		CHAR Side[3];
		scanf("%s", &Side);
		Side[0] = tolower(Side[0]);
		if ((Side[0] != 'l') && (Side[0] != 'r') && (Side[0] != 'q'))
			continue;

		if (Side[0] == 'q')
			break;

		Side[1] = tolower(Side[1]);
		if ((Side[1] != 'x') && (Side[1] != 'y'))
			continue;

		printf("-32768 to 32767 (Q to quit)?\n");
		char s_val[20];
		scanf("%s", s_val);
		if (tolower(s_val[0]) == 'q')
			break;

		SHORT sh_val;
		sscanf(s_val, "%u", &sh_val);

		if ((Side[0] == 'r') && (Side[1] == 'x'))
			ok = SetAxisRx(iDev, sh_val);
		else
		if ((Side[0] == 'r') && (Side[1] == 'y'))
			ok = SetAxisRy(iDev, sh_val);
		else
		if ((Side[0] == 'l') && (Side[1] == 'x'))
			ok = SetAxisLx(iDev, sh_val);
		else
		if ((Side[0] == 'l') && (Side[1] == 'y'))
			ok = SetAxisLy(iDev, sh_val);
	}

}
void PressDpad(UINT iDev)
{
	printf("Testing D-Pad\n");
	UCHAR Flags = 0;
	size_t len = 0;

	while (1)
	{
		printf("Enter Position: [N]uetral, [Q]uite or any combination of [U]p/[D]own/[L]eft/[R]ight\n");
		CHAR Side[5];
		scanf("%s", &Side);
		len = strlen(Side);
		Side[0] = tolower(Side[0]);
		if ((Side[0] != 'n') && (Side[0] != 'q') && (Side[0] != 'u') && (Side[0] != 'd') && (Side[0] != 'l') && (Side[0] != 'r'))
			continue;

		if (Side[0] == 'q')
			break;

		if (Side[0] == 'n')
		{
			DWORD ok = SetDpad(iDev, 0);
			continue;
		}

		for (int i = 0; i < len; i++)
		{
			if (tolower(Side[i]) == 'u')
				Flags |= XINPUT_GAMEPAD_DPAD_UP;
			if (tolower(Side[i]) == 'd')
				Flags |= XINPUT_GAMEPAD_DPAD_DOWN;
			if (tolower(Side[i]) == 'r')
				Flags |= XINPUT_GAMEPAD_DPAD_RIGHT;
			if (tolower(Side[i]) == 'l')
				Flags |= XINPUT_GAMEPAD_DPAD_LEFT;
		}

		DWORD ok = SetDpad(iDev, Flags);
		Side[0] = Side[1] = Side[2] = Side[3] = Side[4] = '\0';
		Flags = 0;
	}

}
void PollVibration(UINT iDev)
{
	XINPUT_VIBRATION Vib;
	DWORD res;
	UINT Loop = 0;
	CHAR Quit[5];
	WORD Left=0, Right=0;

	printf("Set Vibration Values\n");

	do {
		res = GetVibration(iDev, &Vib);
		if (res != STATUS_SUCCESS)
			return;

		if ((Left != Vib.wLeftMotorSpeed) || (Right != Vib.wRightMotorSpeed))
		{
			Left = Vib.wLeftMotorSpeed;
			Right = Vib.wRightMotorSpeed;
			printf("Left: %d - Right: %d\n", Left, Right);
		}
		Sleep(20);
		Loop++;
		if (Loop == 1000)
		{
			printf("Enter [Q] to quite, Hit other key to continue\n");
			scanf("%s", &Quit);
			if (tolower(Quit[0]) == 'q')
				break;
		}

	} while (1);
}