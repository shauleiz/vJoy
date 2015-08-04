#define DEV_ID		1

// Prototypes
void  CALLBACK FfbFunction(PVOID data);
void  CALLBACK FfbFunction1(PVOID data,PVOID UserData);

BOOL PacketType2Str(FFBPType Type, LPTSTR Str);
BOOL EffectType2Str(FFBEType Ctrl, LPTSTR Str);
BOOL DevCtrl2Str(FFB_CTRL Type, LPTSTR Str);
BOOL EffectOpStr(FFBOP Op, LPTSTR Str);
int  Polar2Deg(BYTE Polar);
int  Byte2Percent(BYTE InByte);
int TwosCompByte2Int(BYTE in);
int TwosCompWord2Int(WORD in);
