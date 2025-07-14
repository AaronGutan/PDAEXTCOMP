#ifndef _SCANADDIN_H
#define _SCANADDIN_H

#ifndef _ADDIN_ADDIN_H
#include "Addin/Addin.h"
#endif

#ifndef _SCANWINDOW_H
#include "ScanWindow.h"
#endif

struct MessInput
{
  MessInput();
  ~MessInput();
  _TCHAR * m_message;
  _TCHAR * m_data;
};
struct MessOutput
{
  MessOutput();
  ~MessOutput();
  _TCHAR * m_message;
  _TCHAR * m_data;
  void Set(const MessOutput &o);
  void SetMessage(const _TCHAR *message);
  void SetData(const _TCHAR *data);
};

/////////////////////////////////////////////////////////////////////////////
// DScanAddIn

class DScanAddIn : 
    public DAddIn
{

public:
  DScanAddIn(); // { m_dwTitleID = IDS_PROPPAGE_CAPTION; }
  ~DScanAddIn();

    enum 
    {
        propIsScannerEnabled =0,
        propResult,
        propScanTimeout,
        propSendImmediately,
        propRequestPending,
        LastProp      // Always last
    };

    enum 
    {
        methStartScan = 0,
        methStopScan,
        methCleanOutput,
        LastMethod      // Always last
    };

  virtual HRESULT Init(void);
  virtual HRESULT Done(void);
  virtual HRESULT GetInfo(SAFEARRAY **pInfo);
  virtual BOOL FillPropsAndMethods(void);

  virtual HRESULT GetPropVal(long lPropNum,VARIANT *pvarPropVal);
  virtual HRESULT SetPropVal(long lPropNum,VARIANT *pvarPropVal);
  virtual HRESULT IsPropReadable(long lPropNum,BOOL *pboolPropRead);
  virtual HRESULT IsPropWritable(long lPropNum,BOOL *pboolPropWrite);

  virtual HRESULT GetNParams(long lMethodNum,long *plParams);
  virtual HRESULT GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue);
  virtual HRESULT HasRetVal(long lMethodNum,BOOL *pboolRetValue);
  virtual HRESULT CallAsProc(long lMethodNum,SAFEARRAY **paParams);
  virtual HRESULT CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams);

  virtual HRESULT put_Enabled(BOOL boolEnabled);

  void SendStatusText(const _TCHAR *text);
  void SendLabelText(const _TCHAR *text);
  void TransferMessage(const _TCHAR *message, const _TCHAR *data);
  void SendNext();

  MessOutput m_outputqueue[100];
  unsigned int m_nOutputs;
  bool m_bSendImmediately;
  DWORD m_result;
  UINT m_uiTimer;        // Timer reference
  ScanWindow m_wnd;
};

#endif // _LOGADDIN_H
