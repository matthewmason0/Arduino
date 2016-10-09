// MBusProtocol.cpp: implementation of the CMBusProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include "MBusProtocol.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef struct
{	// one entry in the coding table
	tCommand cmd;
	char szMask[32];
	char szComment[32];
} tEntry;

static const tEntry aCodetable[] = 
{
	{ rPing, "18", "Ping" },
	{ cPingOK, "98", "Ping OK" },
	{ cAck, "9F0000f", "Ack/Wait" }, // f0=0|1|6|7|9
	{ cPreparing,  "991ttiimmssff0f", "Preparing" }, //f0=0:normal, f0=4:repeat one, f0=8:repeat all
	{ cStopped,    "992ttiimmssff0f", "Stopped" }, //f1=0:normal, f1=2:mix, f1=8:scan
	{ cPaused,     "993ttiimmssff0f", "Paused" }, // f3=1: play mode, f3=2:paused mode, f3|=8: stopped
	{ cPlaying,    "994ttiimmssff0f", "Playing" },
	{ cSpinup,     "995ttiimmssff0f", "Spinup" },
	{ cForwarding, "996ttiimmssff0f", "FF" },
	{ cReversing,  "997ttiimmssff0f", "FR" },
	{ rPlay,    "11101", "Play" },
	{ rPause,   "11102", "Pause" },
	{ rStop,    "11140", "Stop" },
	{ rScnStop, "11150", "ScanStop" },
	{ rPlayFF,  "11105", "Play FF start" },
	{ rPlayFR,  "11109", "Play FR start" },
	{ rPauseFF, "11106", "Pause FF start" },
	{ rPauseFR, "1110A", "Pause FR start" },
	{ rResume,  "11181", "Play from current pos." },
	{ rResumeP, "11182", "Pause from current pos." },
//	{ rNextMix, "1130A314", "next random" },
//	{ rPrevMix, "1130B314", "previous random" },
	{ rSelect,  "113dttff", "Select" }, // f0=1:playing, f0=2:paused, f1=4:random
	{ rRepeatOff, "11400000", "Repeat Off" },
	{ rRepeatOne, "11440000", "Repeat One" },
	{ rRepeatAll, "11480000", "Repeat All" },
	{ rScan,      "11408000", "Scan" },
	{ rMix,       "11402000", "Mix" },
	{ cPwrUp, "9A0000000000", "some powerup?" },
	{ cLastInfo,  "9B0dttfff0f", "last played" }, // f0=0:done, f0=1:busy, f0=8:eject, //f1=4: repeat1, f1=8:repeat all, f2=2:mix
	{ cChanging4, "9B8d00fff0f", "Changing Phase 4" },
	{ cChanging,  "9B9dttfff0f", "Changing" }, 
	{ cNoMagzn,   "9BAd00f00ff", "No Magazin" },
	{ cChanging2, "9BBd00fff0f", "Changing Phase 2" },
	{ cChanging3, "9BCd00fff0f", "Changing Phase 3" },
	{ cChanging1, "9BDd00fff0f", "Changing Phase 1" },
	{ cStatus, "9Cd01ttmmssf", "Disk Status" },
	{ cStat1, "9D000fffff", "some status?" },
	{ cStat2, "9E0000000", "some more status?" },
	// also seen:
	// 11191
};


CMBusProtocol::CMBusProtocol()
{
}

CMBusProtocol::~CMBusProtocol()
{
}


// use acRaw member to generate the others
HRESULT CMBusProtocol::DecodePacket(tMbusPacket* pPacket, LPCTSTR strSource)
{
	size_t len = strlen(strSource);
	size_t i,j;
	
	// reset all the decoded information
	pPacket->source = eUnknown;
	pPacket->nChksum = -1;
	pPacket->bChkOK = FALSE;
	pPacket->cmd = eInvalid;
	pPacket->szDescription = "";
	pPacket->nFlagDigits = 0;
	pPacket->dwContent = 0;
	pPacket->nDisk = 0;
	pPacket->nTrack = 0;
	pPacket->nIndex = 0;
	pPacket->nMinute = 0;
	pPacket->nSecond = 0;
	pPacket->nFlags = 0;

	if (len < 3)
		return E_FAIL;

	len--;
	pPacket->source = (tSource)Hex2Int(strSource[0]); // determine source from first digit
	pPacket->nChksum = CalcChecksum(strSource, len);
	pPacket->bChkOK = (pPacket->nChksum == Hex2Int(strSource[len])); // verify checksum


	for (i=0; i<sizeof(aCodetable)/sizeof(*aCodetable); i++)
	{	// try all commands
		if (len != strlen(aCodetable[i].szMask))
			continue; // size mismatch

		const char* pszRun = strSource; // source read pointer
		const char* pszCompare = aCodetable[i].szMask; // current compare

		for (j=0; j<len; j++)
		{	// all (upper case) hex digits of the mask must match
			if (*pszCompare >= '0' &&  *pszCompare <= '9' || *pszCompare >= 'A' &&  *pszCompare <= 'F')
			{
				if (*pszCompare != *pszRun)
					break; // exit the char loop
			}
			pszRun++;
			pszCompare++;
		}
		if (j == len)
		{	// a match, now decode parameters if present
			for (j=0; j<len; j++)
			{
				switch (aCodetable[i].szMask[j])
				{
				case 'd': // disk
					pPacket->nDisk = (pPacket->nDisk << 4) | Hex2Int(strSource[j]);
					pPacket->dwContent |= F_DISK;
					break;
				case 't': // track
					pPacket->nTrack = (pPacket->nTrack << 4) | Hex2Int(strSource[j]);
					pPacket->dwContent |= F_TRACK;
					break;
				case 'i': // index
					pPacket->nIndex = (pPacket->nIndex << 4) | Hex2Int(strSource[j]);
					pPacket->dwContent |= F_INDEX;
					break;
				case 'm': // minute
					pPacket->nMinute = (pPacket->nMinute << 4) | Hex2Int(strSource[j]);
					pPacket->dwContent |= F_MINUTE;
					break;
				case 's': // second
					pPacket->nSecond = (pPacket->nSecond << 4) | Hex2Int(strSource[j]);
					pPacket->dwContent |= F_SECOND;
					break;
				case 'f': // flags
					pPacket->nFlags = (pPacket->nFlags << 4) | Hex2Int(strSource[j]);
					pPacket->dwContent |= F_FLAGS;
					pPacket->nFlagDigits++;
					break;
				} // switch
			} // for j

			pPacket->cmd = aCodetable[i].cmd;
			pPacket->szDescription = aCodetable[i].szComment;
			break; // exit the command loop
		}
	}

	return (pPacket->cmd == eInvalid) ? E_FAIL : S_OK;

}


// compose strDest
HRESULT CMBusProtocol::EncodePacket(const tMbusPacket* pPacket, LPTSTR strDest)
{
	HRESULT hr = S_OK;
	int i,j;
	size_t len;
	tMbusPacket Packet = *pPacket; // a copy which I can modify

	// seach the code table entry
	for (i=0; i<sizeof(aCodetable)/sizeof(*aCodetable); i++)
	{	// try all commands
		if (Packet.cmd == aCodetable[i].cmd)
			break;
	}

	if (i == sizeof(aCodetable)/sizeof(*aCodetable))
	{
		strDest = '\0'; // return an empty string
		return E_FAIL; // not found
	}


	const char* pszTemplate = aCodetable[i].szMask;
	char* pszWrite = strDest;
	len = strlen(pszTemplate);


	for (j=len-1; j>=0; j--) // reverse order works better for multi-digit numbers
	{
		if (pszTemplate[j] >= '0' &&  pszTemplate[j] <= '9' || pszTemplate[j] >= 'A' &&  pszTemplate[j] <= 'F')
		{	// copy regular hex digits
			pszWrite[j] = pszTemplate[j];
			continue;
		}

		switch(pszTemplate[j])
		{	// I just assume that any necessary parameter data is present
		case 'd': // disk
			pszWrite[j] = Int2Hex(Packet.nDisk & 0x0F);
			Packet.nDisk >>= 4;
			break;
		case 't': // track
			pszWrite[j] = Int2Hex(Packet.nTrack & 0x0F);
			Packet.nTrack >>= 4;
			break;
		case 'i': // index
			pszWrite[j] = Int2Hex(Packet.nIndex & 0x0F);
			Packet.nIndex >>= 4;
			break;
		case 'm': // minute
			pszWrite[j] = Int2Hex(Packet.nMinute & 0x0F);
			Packet.nMinute >>= 4;
			break;
		case 's': // second
			pszWrite[j] = Int2Hex(Packet.nSecond & 0x0F);
			Packet.nSecond >>= 4;
			break;
		case 'f': // flags
			pszWrite[j] = Int2Hex(Packet.nFlags & 0x0F);
			Packet.nFlags >>= 4;
			break;
		default: // unknow format char
			pszWrite[j] = '?';
			hr = S_FALSE; // not quite OK
		}
	}

	int nChk = CalcChecksum(pszWrite, len);
	pszWrite[len] = Int2Hex(nChk); // add checksum
	pszWrite[len+1] = '\0'; // string termination
	
	return hr;
}


int CMBusProtocol::Hex2Int(char c)
{
	if (c < '0')
		return -1;
	else if (c <= '9')
		return (c - '0');
	else if (c < 'A')
		return -1;
	else if (c <= 'F')
		return (c - 'A' + 10);
	else if (c < 'a')
		return -1;
	else if (c <= 'f')
		return (c - 'a' + 10);

	return -1; // default
}


char CMBusProtocol::Int2Hex(int n)
{
	if (n<10)
		return ('0' + n);
	else if (n<16)
		return ('A' + n - 10);

	return 'X'; // out of range
}

// generate the checksum
int CMBusProtocol::CalcChecksum(const char* szHex, int nLen)
{
	int nChk = 0;
	int i;
	
	for (i=0; i<nLen; i++)
	{
		nChk ^= Hex2Int(szHex[i]);
	}
	nChk = (nChk+1) % 16;
	
	return nChk;
}
