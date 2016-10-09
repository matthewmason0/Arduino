// MBusProtocol.h: interface for the CMBusProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MBUSPROTOCOL_H__A83CE9F8_7D43_4E15_8369_F0E6E59DF7DB__INCLUDED_)
#define AFX_MBUSPROTOCOL_H__A83CE9F8_7D43_4E15_8369_F0E6E59DF7DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// bitflags for content
#define F_DISK   0x00000001
#define F_TRACK  0x00000002
#define F_INDEX  0x00000004
#define F_MINUTE 0x00000008
#define F_SECOND 0x00000010
#define F_FLAGS  0x00000020

typedef enum
{
	eUnknown = 0,
	eRadio = 1,
	eCD = 9,
} tSource;

typedef enum
{
	eInvalid = 0,
	// radio to changer
	rPing,
	rPlay,
	rPause,
	rStop,
	rScnStop,
	rPlayFF,
	rPlayFR,
	rPauseFF,
	rPauseFR,
	rResume,
	rResumeP,
	rNextMix,
	rPrevMix,
	rRepeatOff,
	rRepeatOne,
	rRepeatAll,
	rScan,
	rMix,
	rSelect,
	// changer to radio
	cPingOK,
	cAck,
	cPreparing,
	cStopped,
	cPaused,
	cPlaying,
	cSpinup,
	cForwarding,
	cReversing,
	cPwrUp,
	cLastInfo,
	cChanging4,
	cChanging,
	cNoMagzn,
	cChanging2,
	cChanging3,
	cChanging1,
	cStatus,
	cStat1,
	cStat2,
} tCommand;


typedef struct
{
//	char acRaw[100]; // hex packet including checksum
	tSource source; // origin
	int nChksum; // checksum
	int bChkOK; // checksum OK
	tCommand cmd; // command ID
	const char* szDescription; // decoded desciption
	int nFlagDigits; // how many flag digits
	DWORD dwContent; // bit flags validating the following information items
	int nDisk;   // BCD format
	int nTrack;  // BCD format
	int nIndex;  // BCD format
	int nMinute; // BCD format
	int nSecond; // BCD format
	int nFlags;  // BCD format
} tMbusPacket;

// utility macros
#define BCD2INT(n) (((n)/16) * 10 + ((n) % 16)) // for max. 2 digits
#define INT2BCD(n) (((n)/10) * 16 + ((n) % 10)) // for max. 2 digits
#define INT2HEX(n) (((n) < 10) ? '0' + (n) : 'A' - 10 + (n)) // single digit

class CMBusProtocol  
{
public:
	CMBusProtocol();
	virtual ~CMBusProtocol();

	HRESULT DecodePacket(tMbusPacket* pPacket, LPCTSTR strSource); // decode from a given string
	HRESULT EncodePacket(const tMbusPacket* pPacket, LPTSTR strDest); // compose string, must have enough space
	int CalcChecksum(const char* szHex, int nLen);

protected:

	// utility functions
	int Hex2Int(char c);
	char Int2Hex(int n);

};

#endif // !defined(AFX_MBUSPROTOCOL_H__A83CE9F8_7D43_4E15_8369_F0E6E59DF7DB__INCLUDED_)
