// ChangerEmu.cpp: implementation of the CChangerEmu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChangerEmu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChangerEmu::CChangerEmu()
{
	memset(&m_Packet, 0, sizeof(m_Packet));

	m_Packet.cmd = cStopped;
	m_Packet.nDisk = 1;
	m_Packet.nFlags = 0x001;

	m_bWaitForEcho = FALSE;
	m_LastCmd = eInvalid;
	m_EchoState = eQuiet;
}

CChangerEmu::~CChangerEmu()
{
}


// returns S_OK if an answer is desired (then no timer should be spawned)
// or S_FALSE if not replying, in which case a timer may be spawned
HRESULT CChangerEmu::ProcessCommand(
	const char* szReceived, // string version of the received packed, for echo test
	const tMbusPacket* pReceived, // received packet in already decoded form
	char* szOutBuf, // destination for the answer packet string
	DWORD* pdwCallAgain) // set to milliseconds units in wished to be called again
{
	HRESULT hr = S_OK; // default is do reply
	tMbusPacket Response;
	memset(&Response, 0, sizeof(Response));

	// check for echo
	if (m_bWaitForEcho && pReceived != NULL)
	{	// I should have received my own packet string back. If not, there might be a collision?
		if (strcmp(m_szLastSent, szReceived) == 0)
		{	// OK, we might send the next if desired
			m_bWaitForEcho = FALSE;

			// evaluate state and send more or maybe spawn timer
			switch (m_EchoState)
			{
			case eGiveState: // this state exists to send the first timestamp right away
				m_EchoState = (m_Packet.cmd == cPlaying) ? ePlaying : eQuiet;
				Response = m_Packet;
				break;

			case ePlaying: // regular timestamps
				*pdwCallAgain = 1000;
				hr= S_FALSE;
				break;

			case eResuming:
				Response.cmd = cStatus;
				Response.nDisk = m_Packet.nDisk;
				Response.nTrack = INT2BCD(99);
				Response.nMinute = INT2BCD(99);
				Response.nSecond = INT2BCD(99);
				Response.nFlags = 0xF;
				m_EchoState = eGiveState;
				break;

			case eChanging1: // debug short cut
				Response.cmd = cChanging;
				Response.nDisk = m_Packet.nDisk;
				Response.nTrack = m_Packet.nTrack;
				Response.nFlags = 0x0001; // done
				m_EchoState = eGiveState;
				break;

			default:
				hr = S_FALSE; // send nothing

			}

			if (hr == S_OK)
			{
				*pdwCallAgain = 0; // discipline myself: no timer if responding
				m_Mbus.EncodePacket(&Response, szOutBuf);
				strcpy(m_szLastSent, szOutBuf); // remember for echo check
				m_bWaitForEcho = TRUE;
			}
			return hr;
		}
		else
		{	// what to do on collision? try sending again
			strcpy(szOutBuf, m_szLastSent); // copy old to output
			return S_OK; // send again
		}

	}
	
	
	if (pReceived == NULL)
	{	// response from timer, not by reception (could be a seperate function in future)
		switch (m_EchoState)
		{
		case ePlaying:
			if (m_Packet.cmd == cPlaying)
			{
				m_Packet.nTrack = INT2BCD(BCD2INT(m_Packet.nTrack) + 1); // count
				if (m_Packet.nTrack > 0x99)
					m_Packet.nTrack = 0;
			}
			else 
				hr = S_FALSE;
			break;

		default:
			hr = S_FALSE; // send nothing

		}

		if (hr == S_OK)
		{
			*pdwCallAgain = 0; // discipline myself: no timer if responding
			m_Mbus.EncodePacket(&m_Packet, szOutBuf);
			strcpy(m_szLastSent, szOutBuf); // remember for echo check
			m_bWaitForEcho = TRUE;
		}
		
		return hr;
	}


	// check if intact and for us
	if (pReceived != NULL && (!pReceived->bChkOK || pReceived->source != eRadio))
	{	// broken packet
		return S_FALSE; // ignore for now
	}
	
	// generate immediate response to radio command, want to see no timer spawn here
	m_LastCmd = pReceived->cmd;
	switch(pReceived->cmd)
	{
	case rPing:
		Response.cmd = cPingOK;
		break;
		
	case rPlay:
		m_Packet.cmd = cPlaying;
		m_Packet.nFlags &= ~0x00B;
		m_Packet.nFlags |=  0x001;
		Response = m_Packet;
		break;

	case rPause:
		m_Packet.cmd = cPaused;
		m_Packet.nFlags &= ~0x00B;
		m_Packet.nFlags |=  0x002;
		Response = m_Packet;
		break;

	case rScnStop:
	case rStop:
		m_Packet.cmd = cStopped;
		m_Packet.nFlags |=  0x008;
		Response = m_Packet;
		break;

	case rPlayFF:
		m_Packet.cmd = cForwarding;
		Response = m_Packet;
		break;

	case rPlayFR:
		m_Packet.cmd = cReversing;
		Response = m_Packet;
		break;

	case rRepeatOff:
		m_Packet.nFlags &= ~0xCA0;
		Response = m_Packet;
		break;

	case rRepeatOne:
		m_Packet.nFlags &= ~0xCA0;
		m_Packet.nFlags |=  0x400;
		Response = m_Packet;
		break;

	case rRepeatAll:
		m_Packet.nFlags &= ~0xCA0;
		m_Packet.nFlags |=  0x800;
		Response = m_Packet;
		break;

	case rScan:
		m_Packet.nFlags &= ~0xCA0;
		m_Packet.nFlags |=  0x080;
		Response = m_Packet;
		break;

	case rMix:
		m_Packet.nFlags &= ~0xCA0;
		m_Packet.nFlags |=  0x020;
		Response = m_Packet;
		break;

	case rSelect:
		Response.cmd = cChanging;
		if (pReceived->nDisk != 0 && pReceived->nDisk != m_Packet.nDisk)
		{
			m_EchoState = eChanging1;
			Response.nDisk = m_Packet.nDisk = pReceived->nDisk;
			//m_Packet.nDisk = 9; // test hack!!
			Response.nTrack = m_Packet.nTrack = 0;
			Response.nFlags = 0x1001; // busy
		}
		else // zero indicates same disk
		{
			m_EchoState = eGiveState;
			m_Packet.nMinute = 0;
			m_Packet.nSecond = 0;
			Response.nDisk = m_Packet.nDisk;
			Response.nTrack = m_Packet.nTrack = pReceived->nTrack;
			Response.nFlags = 0x0001; // done
		}
		break;

	case rResume:
	case rResumeP:
		m_EchoState = eResuming;
		m_Packet.cmd = (pReceived->cmd == rResume) ? cPlaying : cPaused;
		Response.cmd = cChanging;
		Response.nDisk = m_Packet.nDisk;
		Response.nTrack = m_Packet.nTrack;
		Response.nFlags = 0x0001; // done
		break;
	
	default:
		Response.cmd = cAck;

	} // switch(pReceived->cmd)
	
	
	if (hr == S_OK)
	{
		*pdwCallAgain = 0; // discipline myself: no timer if responding
		m_Mbus.EncodePacket(&Response, szOutBuf);
		strcpy(m_szLastSent, szOutBuf); // remember for echo check
		m_bWaitForEcho = TRUE;
	}


	return hr;
}
