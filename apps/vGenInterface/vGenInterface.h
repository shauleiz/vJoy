// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VGENINTERFACE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VGENINTERFACE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VGENINTERFACE_EXPORTS
#define VGENINTERFACE_API __declspec(dllexport)
#else
#define VGENINTERFACE_API __declspec(dllimport)
#endif

// This class is exported from the vGenInterface.dll
class VGENINTERFACE_API CvGenInterface {
public:
	CvGenInterface(void);
	// TODO: add your methods here.
};

extern VGENINTERFACE_API int nvGenInterface;

VGENINTERFACE_API int fnvGenInterface(void);
