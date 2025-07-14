#include "commhdr.h"
#pragma hdrstop

#include "LogAddIn.h"
#include "Addin/Log.h"
#include "resource.h"
extern HINSTANCE g_hinst;

BOOL  DLogAddIn::FillPropsAndMethods(void)
{
  m_props[0].m_propIds = IDS_TERM_ENABLED;
  m_props[0].m_propNum = propIsLogEnabled;
  m_props[1].m_propIds = IDS_TERM_FILENAME;
  m_props[1].m_propNum = propFileName;
  m_props[2].m_propIds = IDS_TERM_PRINTTIME;
  m_props[2].m_propNum = propPrintTime;
  m_nProps = LastProp;
  m_methods[0].m_methIds = IDS_TERM_OUTPUT;
  m_methods[0].m_methNum = methOutput;
  m_methods[1].m_methIds = IDS_TERM_DELETE;
  m_methods[1].m_methNum = methDelete;
  m_methods[2].m_methIds = IDS_TERM_SETFILENAME;
  m_methods[2].m_methNum = methSetFileName;
  m_nMethods = LastMethod;
  LOG(Log(_T("Properties filled")));
  return TRUE;
}

HRESULT  DLogAddIn::GetPropVal(long lPropNum,VARIANT *pvarPropVal)
{

    LOG(Log(_T("Getting prop by num %d"), lPropNum));
    ::VariantInit(pvarPropVal);
    switch(lPropNum)
    {
      case propIsLogEnabled:
        {
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_boolEnabled ? 1 : 0;
        }
          break;
      case propFileName:
          V_VT(pvarPropVal) = VT_BSTR;
          V_BSTR(pvarPropVal) = SysAllocString(m_FileName);
          break;
      case propPrintTime:
        {
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_bPrintTime ? 1 : 0;
        }
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DLogAddIn::SetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
    LOG(Log(_T("Setting prop by num %d"), lPropNum));
    switch(lPropNum)
    { 
      case propIsLogEnabled:
          if (V_VT(pvarPropVal) != VT_I4)
              return S_FALSE;
          m_boolEnabled = V_I4(pvarPropVal);
          break;
      case propFileName:
          if (V_VT(pvarPropVal) != VT_BSTR) {
                return S_FALSE;
          }
          _tcscpy(m_FileName,OLE2T(V_BSTR(pvarPropVal)));
          break;
      case propPrintTime:
          if (V_VT(pvarPropVal) != VT_I4)
              return S_FALSE;
          m_bPrintTime = V_I4(pvarPropVal);
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DLogAddIn::IsPropReadable(long lPropNum,BOOL *pboolPropRead)
{

    LOG(Log(_T("Getting prop readability by num %ld"), lPropNum));
    switch(lPropNum)
    { 
      case propIsLogEnabled:
          *pboolPropRead = TRUE;
          break;
      case propFileName:
          *pboolPropRead = TRUE;
          break;
      case propPrintTime:
          *pboolPropRead = TRUE;
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DLogAddIn::IsPropWritable(long lPropNum,BOOL *pboolPropWrite)
{

    LOG(Log(_T("Getting prop writeability by num %d"), lPropNum));
    switch(lPropNum)
    { 
      case propIsLogEnabled:
          *pboolPropWrite = TRUE;
          break;
      case propFileName:
          *pboolPropWrite = TRUE;
          break;
      case propPrintTime:
          *pboolPropWrite = TRUE;
          break;
    default:
        return S_FALSE;
    }

    return S_OK;
}

HRESULT  DLogAddIn::GetNParams(long lMethodNum,long *plParams)
{

    *plParams = 0;
    switch(lMethodNum)
    { 
      case methOutput:
          *plParams = 1;
          break;
      case methDelete:
          *plParams = 0;
          break;
      case methSetFileName:
          *plParams = 2;
          break;
      default:
          return S_FALSE;
    }
    
    return S_OK;
}

HRESULT  DLogAddIn::GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue)
{

    ::VariantInit(pvarParamDefValue);
    switch(lMethodNum)
    { 
      case methOutput:
      case methDelete:
          /* There are no parameter values by default */
        break;
      case methSetFileName:
        if (lParamNum == 0 ){
          pvarParamDefValue->vt = VT_I4;
          pvarParamDefValue->lVal = 1;
        }
        break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DLogAddIn::HasRetVal(long lMethodNum,BOOL *pboolRetValue)
{

    switch(lMethodNum)
    { 
      case methOutput:
      case methDelete:
      case methSetFileName:
          /* All methods are procedures */
          *pboolRetValue = FALSE;
          break;
      default:
          return S_FALSE;
    }

    return S_OK;
}

HRESULT  DLogAddIn::CallAsProc(long lMethodNum, SAFEARRAY **paParams)
{

    USES_CONVERSION;

    switch(lMethodNum)
    { 
      case methOutput:
        {
          HRESULT hr = S_OK;
          VARIANT var = GetNParam(*paParams,0);
          if (V_VT(&var) != VT_BSTR) {
            hr = S_FALSE;
          } else if (m_boolEnabled) {
            OutputString(V_BSTR(&var), m_FileName, m_bPrintTime != 0);
          }
          ::VariantClear(&var);
          }
        break;
      case methDelete:
        DeleteLogFile(m_FileName);
        break;
      case methSetFileName:
        {
          HRESULT hr = S_OK;
          VARIANT var = GetNParam(*paParams,0);
          VARIANT varbool = GetNParam(*paParams,1);
          if (V_VT(&varbool) != VT_I4) {
              hr = S_FALSE;
          } else if (V_VT(&var) != VT_BSTR) {
              hr = S_FALSE;
          } else {
            _tcscpy(m_FileName,GetFullFileName(0, V_BSTR(&var)));
            bool bDelete = V_I4(&varbool) != 0;
            if (bDelete){
              DeleteLogFile(m_FileName);
            }
          }
          ::VariantClear(&var);
          ::VariantClear(&varbool);
          return hr;
        }
        break;
      default:
          return S_FALSE;
    }
    
    return S_OK;
}

HRESULT  DLogAddIn::CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams)
{
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// DLogAddIn


// IPropertyLink

/*HRESULT  DLogAddIn::get_Enabled(BOOL *boolEnabled)
{
    *boolEnabled = m_boolEnabled;
    return S_OK;
}

HRESULT  DLogAddIn::put_Enabled(BOOL boolEnabled)
{
    m_boolEnabled = boolEnabled;
    return S_OK;
}
*/

/////////////////////////////////////////////////////////////////////////////
// DLogAddIn

DLogAddIn::DLogAddIn()
{
  _tcscpy(m_FileName, GetFullFileName(0, _T("Log.log")));
  m_bPrintTime = true;
  m_boolEnabled = true;
}

 DLogAddIn::~DLogAddIn()
 {
 }




