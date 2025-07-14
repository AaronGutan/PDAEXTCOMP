#include "commhdr.h"
#pragma hdrstop

#include "ScanAddIn.h"
#include "resource.h"
#include "addin/log.h"
extern HINSTANCE g_hinst;

MessInput::MessInput()
:  m_message(0)
, m_data(0)
{
}
MessInput::~MessInput()
{
  delete [] m_message;
  delete [] m_data;
}

MessOutput::MessOutput()
:  m_message(0)
, m_data(0)
{
}
MessOutput::~MessOutput()
{
  delete [] m_message;
  delete [] m_data;
}
void MessOutput::SetMessage(const _TCHAR *message)
{
  delete [] m_message;
  if (message) {
    m_message = new _TCHAR[_tcslen(message) + 1];
    _tcscpy(m_message, message);
  }
  else m_message = 0;
}
void MessOutput::SetData(const _TCHAR *data)
{
  delete [] m_data;
  if (data) {
    m_data = new _TCHAR[_tcslen(data) + 1];
    _tcscpy(m_data, data);
  }
  else data = 0;
}
void MessOutput::Set(const MessOutput &o)
{
  SetMessage(o.m_message);
  SetData(o.m_data);
}
/////////////////////////////////////////////////////////////////////////////
// DScanAddIn

DScanAddIn::DScanAddIn()
: m_wnd(this)
{
  m_boolEnabled = true;
  m_result = 0;
  m_bSendImmediately = true;
  m_uiTimer = 0; 
  m_nOutputs = 0;
  LOG(Log(_T("AddIn Created")));
}

 DScanAddIn::~DScanAddIn()
 {
 }

HRESULT DScanAddIn::Init()
{
  m_wnd.Init();
  return DAddIn::Init();
}

HRESULT DScanAddIn::Done()
{
  #if REAL_SCAN()
  m_wnd.StopAll();
  #endif //REAL_SCAN()
  return S_OK;
}

HRESULT  DScanAddIn::GetInfo(SAFEARRAY **pInfo)
{

    // Component should put supported component technology version 
    // in VARIANT at index 0     
    long lInd = 1;
    VARIANT varScanner;
    V_VT(&varScanner) = VT_BOOL;
    // This component supports scanner
    V_BOOL(&varScanner) = 1000;
    SafeArrayPutElement(*pInfo,&lInd,&varScanner);
    
    LOG(Log(_T("GetInfo ended")));
    return S_OK;
}

BOOL  DScanAddIn::FillPropsAndMethods(void)
{
  m_props[0].m_propIds = IDS_TERM_ENABLED;
  m_props[0].m_propNum = propIsScannerEnabled;
  m_props[1].m_propIds = IDS_TERM_RESULT;
  m_props[1].m_propNum = propResult;
  m_props[2].m_propIds = IDS_TERM_SCANTIMEOUT;
  m_props[2].m_propNum = propScanTimeout;
  m_props[3].m_propIds = IDS_TERM_SENDIMMEDIATELY;
  m_props[3].m_propNum = propSendImmediately;
  m_props[4].m_propIds = IDS_TERM_REQUESTPENDING;
  m_props[4].m_propNum = propRequestPending;
  m_nProps = LastProp;
  m_methods[0].m_methIds = IDS_TERM_STARTSCANNER;
  m_methods[0].m_methNum = methStartScan;
  m_methods[1].m_methIds = IDS_TERM_STOPSCANNER;
  m_methods[1].m_methNum = methStopScan;
  m_methods[2].m_methIds = IDS_TERM_CLEANOUTPUT;
  m_methods[2].m_methNum = methCleanOutput;
  m_nMethods = LastMethod;
  LOG(Log(_T("Props and methods filled")));
  return TRUE;
}

HRESULT  DScanAddIn::GetPropVal(long lPropNum,VARIANT *pvarPropVal)
{

    ::VariantInit(pvarPropVal);
    switch(lPropNum)
    {
      case propScanTimeout:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_wnd.m_dwScanTimeout;
          LOG(Log(_T("Got Scan Timeout %ld"), m_wnd.m_dwScanTimeout));
          break;
      case propRequestPending:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_wnd.m_bRequestPending;
          LOG(Log(_T("Got Scan Request Pending %d"), m_wnd.m_bRequestPending));
          break;
      case propSendImmediately:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_bSendImmediately;
          LOG(Log(_T("Got Scan Send Immediately %d"), m_bSendImmediately));
          break;
      case propIsScannerEnabled:
        {
          bool bEnabled = m_wnd.IsEnabled();
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = bEnabled?1:0;
          LOG(Log(_T("Got Scan Enabled %d"),  bEnabled?1:0));
        }
          break;
      case propResult:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_result;
          LOG(Log(_T("Got Result Value %d"),  m_result));
          break;
      default:
          LOG(Log(_T("Asked for get wrong property %ld"),  lPropNum));
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DScanAddIn::SetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
    switch(lPropNum)
    { 
      case propScanTimeout:
          if (V_VT(pvarPropVal) != VT_I4)
              return S_FALSE;
          m_wnd.m_dwScanTimeout = V_I4(pvarPropVal);
          LOG(Log(_T("Set Scan Request Pending %d"), m_wnd.m_bRequestPending));
          break;
      case propSendImmediately:
          if (V_VT(pvarPropVal) != VT_I4) {
             LOG(Log(_T("Got Scan Send Immediately error")));
                return S_FALSE;
          }
          m_bSendImmediately = V_I4(pvarPropVal) == 0? false : true;
          LOG(Log(_T("Got Scan Send Immediately %d"), m_bSendImmediately));
          if (m_bSendImmediately)
            SendNext();
          break;
      default:
          LOG(Log(_T("Asked for set wrong property %ld"),  lPropNum));
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DScanAddIn::IsPropReadable(long lPropNum,BOOL *pboolPropRead)
{

    switch(lPropNum)
    { 
      case propIsScannerEnabled:
          *pboolPropRead = TRUE;
          break;
      case propResult:
          *pboolPropRead = TRUE;
          break;
      case propScanTimeout:
          *pboolPropRead = TRUE;
          break;
      case propSendImmediately:
          *pboolPropRead = TRUE;
          break;
      case propRequestPending:
          *pboolPropRead = TRUE;
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DScanAddIn::IsPropWritable(long lPropNum,BOOL *pboolPropWrite)
{
    switch(lPropNum)
    { 
      case propIsScannerEnabled:
          *pboolPropWrite =FALSE;
          break;
      case propResult:
          *pboolPropWrite = FALSE;
          break;
      case propScanTimeout:
          *pboolPropWrite = TRUE;
          break;
      case propSendImmediately:
          *pboolPropWrite = TRUE;
          break;
      case propRequestPending:
          *pboolPropWrite = FALSE;
          break;
    default:
        return S_FALSE;
    }

    return S_OK;
}

HRESULT  DScanAddIn::GetNParams(long lMethodNum,long *plParams)
{

    *plParams = 0;
    switch(lMethodNum)
    { 
      case methStartScan:
          *plParams = 0;
          break;
      case methStopScan:
          *plParams = 0;
          break;
      case methCleanOutput:
          *plParams = 0;
          break;
      default:
          return S_FALSE;
    }
    
    return S_OK;
}

HRESULT  DScanAddIn::GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue)
{

    ::VariantInit(pvarParamDefValue);
    switch(lMethodNum)
    { 
      case methStartScan:
      case methStopScan:
      case methCleanOutput:
          /* There are no parameter values by default */
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DScanAddIn::HasRetVal(long lMethodNum,BOOL *pboolRetValue)
{

    switch(lMethodNum)
    { 
      case methStartScan:
      case methStopScan:
      case methCleanOutput:
          /* All methods are procedures */
          *pboolRetValue = FALSE;
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DScanAddIn::CallAsProc(long lMethodNum, SAFEARRAY **paParams)
{

    switch(lMethodNum)
    { 
      case methStartScan:
        #if REAL_SCAN()
        LOG(Log(_T("StartScan called")));
        m_wnd.StartScan();
        m_result = 0;
        break;
        #else //REAL_SCAN()
        return ERROR_CALL_NOT_IMPLEMENTED;
        #endif //REAL_SCAN()
      case methStopScan:
        #if REAL_SCAN()
        LOG(Log(_T("StopScan called")));
        m_wnd.StopScan();
        m_result = 0;
        break;
        #else //REAL_SCAN()
        return ERROR_CALL_NOT_IMPLEMENTED;
        #endif //REAL_SCAN()
      case methCleanOutput:
        LOG(Log(_T("CleanOutput called")));
        m_nOutputs = 0;
        m_result = 0;
        break;
      default:
          return S_FALSE;
    }
    
    return S_OK;
}

HRESULT  DScanAddIn::CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams)
{
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IPropertyLink


HRESULT DScanAddIn::put_Enabled(BOOL boolEnabled)
{

  _TCHAR strSource[200];
  ::LoadString(g_hinst, IDS_MESSAGE_SOURCE, strSource, 200);
  _TCHAR strMessage[200];
  ::LoadString(g_hinst, IDS_ENABLED, strMessage, 200);
  _TCHAR strData[200];
  strData[0] = _T('\0');

    m_boolEnabled = boolEnabled;
  if (boolEnabled)
    _tcscpy(strData, _T("1"));
  else 
    _tcscpy(strData, _T("0"));
  m_iAsyncEvent->ExternalEvent(SysAllocString(strSource),
                                SysAllocString(strMessage),
                                SysAllocString(strData));

  return S_OK;
}


void DScanAddIn::SendStatusText(const _TCHAR *text)
{
  _TCHAR strMessage[200];
  _tcscpy(strMessage, _T("Status"));
  TransferMessage(strMessage, text);
}

void DScanAddIn::SendLabelText(const _TCHAR *text)
{
  _TCHAR strMessage[200];
  _tcscpy(strMessage, _T("BarCodeValue"));
  LOG(Log(_T("Sending label %s"), text));
  TransferMessage(strMessage, text);
}

void DScanAddIn::TransferMessage(const _TCHAR *message, const _TCHAR *data)
{

  LOG(Log(_T("Transferring message %d"), (int)m_bSendImmediately));
  if(m_bSendImmediately)
  {
    _TCHAR strSource[200];
    ::LoadString(g_hinst, IDS_MESSAGE_SOURCE, strSource, 200);
    LOG(Log(_T("Sending message %s %s %s"), strSource, message, data));
    m_iAsyncEvent->ExternalEvent(SysAllocString(strSource),
                                 SysAllocString(message),
                                 SysAllocString(data));
    //m_bSendImmediately = false;
  }
  else {
    if (m_nOutputs < 100) {
      LOG(Log(_T("Saving message %s %s"), message, data));
      m_outputqueue[m_nOutputs].SetMessage(message);
      m_outputqueue[m_nOutputs].SetData(data);
      m_nOutputs++;
    }
    else {
      LOG(Log(_T("Saving message overflow %s %s"), message, data));
    }
  }
}

void DScanAddIn::SendNext()
{
  if (m_nOutputs > 0) {
    _TCHAR strSource[200];
    ::LoadString(g_hinst, IDS_MESSAGE_SOURCE, strSource, 200);
    LOG(Log(_T("Sending saved message %s %s %s"), strSource, m_outputqueue[m_nOutputs].m_message, m_outputqueue[m_nOutputs].m_data));

    m_iAsyncEvent->ExternalEvent(SysAllocString(strSource),
      SysAllocString(m_outputqueue[m_nOutputs].m_message),
      SysAllocString(m_outputqueue[m_nOutputs].m_data));
    for (unsigned int i = 0; i < m_nOutputs - 1; i++) {
      m_outputqueue[i].SetMessage(m_outputqueue[i+1].m_message);
      m_outputqueue[i].SetData(m_outputqueue[i+1].m_data);
    }
    m_outputqueue[m_nOutputs - 1].SetMessage(0);
    m_outputqueue[m_nOutputs - 1].SetData(0);
    m_nOutputs--;
    m_result = 0;
    //m_bSendImmediately = false;
  }
  else m_result = 2;
}




