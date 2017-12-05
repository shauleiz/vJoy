// FreeDev.cpp : Free vXbox & vJoy devices
//
#include "stdafx.h"
#include <Xinput.h>
#include "../../vGenInterface.h"


int main(int argc, const char* argv[]) 
{
	DWORD res;

	if (argc<2 )
	{
		printf("Syntax: [1..16|a]\n");
		return -1;
	}
    
	/////// 'a' - All devices
	// vXbox
	if (argc == 2 && tolower(argv[1][0])=='a' )
	{ 
		printf("Removing All\n");
		for (int i = 1; i < 5; i++)
		{ 
			res = UnPlugForce(i);
			if (res == STATUS_SUCCESS)
				printf("Device %d - Unplugged\n", i);
			else
				printf("Device %d - No change\n", i);
		}
		return 0;
	}

	// Remove one device
	int i = atoi(argv[1]);
	if (i < 1 || i>4)
	{
		printf("Devive range: 1-4\n");
		return -2;
	}

	res = UnPlugForce(i);
	if (res == STATUS_SUCCESS)
		printf("Device %d - Unplugged\n", i);
	else
		printf("Device %d - No change\n", i);



	printf("OK\n");
	return 0;
}

