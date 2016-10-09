// CMbusTestDlg.h : header file
//

#if !defined(AFX_CMBUSTESTDLG_H__CBE0CB44_A6F7_4000_8309_740A8D3245AF__INCLUDED_)
#define AFX_CMBUSTESTDLG_H__CBE0CB44_A6F7_4000_8309_740A8D3245AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tserial_event.h"
#include "MBusProtocol.h"
#include "ChangerEmu.h"

// custom messages
#define MSG_CHAR_ARRIVAL (WM_USER+13) // character received
#define MSG_CONNECTED    (WM_USER+14) // COM connect status

/////////////////////////////////////////////////////////////////////////////
// CMbusTestDlg dialog

class CMbusTestDlg : public CDialog
{
// Construction
public:
	CMbusTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMbusTestDlg)
	enum { IDD = IDD_MBUSTEST_DIALOG };
	CListCtrl	m_LogList;
	CString	m_strPort;
	BOOL	m_bConnected;
	int		m_nAppMode;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMbusTestDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	virtual LRESULT OnCharMessage(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnConnectMessage(WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMbusTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboPort();
	afx_msg void OnRadioMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
	void FormatDumpMsg(tMbusPacket* pPacket, CString* pStr);
	void OnLineReceived(LPCTSTR pszLine); // incoming packet completed

		
	// static notifications
	void static OnSerialEvent(uint32 object, uint32 event); 

// members
	Tserial_event m_serial; // class for serial
	int m_nPort; // COM port
	CString m_strReceived; // incoming packet
	int m_nLines; // # of lines (packets) received
	FILE* m_pLogFile; // debug log to file

	DWORD m_dwLastTimestamp; // last round
	DWORD m_dwTimestamp; // this round
	DWORD m_dwRecallTime; // time after which the emulator likes to get called again

	CImageList m_ImageList; // the log window
	CMBusProtocol m_Mbus; // the class that understands the protocol
	CChangerEmu m_ChangerEmu; // CD changer emulation object
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMBUSTESTDLG_H__CBE0CB44_A6F7_4000_8309_740A8D3245AF__INCLUDED_)
