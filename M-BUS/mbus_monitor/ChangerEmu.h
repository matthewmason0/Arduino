// ChangerEmu.h: interface for the CChangerEmu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEREMU_H__E341E7A9_5388_4353_A83C_FFD2BD12D900__INCLUDED_)
#define AFX_CHANGEREMU_H__E341E7A9_5388_4353_A83C_FFD2BD12D900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MbusProtocol.h"

class CChangerEmu  
{
public:
	CChangerEmu();
	virtual ~CChangerEmu();

	HRESULT ProcessCommand(const char* szReceived, const tMbusPacket* pReceived, char* szResponse, DWORD* pdwCallAgain);

protected:
	// prototypes
	HRESULT IncrementTime(tMbusPacket* pReceived, int nSeconds);
	
	// members
	CMBusProtocol m_Mbus; // the class that understands the protocol

	// internal state
	tMbusPacket m_Packet; // maintained packet
	tCommand m_LastCmd; // last command from radio
	BOOL m_bWaitForEcho; // set if a response has been sent
	char m_szLastSent[100]; // our last sent packet, for echo check
	enum
	{
		eQuiet, // do nothing
		eGiveState, // issue play state
		ePlaying, // issue play state in regular intervals
		eResuming, // starting up
		eChanging1,
		eChanging2,
		eChanging3,
		eChanging4,
	} m_EchoState; // what to do after own echo has been  received


};

#endif // !defined(AFX_CHANGEREMU_H__E341E7A9_5388_4353_A83C_FFD2BD12D900__INCLUDED_)
