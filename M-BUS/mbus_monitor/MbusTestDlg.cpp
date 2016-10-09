// MbusTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include <mmsystem.h> // for timer
#include "MbusTest.h"
#include "AboutDlg.h"
#include "MbusTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _BASE CDialog // our base class


/////////////////////////////////////////////////////////////////////////////
// CMbusTestDlg dialog

CMbusTestDlg::CMbusTestDlg(CWnd* pParent /*=NULL*/)
	: _BASE(CMbusTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMbusTestDlg)
	m_strPort = _T("");
	m_bConnected = FALSE;
	m_nAppMode = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwLastTimestamp = 0;
	m_pLogFile = NULL;
}

void CMbusTestDlg::DoDataExchange(CDataExchange* pDX)
{
	_BASE::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMbusTestDlg)
	DDX_Control(pDX, IDC_LIST, m_LogList);
	DDX_CBString(pDX, IDC_COMBO_PORT, m_strPort);
	DDX_Check(pDX, IDC_CHECK_CONNECTED, m_bConnected);
	DDX_Radio(pDX, IDC_RADIO_MONITOR, m_nAppMode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMbusTestDlg, _BASE)
	//{{AFX_MSG_MAP(CMbusTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_PORT, OnSelchangeComboPort)
	ON_BN_CLICKED(IDC_RADIO_CHANGER, OnRadioMode)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_MONITOR, OnRadioMode)
	ON_BN_CLICKED(IDC_RADIO_RADIO, OnRadioMode)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(MSG_CHAR_ARRIVAL, OnCharMessage)
	ON_MESSAGE(MSG_CONNECTED,    OnConnectMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMbusTestDlg message handlers

BOOL CMbusTestDlg::OnInitDialog()
{
	_BASE::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	// read-only trick to checkbox button
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CONNECTED);
	pBtn->SetState(pBtn->IsWindowEnabled());

	// setup the listbox
	m_ImageList.Create(16, 16, TRUE, 4, 4);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_RADIO));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_CD));
	m_LogList.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_LogList.InsertColumn(0, "", LVCFMT_LEFT, 20, 0); // Icon
	m_LogList.InsertColumn(1, "Time diff.", LVCFMT_LEFT, 55, 0);
	m_LogList.InsertColumn(2, "Log", LVCFMT_LEFT, 150, 0);
	m_LogList.InsertColumn(3, "Description", LVCFMT_LEFT, 300, 1);

	m_nAppMode = 0;

	m_nLines = 0;
	m_serial.owner = this;
	m_serial.setManager(OnSerialEvent);

	m_pLogFile = fopen("mbus.log", "wb"); // debug test
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMbusTestDlg::OnClose() 
{
	if (m_pLogFile != NULL)
		fclose(m_pLogFile);

	if (m_bConnected)
		m_serial.disconnect();
	
	_BASE::OnClose();
}


void CMbusTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		_BASE::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMbusTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		_BASE::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMbusTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// hack to keep the checkbox grayed like read only
BOOL CMbusTestDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
		if (pWnd)
		{
			switch (pWnd->GetDlgCtrlID())
			{
			case IDC_CHECK_CONNECTED:
				return TRUE;
			}
		}
	}

	return _BASE::PreTranslateMessage(pMsg);
}

void CMbusTestDlg::OnSelchangeComboPort() 
{
	CDataExchange dx(this, TRUE); // read
	DDX_CBString(&dx, IDC_COMBO_PORT, m_strPort); // do a data exchange for this item by hand
	if (m_bConnected)
		m_serial.disconnect();

	m_serial.connect((char*)(LPCTSTR)m_strPort, 115200, SERIAL_PARITY_NONE, 8, FALSE);
}

void CMbusTestDlg::OnSerialEvent(uint32 object, uint32 event)
{	// note that this function is static!
	char *buffer;
	int   size;
	CMbusTestDlg* pThis;

	pThis = (CMbusTestDlg*)(((Tserial_event*)object)->owner);
	if (pThis != NULL && ::IsWindow(pThis->m_hWnd))
	{
		switch(event)
		{
		case SERIAL_CONNECTED:
			pThis->PostMessage(MSG_CONNECTED, 1); // post this to dialog thread
			break;

		case  SERIAL_DISCONNECTED:
			pThis->PostMessage(MSG_CONNECTED, 0); // post this to dialog thread
			break;
		
		case SERIAL_DATA_ARRIVAL:
			size = pThis->m_serial.getDataInSize();
			buffer = pThis->m_serial.getDataInBuffer();
			while (size--)
				pThis->PostMessage(MSG_CHAR_ARRIVAL, *buffer++); // post this to dialog thread

			pThis->m_serial.dataHasBeenRead();
			break;
        }
    } // if pThis
}


LRESULT CMbusTestDlg::OnCharMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_pLogFile != NULL)
		fwrite(&wParam, 1, 1, m_pLogFile); // debug log
	
	if (wParam == '\r')
	{	// line completed
		OnLineReceived((LPCTSTR)m_strReceived);
		m_strReceived.Empty(); // important: clear for next round
	}
	else if (wParam >= ' ') 
	{	// keep storing non-control characters
		if (m_strReceived.GetLength() == 0) // first char?
			m_dwTimestamp = timeGetTime();

		m_strReceived += (TCHAR)wParam;
	}

	return 0;
}

LRESULT CMbusTestDlg::OnConnectMessage(WPARAM wParam, LPARAM lParam)
{
	m_bConnected = wParam;
	CDataExchange dx(this, FALSE); // write
	DDX_Check(&dx, IDC_CHECK_CONNECTED, m_bConnected); // do a data exchange for this item by hand

	UpdateData(FALSE); // refresh display
	return 0;
}


// incoming packet completed
void CMbusTestDlg::OnLineReceived(LPCTSTR pszLine)
{
	HRESULT hr;
	
	// decode the line
	tMbusPacket Packet;
	m_Mbus.DecodePacket(&Packet, pszLine);

	// display the icon
	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	if (Packet.bChkOK)
		lvi.iImage = (Packet.source == eRadio) ? 1 : ((Packet.source == eCD) ? 2 : 0);
	lvi.iItem = m_nLines;
	lvi.iItem = m_LogList.InsertItem(&lvi); // returns real position
	lvi.mask = LVIF_TEXT; // for next subitems
	
	// display delta time
	CString strTime;
	if (m_dwLastTimestamp != 0)
		strTime.Format("%d", m_dwTimestamp - m_dwLastTimestamp);
	m_dwLastTimestamp = m_dwTimestamp; // remember for next one
	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)((LPCTSTR)strTime);
	m_LogList.SetItem(&lvi);

	// display dump
	lvi.iSubItem = 2;
	lvi.pszText = (LPTSTR)pszLine;
	m_LogList.SetItem(&lvi);

	// compose descriptive string
	CString strDescription;
	FormatDumpMsg(&Packet, &strDescription);
	lvi.iSubItem = 3;
	lvi.pszText = (LPTSTR)((LPCTSTR)strDescription);
	m_LogList.SetItem(&lvi);

	CSize size(0, 1000);
	m_LogList.Scroll(size); // scroll to the end
	m_nLines++;


	// pass the packet to emulation, if applicable
	if(m_nAppMode == 1) // CD emulation
	{
		char szResponseBuf[64] = {'\0'};
		m_dwRecallTime = 0;
		hr = m_ChangerEmu.ProcessCommand(pszLine, &Packet, szResponseBuf, &m_dwRecallTime);
		if (hr == S_OK)
		{	// wants to send a response
			strcat(szResponseBuf, "\r\n");
			m_serial.sendData(szResponseBuf, strlen(szResponseBuf));

		}
		if (m_dwRecallTime)
		{	// wants to be notified again
			SetTimer(IDD, m_dwRecallTime, NULL);
		}
	}

}

void CMbusTestDlg::FormatDumpMsg(tMbusPacket* pPacket, CString* pStr)
{
	CString strAdd;
	*pStr = (pPacket->cmd == eInvalid) ? "???" : pPacket->szDescription;

	if (pPacket->dwContent & F_DISK)
	{
		strAdd.Format(" Disk %X", pPacket->nDisk);
		*pStr += strAdd;
	}

	if (pPacket->dwContent & F_TRACK)
	{
		strAdd.Format(" Track %02X", pPacket->nTrack);
		*pStr += strAdd;
	}

	if (pPacket->dwContent & F_INDEX)
	{
		strAdd.Format(" Index %02X", pPacket->nIndex);
		*pStr += strAdd;
	}

	if (pPacket->dwContent & F_MINUTE)
	{
		strAdd.Format(" Time %02X:", pPacket->nMinute);
		*pStr += strAdd;
	}

	if (pPacket->dwContent & F_SECOND)
	{
		strAdd.Format("%02X", pPacket->nSecond);
		*pStr += strAdd;
	}

	if (pPacket->dwContent & F_FLAGS)
	{
		CString strFormat;
		strFormat.Format("%dX", pPacket->nFlagDigits); // hack to generate a runtime format specification
		strFormat = " Flags %0" + strFormat;
		strAdd.Format(strFormat, pPacket->nFlags);
		*pStr += strAdd;
	}
}

// app mode changed
void CMbusTestDlg::OnRadioMode() 
{
	int nAppMode;
	CDataExchange dx(this, TRUE); // read
	DDX_Radio(&dx, IDC_RADIO_MONITOR, nAppMode);
	if (nAppMode != m_nAppMode)
	{
		m_nAppMode = nAppMode;
	}
}

void CMbusTestDlg::OnTimer(UINT nIDEvent) 
{
	HRESULT hr;
	if (m_strReceived.GetLength() == 0) // not currently receiving?
	{	
		KillTimer(IDD); // I want single shot, so kill on arrival

		char szResponseBuf[64] = {'\0'};
		m_dwRecallTime = 0;
		hr = m_ChangerEmu.ProcessCommand(NULL, NULL, szResponseBuf, &m_dwRecallTime);
		if (hr == S_OK)
		{	// wants to send a response
			strcat(szResponseBuf, "\r\n");
			m_serial.sendData(szResponseBuf, strlen(szResponseBuf));

		}
		if (m_dwRecallTime)
		{	// wants to be notified again
			SetTimer(IDD, m_dwRecallTime, NULL);
		}
	}
	
	_BASE::OnTimer(nIDEvent);
}


