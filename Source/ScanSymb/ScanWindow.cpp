#include "commhdr.h"
#pragma hdrstop

#include "ScanAddIn.h"
#include "AddIn/log.h"
TCHAR			szScannerName[MAX_PATH] = TEXT("SCN1:");	// default scanner name
#include "ScanWindow.h"

ScanWindow *g_wnd = 0;
extern HINSTANCE g_hinst;
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  
  LOG(Log(_T("Message received %d, hWnd = %d, g_wnd = %d"), message, hWnd, g_wnd->m_hwnd));
  switch (message) 
  {
      #if REAL_SCAN()
      case UM_SCAN:
        g_wnd->Scan((LPSCAN_BUFFER)lParam);
        break;
      case UM_STARTSCANNING:
        g_wnd->StartScan();
        break;
      case UM_STOPSCANNING:
        g_wnd->StopScan();
        break;
      case UM_STOPALL:
        g_wnd->StopAll();
        break;
      #endif //REAL_SCAN()
	    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
  WNDCLASS	wc;
  
  wc.style			= CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc		= (WNDPROC) WndProc;
  wc.cbClsExtra		= 0;
  wc.cbWndExtra		= 0;
  wc.hInstance		= hInstance;
  wc.hIcon			= 0;
  wc.hCursor			= 0;
  wc.hbrBackground	= 0;
  wc.lpszMenuName		= 0;
  wc.lpszClassName	= szWindowClass;
  
  LOG(Log(_T("Class registered")));
  return RegisterClass(&wc);
}

//******************************************************************************

ScanWindow::ScanWindow(DScanAddIn *addin)//DApplication &app)
  : m_addin(addin)
#if REAL_SCAN()
  , m_hScanner(0)
  , m_lpScanBuffer(0)
#endif //REAL_SCAN()
  , m_bUseText(TRUE)
  , m_dwScanSize(7095) // default scan buffer size
  , m_dwScanTimeout(0) // default timeout value (0 means no timeout)	
  ,	m_bTriggerFlag(FALSE)
  , m_bRequestPending(FALSE)
  , m_bStopScanning(FALSE)
{
  MyRegisterClass(g_hinst, _T("ScanWindow"));
  
  g_wnd = this;
  m_hwnd = CreateWindow(_T("ScanWindow"), 0, 0,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hinst, NULL);
  LOG(Log(_T("Window created %d"), g_wnd));
}

ScanWindow::~ScanWindow()
{
  SendMessage(m_hwnd, WM_DESTROY, 0, 0);
  LOG(Log(_T("Window destroyed RequestPending = %d, StopScanning = %d"),m_bRequestPending, m_bStopScanning));
}
 
bool ScanWindow::IsEnabled(void)
{
#if REAL_SCAN()
  if (!m_hScanner)
    return false;
  SCAN_STATUS stat;
  DWORD ret = SCAN_GetScanStatus(m_hScanner, &stat);
  ASSERT(ret == E_SCN_SUCCESS);
  LOG(Log(_T("GetScanStatus result = %d"), ret));
  if (ret == E_SCN_SUCCESS) 
    return stat.bEnabled? 1 : 0;
#endif //REAL_SCAN()
  return false;
}

#if REAL_SCAN()
LRESULT ScanWindow::Scan(LPSCAN_BUFFER lpScanBuf)
{
  LOG(Log(_T("Got UM_SCAN")));
  m_bRequestPending = FALSE;

  // Clear the soft trigger
  m_bTriggerFlag = FALSE;
  DWORD ret = SCAN_SetSoftTrigger(m_hScanner,&m_bTriggerFlag);
  LOG(Log(_T("SCAN_SetSoftTrigger result = %d"), ret));
	
  if ( lpScanBuf == NULL ) {
    LOG(Log(_T("UM_SCAN Wrong buffer")));
    return 0;
  }

  switch (SCNBUF_GETSTAT(lpScanBuf))
  { 
	  case E_SCN_DEVICEFAILURE:

      LOG(Log(_T("UM_SCAN Device Failure")));
      m_addin->SendStatusText(_T("Device failure"));
		  break;

	  case E_SCN_READPENDING:

      LOG(Log(_T("UM_SCAN ReadPending")));
      m_addin->SendStatusText(_T("Read pending"));
		  break;

	  case E_SCN_READCANCELLED:

      LOG(Log(_T("UM_SCAN Read Cancelled")));
		  if (m_bStopScanning)
		  {	// complete the second step of UM_STOPSCANNING
			  StopScan();	
        LOG(Log(_T("UM_SCAN Send stop")));
	  	  return 0;
		  }
      m_addin->SendStatusText(_T("Read cancelled"));
		  break;

	  case E_SCN_SUCCESS:
      {
        LOG(Log(_T("UM_SCAN Success %d %s"), SCNBUF_GETLEN(lpScanBuf), Bin2Hex(lpScanBuf, SCNBUF_GETLEN(lpScanBuf))));
        _TCHAR *cp = (LPTSTR)SCNBUF_GETDATA(m_lpScanBuffer);
	      _TCHAR			szLabelType[10];
	      _TCHAR			szLen[MAX_PATH];
		    wsprintf(szLabelType, TEXT("0x%.2X"), SCNBUF_GETLBLTYP(lpScanBuf));
		    wsprintf(szLen, TEXT("%d"), SCNBUF_GETLEN(lpScanBuf));
        //_TCHAR *mess = new _TCHAR[_tcslen(szLabelType) + _tcslen(szLen) + _tcslen(cp) +5];
        //_tcscpy(mess, szLen);
        //_tcscat(mess, _T("|"));
        //_tcscat(mess, szLabelType);
        //_tcscat(mess, _T("|"));
        //_tcscat(mess, cp);
        _TCHAR *mess = new _TCHAR[_tcslen(cp) +5];
        _tcscpy(mess, cp);
        LOG(Log(_T("Scanned text %s"), mess));
         m_addin->SendLabelText(mess);
        delete [] mess;
      }
		  
  }
  m_bRequestPending = false;
  if (m_bStopScanning) {
    LOG(Log(_T("Closing scanner after message")));
    DWORD ret = SCAN_Close(m_hScanner);
    LOG(Log(_T("SCAN_Close result = %d"), ret));
    m_hScanner = 0;
  }
  else
    WaitForNext();

  return 0;
}
#endif //REAL_SCAN()

BOOL ScanWindow::Init()
{
#if REAL_SCAN()
  LOG(Log(_T("Init Scanning ")));
	m_bStopScanning = FALSE;
	DWORD dwResult = SCAN_Open(szScannerName, &m_hScanner);
  LOG(Log(_T("SCAN_Open result = %d"), dwResult));
	if ( dwResult != E_SCN_SUCCESS )
	{	
    LOG(Log(_T("StartScanning Scan_Open error %s"), Bin2Hex(&dwResult, sizeof(dwResult))));
    return FALSE;
	}
  m_bRequestPending = false;
  LOG(Log(_T("Scanner opened %ld"), m_hScanner));
#endif //REAL_SCAN()
  return TRUE;
}

#if REAL_SCAN()
LRESULT ScanWindow::StartScan()
{
  LOG(Log(_T("StartScanning ")));
	// Open scanner, prepare for scanning, 
	// and submit the first read request
  if (!m_hScanner) {
     LOG(Log(_T("Error - not initialized")));
     m_addin->SendStatusText(_T("Not initialized"));
     return 0;
  }
	DWORD dwResult = SCAN_Enable(m_hScanner);
  LOG(Log(_T("SCAN_Enable result = %d"), dwResult));
	if ( dwResult != E_SCN_SUCCESS )
	{
    if (dwResult != 0xa0000011 ) {
      LOG(Log(_T("StartScanning Scan_Enable error1 %s"), Bin2Hex(&dwResult, sizeof(dwResult))));
      return 0;
    }
	}
  LOG(Log(_T("StartScanning Scan Enable success")));
	
	m_lpScanBuffer = SCAN_AllocateBuffer(m_bUseText, m_dwScanSize);
  LOG(Log(_T("SCAN_AllocateBuffer result = %d"), m_lpScanBuffer));
	if (m_lpScanBuffer == NULL)
	{
    LOG(Log(_T("StartScanning Scan Enable error - no buffer")));
    return 0;
	}
  LOG(Log(_T("StartScanning success")));
  WaitForNext();
  return 0;
}

LRESULT ScanWindow::StopScan()
{
  LOG(Log(_T("StopScanning ")));
	// We stop scanning in two steps: first, cancel any pending read 
	// request; second, after there is no more pending request, disable 
	// and close the scanner. We may need to do the second step after 
	// a UM_SCAN message told us that the cancellation was completed.
  if (!m_hScanner) {
     LOG(Log(_T("Error - not initialized")));
     m_addin->SendStatusText(_T("Not initialized"));
     return 0;
  }
  if (!m_bStopScanning && m_bRequestPending) {													
    LOG(Log(_T("Flush")));
		DWORD ret = SCAN_Flush(m_hScanner);
    LOG(Log(_T("SCAN_Flush result = %d"), ret));
  }

	if (!m_bRequestPending)			
	{							 
    LOG(Log(_T("Disable")));
	  DWORD ret =  SCAN_Disable(m_hScanner);
    LOG(Log(_T("SCAN_Disable result = %d"), ret));
	}
	m_bStopScanning = TRUE;
  LOG(Log(_T("StopScanning ended")));
  return 0;
}

LRESULT ScanWindow::StopAll()
{
  LOG(Log(_T("Stop All Scanning ")));
	// We stop scanning in two steps: first, cancel any pending read 
	// request; second, after there is no more pending request, disable 
	// and close the scanner. We may need to do the second step after 
	// a UM_SCAN message told us that the cancellation was completed.
  if (!m_hScanner) {
     LOG(Log(_T("Error - not initialized")));
     m_addin->SendStatusText(_T("Not initialized"));
     return 0;
  }
  if (!m_bStopScanning && m_bRequestPending) {													
    LOG(Log(_T("Flush")));
		DWORD ret = SCAN_Flush(m_hScanner);
    LOG(Log(_T("SCAN_Flush result = %d"), ret));
  }

	if (!m_bRequestPending)			
	{							 
    LOG(Log(_T("Disable")));

		DWORD ret =  SCAN_Disable(m_hScanner);
    LOG(Log(_T("SCAN_Disable result = %d"), ret));

    if (m_lpScanBuffer) {
      LOG(Log(_T("Deallocate")));
			ret = SCAN_DeallocateBuffer(m_lpScanBuffer);
      LOG(Log(_T("SCAN_DeallocateBuffer result = %d"), ret));
    }

    LOG(Log(_T("Close")));
		ret = SCAN_Close(m_hScanner);
    LOG(Log(_T("SCAN_Close result = %d"), ret));
    m_hScanner = 0;
	}
	m_bStopScanning = TRUE;
  LOG(Log(_T("Stop All Scanning ended")));
  return 0;
}
#endif //REAL_SCAN()

void ScanWindow::SetTriggerOn(void)
{
#if REAL_SCAN()
  // Clear the state first before we set it to TRUE
  LOG(Log(_T("Setting Trigger on")));
	m_bTriggerFlag = FALSE;
	DWORD ret = SCAN_SetSoftTrigger(m_hScanner,&m_bTriggerFlag);
  LOG(Log(_T("SCAN_SetSoftTrigger off result = %d"), ret));

	m_bTriggerFlag = TRUE;
	ret = SCAN_SetSoftTrigger(m_hScanner,&m_bTriggerFlag);
  LOG(Log(_T("SCAN_SetSoftTrigger on result = %d"), ret));
  LOG(Log(_T("Setting Trigger on ended")));
#endif //REAL_SCAN()
}

void ScanWindow::WaitForNext(void) 
{
#if REAL_SCAN()
  LOG(Log(_T("WaitForNext")));
	DWORD dwResult = SCAN_ReadLabelMsg(m_hScanner,
								 m_lpScanBuffer,
								 m_hwnd,
								 UM_SCAN,
								 m_dwScanTimeout,
								 NULL);
  LOG(Log(_T("SCAN_ReadLabelMsg result = %d"), dwResult));
  if ( dwResult != E_SCN_SUCCESS ){
    LOG(Log(_T("WaitForNext Scan_ReadLabelMsg error %s"), Bin2Hex(&dwResult, sizeof(dwResult))));
    return;
  }
	else
		m_bRequestPending = TRUE;
  LOG(Log(_T("WaitForNext Scan_ReadLabelMsg success")));
#endif //REAL_SCAN()

}