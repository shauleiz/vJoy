// vGenInterface.cpp : Defines the exported functions for the DLL application.
//


#include "stdafx.h"
#include "vGenInterface.h"
#include "..\..\inc\public.h"

#define STATIC
#include "..\..\inc\vjoyinterface.h"

#pragma comment(lib, "vJoyInterfaceStat.lib")

void Tester()
{
	SHORT ver = vJoyNS::GetvJoyVersion();
}

// This is an example of an exported variable
VGENINTERFACE_API int nvGenInterface=0;

// This is an example of an exported function.
VGENINTERFACE_API int fnvGenInterface(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see vGenInterface.h for the class definition
CvGenInterface::CvGenInterface()
{
    return;
}
