#include <public.h>


//DEFINE_GUID(GUID_DEVINTERFACE_HID, 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00,0x11, 0x11, 0x00, 0x00, 0x30);

#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant); __pragma(warning(default: 4127))


// Prototypes
DWORD GetErrorString(TCHAR * Msg, int Size);
HANDLE OpenJoystickDevice(void);
#ifdef PPJOY_MODE
VOID Convert2CompatibleFormat(PJOYSTICK_STATE pPosCompat, PJOYSTICK_POSITION pPosNative);
#endif
BOOL LoadJoystickPos();
HANDLE OpenJoystickRawDevice(UINT i);
bool isRawDevice(const char *DevicePath, int Index);

