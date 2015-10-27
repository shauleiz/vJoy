// vJoyInstall wrapper interface functions

#ifdef __cplusplus
#define EC "C"
#else
#define EC 
#endif


extern EC int WINAPI  Func3(void);
extern EC BOOL WINAPI  is_ppjoy_installed(void);
extern EC BOOL WINAPI  is_vjoy_installed(void);
extern EC BOOL WINAPI  is_vjoy_installed_specific(USHORT Revision);
extern EC BOOL WINAPI  enable(USHORT Revision);
extern EC BOOL WINAPI  disable(USHORT Revision);
extern EC int WINAPI installation(TCHAR * DeviceHWID, TCHAR * InfFile);
extern EC BOOL WINAPI get_inf_file(TCHAR * InfFile);
extern EC BOOL WINAPI get_dev_hwid(TCHAR * DeviceHWID);
extern EC void WINAPI  SetStatusMessageFunc(StatusMessageFunc f);
extern EC BOOL WINAPI  repair(TCHAR * DeviceHWID, TCHAR * InfFile);
extern EC int WINAPI  removal(TCHAR * DeviceHWID, TCHAR * InfFile, BOOL DelInf);
extern EC void WINAPI refresh_vjoy(void);
extern EC void WINAPI refresh_vjoy_specific(USHORT Revision);
