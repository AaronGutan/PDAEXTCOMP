#ifndef _SCANWINDOW_H
#define _SCANWINDOW_H

#ifdef _WIN32_WCE
#ifdef _i386_ // for emulator
#define REAL_SCAN() 0
#else // _i386_
#define REAL_SCAN() 1
#endif // _i386_
#else // _WIN32_WCE
#define REAL_SCAN() 0
#endif // _WIN32_WCE
class ScanSymb;
class DScanAddIn;

#if REAL_SCAN()
#include <ScanCAPI.h>

enum tagUSERMSGS
{
	UM_SCAN	= WM_USER + 0x200,
	UM_STARTSCANNING,
	UM_STOPSCANNING,
	UM_STOPALL
};
#endif //REAL_SCAN()


class ScanWindow //: public CWindowImpl<ScanWindow>
{
//  typedef CWindow Inherited;
public:
  ScanWindow(DScanAddIn *addin);//DApplication &appl);
  ~ScanWindow();
  void SetTriggerOn(void);
  void WaitForNext(void);
  bool IsEnabled(void);
  BOOL m_bUseText;
  DWORD	m_dwScanSize;				
  DWORD	m_dwScanTimeout;
  BOOL	m_bTriggerFlag;
  BOOL	m_bRequestPending;
  BOOL	m_bStopScanning;
  BOOL Init();
  void Destroy();
  #if REAL_SCAN()
  LRESULT StopAll();
  LRESULT StartScan();
  LRESULT StopScan();
  LRESULT Scan(LPSCAN_BUFFER buff);
  #endif //REAL_SCAN()

  HWND m_hwnd;
private:
  #if REAL_SCAN()
  HANDLE m_hScanner;
  LPSCAN_BUFFER	m_lpScanBuffer;
  #endif //REAL_SCAN()
  DScanAddIn *m_addin; 
};


#endif // _SCANWINDOW_H
