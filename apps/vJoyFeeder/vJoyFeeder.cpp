// vJoyDemo.cpp : main project file.

#include "stdafx.h"
#include "vjoyclient.h"
#include "Form1.h"
#include <vJoyInterface.h>

#pragma comment(lib, "vJoyInterface.lib")

using namespace vJoyDemo;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// Create the main window and run it
	Application::Run(gcnew Form1());
	return 0;
}


