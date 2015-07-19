// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#undef _UNICODE
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <basetyps.h>
#include <cfgmgr32.h>	// APIs for the Configuration Manager
#include <Setupapi.h>	// Setup and Device Installer services
#include <strsafe.h>	// safer C library string  routine replacements.
#include <Newdev.h>		// Public header file for Windows NT Driver Installation DLL
#include <INITGUID.H>	// Definitions for controlling GUID initialization
#include <dbt.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "User32.lib")


#undef _UNICODE
