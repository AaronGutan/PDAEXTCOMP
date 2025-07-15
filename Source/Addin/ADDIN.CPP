#include "commhdr.h"
#pragma hdrstop

#include "AddIn.h"
#include "log.h"

const IID IID_IInitDone =     {0xAB634001,0xF13D,0x11d0,{0xA4,0x59,0x00,0x40,0x95,0xE1,0xDA,0xEA}};
const IID IID_IPropertyProfile = {0xAB634002,0xF13D,0x11d0,{0xA4,0x59,0x00,0x40,0x95,0xE1,0xDA,0xEA}};
const IID IID_IAsyncEvent = {0xab634004,0xf13d,0x11d0,{0xa4,0x59,0x00,0x40,0x95,0xe1,0xda,0xea}};
const IID IID_ILanguageExtender = {0xAB634003,0xF13D,0x11d0,{0xA4,0x59,0x00,0x40,0x95,0xE1,0xDA,0xEA}};
const IID IID_IStatusLine = {0xab634005,0xf13d,0x11d0,{0xa4,0x59,0x00,0x40,0x95,0xe1,0xda,0xea}};
const IID IID_IPropertyLink = {0x52512A61,0x2A9D,0x11d1,{0xA4,0xD6,0x00,0x40,0x95,0xE1,0xDA,0xEA}};

// This global interface is for timer implementation only
static IAsyncEvent *pAsyncEvent = NULL;
extern HINSTANCE g_hinst;


/////////////////////////////////////////////////////////////////////////////
// DInitDone

DInitDone::DInitDone(DAddIn *parent)
 : m_parent(parent)
{
}

DInitDone::~DInitDone()
{
}
STDMETHODIMP DInitDone::QueryInterface(THIS_ REFIID riid, VOID FAR * FAR * ppvObj)
{
  return m_parent->QueryInterface(riid, ppvObj);
}

ULONG DInitDone::AddRef() 
{
  return m_parent->AddRef();
}
ULONG DInitDone::Release()
{
  return m_parent->Release();
}

 // IInitDone

STDMETHODIMP DInitDone::Init(IDispatch *pConnection)
{
    LOG(Log(_T("Started Init ")));
    m_parent->m_iConnect = pConnection;
    
    m_parent->m_iErrorLog = NULL;
    pConnection->QueryInterface(IID_IErrorLog,(void **)&m_parent->m_iErrorLog);

    m_parent->m_iAsyncEvent = NULL;
    pConnection->QueryInterface(IID_IAsyncEvent,(void **)&m_parent->m_iAsyncEvent);
    
    m_parent->m_iStatusLine = NULL;
    pConnection->QueryInterface(IID_IStatusLine,(void **)&m_parent->m_iStatusLine);
    LOG(Log(_T("Init Continue")));
    return m_parent->Init();
}

STDMETHODIMP DInitDone::Done()
{

    m_parent->SaveProperties();

    if (m_parent->m_iStatusLine) m_parent->m_iStatusLine->Release();
    if (m_parent->m_iProfile) m_parent->m_iProfile->Release();
    if (m_parent->m_iAsyncEvent) m_parent->m_iAsyncEvent->Release();
    if (m_parent->m_iErrorLog) m_parent->m_iErrorLog->Release();
    m_parent->m_iConnect = 0;
    
    return m_parent->Done();
}

STDMETHODIMP DInitDone::GetInfo(SAFEARRAY **pInfo)
{

    LOG(Log(_T("GetInfo started")));
    // Component should put supported component technology version 
    // in VARIANT at index 0     
    long lInd = 0;
    VARIANT varVersion;
    V_VT(&varVersion) = VT_I4;
    // This component supports 1.0 version
    V_I4(&varVersion) = 1000;
    SafeArrayPutElement(*pInfo,&lInd,&varVersion);
    
    LOG(Log(_T("GetInfo continue")));
    return m_parent->GetInfo(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// DLanguageExtender

DLanguageExtender::DLanguageExtender(DAddIn *parent)
 : m_parent(parent)
{
}

DLanguageExtender::~DLanguageExtender()
{
}
STDMETHODIMP DLanguageExtender::QueryInterface(THIS_ REFIID riid, VOID FAR * FAR * ppvObj)
{
  if(riid==IID_ILanguageExtender){
    *ppvObj = (LPVOID)(IInitDone *)this;
    AddRef();
    return S_OK;
  }
  *ppvObj = 0;
  return E_NOINTERFACE;
}
ULONG DLanguageExtender::AddRef() 
{
  return m_parent->AddRef();
}
ULONG DLanguageExtender::Release()
{
  return m_parent->Release();
}

// ILanguageExtender

STDMETHODIMP DLanguageExtender::RegisterExtensionAs(BSTR *bstrExtensionName)
{

  return m_parent->RegisterExtensionAs(bstrExtensionName);
}

STDMETHODIMP DLanguageExtender::GetNProps(long *plProps)
{
    LOG(Log(_T("Entering NProps")));
    return m_parent->GetNProps(plProps);
}

STDMETHODIMP DLanguageExtender::FindProp(BSTR bstrPropName,long *plPropNum)
{
    return m_parent->FindProp(bstrPropName,plPropNum);
}

STDMETHODIMP DLanguageExtender::GetPropName(long lPropNum,long lPropAlias,BSTR *pbstrPropName)
{
    return m_parent->GetPropName(lPropNum, lPropAlias, pbstrPropName);
}

STDMETHODIMP DLanguageExtender::GetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
    return m_parent->GetPropVal(lPropNum, pvarPropVal);
}

STDMETHODIMP DLanguageExtender::SetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
    return m_parent->SetPropVal(lPropNum, pvarPropVal);
}

STDMETHODIMP DLanguageExtender::IsPropReadable(long lPropNum,BOOL *pboolPropRead)
{
    return m_parent->IsPropReadable(lPropNum, pboolPropRead);
}

STDMETHODIMP DLanguageExtender::IsPropWritable(long lPropNum,BOOL *pboolPropWrite)
{
    return m_parent->IsPropWritable(lPropNum, pboolPropWrite);
}

STDMETHODIMP DLanguageExtender::GetNMethods(long *plMethods)
{
    return m_parent->GetNMethods(plMethods);
}

STDMETHODIMP DLanguageExtender::FindMethod(BSTR bstrMethodName, long *plMethodNum)
{
    return m_parent->FindMethod(bstrMethodName, plMethodNum);
}

STDMETHODIMP DLanguageExtender::GetMethodName(long lMethodNum,long lMethodAlias,BSTR *pbstrMethodName)
{
    return m_parent->GetMethodName(lMethodNum, lMethodAlias, pbstrMethodName);
}

STDMETHODIMP DLanguageExtender::GetNParams(long lMethodNum,long *plParams)
{
    return m_parent->GetNParams(lMethodNum, plParams);
}

STDMETHODIMP DLanguageExtender::GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue)
{
    return m_parent->GetParamDefValue(lMethodNum, lParamNum, pvarParamDefValue);
}

STDMETHODIMP DLanguageExtender::HasRetVal(long lMethodNum,BOOL *pboolRetValue)
{
    return m_parent->HasRetVal(lMethodNum, pboolRetValue);
}

STDMETHODIMP DLanguageExtender::CallAsProc(long lMethodNum, SAFEARRAY **paParams)
{
    return m_parent->CallAsProc(lMethodNum, paParams);
}

STDMETHODIMP DLanguageExtender::CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams)
{
    return m_parent->CallAsFunc(lMethodNum, pvarRetValue, paParams);
}

/////////////////////////////////////////////////////////////////////////////
// DPropertyLink

DPropertyLink::DPropertyLink(DAddIn *parent)
 : m_parent(parent)
{
}

DPropertyLink::~DPropertyLink()
{
}
STDMETHODIMP DPropertyLink::QueryInterface(THIS_ REFIID riid, VOID FAR * FAR * ppvObj)
{
  if(riid==IID_IPropertyLink){
    *ppvObj = (LPVOID)(IInitDone *)this;
    AddRef();
    return S_OK;
  }
  *ppvObj = 0;
  return E_NOINTERFACE;
}
ULONG DPropertyLink::AddRef() 
{
  return m_parent->AddRef();
}
ULONG DPropertyLink::Release()
{
  return m_parent->Release();
}

// IPropertyLink

STDMETHODIMP DPropertyLink::get_Enabled(BOOL *boolEnabled)
{
    return m_parent->get_Enabled(boolEnabled);
}

STDMETHODIMP DPropertyLink::put_Enabled(BOOL boolEnabled)
{
    return m_parent->put_Enabled(boolEnabled);
}


/////////////////////////////////////////////////////////////////////////////
// DAddIn

DAddIn::DAddIn()
: m_InitDone(this)
, m_LanguageExtender(this)
, m_PropertyLink(this)
{
  m_iErrorLog = 0;
  m_iProfile = 0;
  m_iStatusLine = 0;
  m_iAsyncEvent = 0;
}

 DAddIn::~DAddIn()
 {
 }

BOOL DAddIn::LoadProperties()
{  

    return TRUE;
}

void DAddIn::SaveProperties()
{
}

STDMETHODIMP DAddIn::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
  if(iid == IID_IUnknown){
    *ppvObj = (LPVOID)this;
    AddRef();
    return S_OK;
  }
  if(iid==IID_IInitDone){
    *ppvObj = (LPVOID)&m_InitDone;
    AddRef();
    return S_OK;
  }
  if(iid==IID_ILanguageExtender){
    *ppvObj = (LPVOID)&m_LanguageExtender;
    AddRef();
    return S_OK;
  }
  if(iid==IID_IPropertyLink){
    *ppvObj = (LPVOID)&m_PropertyLink;
    AddRef();
    return S_OK;
  }
  *ppvObj = 0;
  LOG(Log(_T("Interface not found")));
  return E_NOINTERFACE;
}

STDMETHODIMP_(unsigned long) DAddIn::AddRef()
{
    return ++counter;
}

STDMETHODIMP_(unsigned long) DAddIn::Release()
{
    if (--counter == 0)
    {
        delete this;
        return 0;
    }
    return counter;
}

void DAddIn::TermString(UINT uiResID, long nAlias, _TCHAR *buff, unsigned int lenbuff)
{
  _TCHAR strbuff[200];
  ::LoadString(g_hinst, uiResID, strbuff, 200);

    _TCHAR *cp = _tcschr(strbuff, _T(','));
    if (!cp) {
      if (_tcslen(strbuff) > lenbuff)
        *(strbuff+lenbuff-1) = _T('\0');
      _tcscpy(buff, strbuff);
      return;
    }
    switch(nAlias)
    {
    case 0: // First language
      {
        *cp = _T('\0');
        _tcscpy(buff, strbuff);
        return;
      }
    case 1: // Second language
      {
        if (_tcslen(cp+1) > lenbuff)
          *(cp+1 +lenbuff-1) = _T('\0');
        _tcscpy(buff, cp+1);
        return;
      }
    default:
      {
        *buff = _T('\0');
        return ;
      }
    }
}
VARIANT DAddIn::GetNParam(SAFEARRAY *pArray,long lIndex)
{
    VARIANT vt;
    HRESULT hRes = ::SafeArrayGetElement(pArray,&lIndex,&vt);
    return vt;
}

void DAddIn::PutNParam(SAFEARRAY *pArray,long lIndex,VARIANT vt)
{
    HRESULT hRes = ::SafeArrayPutElement(pArray,&lIndex,&vt);
}


HRESULT DAddIn::Init()
{
  FillPropsAndMethods();
  LOG(Log(_T("Init Ended")));
  return S_OK;
}

HRESULT  DAddIn::Done()
{
  LOG(Log(_T("Done Ended")));
  return S_OK;
}

HRESULT  DAddIn::GetInfo(SAFEARRAY **pInfo)
{
    LOG(Log(_T("GetInfo ended")));
    return S_OK;
}


HRESULT  DAddIn::RegisterExtensionAs(BSTR *bstrExtensionName)
{
    USES_CONVERSION;

    // You may delete next lines and add your own implementation code here

    // Name of extension should be changed avoiding conflicts 
    _TCHAR csExtenderName[300];
    LoadString(g_hinst, 100, csExtenderName, 300);
    //const _TCHAR *cp = csExtenderName;
    *bstrExtensionName = SysAllocString(T2OLE(csExtenderName));
    LOG(Log(_T("Extension registered as %s"), csExtenderName));

    return NULL;
}

HRESULT  DAddIn::GetNProps(long *plProps)
{
  *plProps = m_nProps;
  LOG(Log(_T("Exiting NProps %d"), (int)m_nProps));
  return S_OK;
}

HRESULT  DAddIn::FindProp(BSTR bstrPropName,long *plPropNum)
{
    *plPropNum = -1;
    _TCHAR csPropName[200];
    USES_CONVERSION;
    _tcscpy(csPropName, OLE2T(bstrPropName));
    _TCHAR propname[300];
    for (unsigned int i = 0; i < m_nProps; i++) {
      TermString(m_props[i].m_propIds, 0, propname, 300);
      if (_tcscmp(propname,csPropName) == 0) {
        *plPropNum = m_props[i].m_propNum;
        break;
      }
      TermString(m_props[i].m_propIds, 1, propname, 300);
      if (_tcscmp(propname,csPropName) == 0) {
        *plPropNum = m_props[i].m_propNum;
        break;
      }
    }
    return (*plPropNum == -1) ? S_FALSE : S_OK;
}

HRESULT  DAddIn::GetPropName(long lPropNum,long lPropAlias,BSTR *pbstrPropName)
{

  _TCHAR csPropName[300]; 
  csPropName[0] = _T('0');
  bool bFound = false;
  for (unsigned int i = 0; i < m_nProps; i++) {
    if (lPropNum == m_props[i].m_propNum) {
      TermString(m_props[i].m_propIds,lPropAlias, csPropName, 300);
      bFound = true;
      break;
    }
  }
  if (!bFound)
      return S_FALSE;
  USES_CONVERSION;
  *pbstrPropName = SysAllocString(T2OLE(csPropName));
  return S_OK;
}

HRESULT  DAddIn::GetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
    return S_FALSE;
}

HRESULT  DAddIn::SetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
    return S_FALSE;
}

HRESULT  DAddIn::IsPropReadable(long lPropNum,BOOL *pboolPropRead)
{
    return S_FALSE;
}

HRESULT  DAddIn::IsPropWritable(long lPropNum,BOOL *pboolPropWrite)
{
    return S_FALSE;
}

HRESULT  DAddIn::GetNMethods(long *plMethods)
{
    *plMethods = m_nMethods;
    LOG(Log(_T("Exiting NMethods %d"), m_nMethods));
    return S_OK;
}

HRESULT  DAddIn::FindMethod(BSTR bstrMethodName, long *plMethodNum)
{
    _TCHAR csPropName[300]; 
    csPropName[0] = _T('0');

    _TCHAR propname[300];
    for (unsigned int i = 0; i < m_nMethods; i++) {
      TermString(m_methods[i].m_methIds, 0, propname, 300);
      if (_tcscmp(propname,csPropName) == 0) {
        *plMethodNum = m_methods[i].m_methNum;
        break;
      }
      TermString(m_methods[i].m_methIds, 1, propname, 300);
      if (_tcscmp(propname,csPropName) == 0) {
        *plMethodNum = m_methods[i].m_methNum;
        break;
      }
    }
  	return (*plMethodNum == -1) ? S_FALSE : S_OK;
}

HRESULT  DAddIn::GetMethodName(long lMethodNum,long lMethodAlias,BSTR *pbstrMethodName)
{
  _TCHAR csMethodName[300];
  csMethodName[0]= _T('\0');     
  bool bFound = false;
  for (unsigned int i = 0; i < m_nMethods; i++) {
    if (lMethodNum == m_methods[i].m_methNum) {
      TermString(m_methods[i].m_methIds, lMethodAlias, csMethodName, 300);
      bFound = true;
      break;
    }
  }
  if (!bFound)
      return S_FALSE;
  USES_CONVERSION;
  *pbstrMethodName = SysAllocString(T2OLE(csMethodName));

  return S_OK;
}

HRESULT  DAddIn::GetNParams(long lMethodNum,long *plParams)
{
    return S_FALSE;
}

HRESULT  DAddIn::GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue)
{
    return S_FALSE;
}

HRESULT  DAddIn::HasRetVal(long lMethodNum,BOOL *pboolRetValue)
{
    return S_FALSE;
}

HRESULT  DAddIn::CallAsProc(long lMethodNum, SAFEARRAY **paParams)
{
    return S_FALSE;
}

HRESULT  DAddIn::CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams)
{
    return S_FALSE;
}

// IPropertyLink

HRESULT  DAddIn::get_Enabled(BOOL *boolEnabled)
{
    *boolEnabled = m_boolEnabled;
    return S_OK;
}

HRESULT  DAddIn::put_Enabled(BOOL boolEnabled)
{
    LOG(Log(_T("Set enabled %d"), boolEnabled));
    m_boolEnabled = boolEnabled;
    return S_OK;
}
/*uint DAddIn::GetPropNum(uint propIds)
{
}

uint DAddIn::GetPropIds(uint propNum)
{
}

uint DAddIn::GetMethodNum(uint methodIds)
{
}

uint DAddIn::GetMethodIds(uint methodNum)
{
}
*/
#if 0
static PARAMDATA rgpDataInit[] = {{ OLESTR("Connection"), VT_DISPATCH },
};

static PARAMDATA rgpDataGetInfo[] = {{ OLESTR("Info"), VT_SAFEARRAY },
};

enum IMETH_INITDONE
{
    IMETH_INIT = 0,
    IMETH_DONE = 1,
    IMETH_GETINFO = 2,
};

enum IDMEMBER_INIDDONE
{
    IDMEMBER_INIT = 0,
    IDMEMBER_DONE,
    IDMEMBER_GETINFO
};

static METHODDATA rgmdataDInitDone[] =
{
    { OLESTR("Init"), rgpDataInit, IDMEMBER_INIT, 
      IMETH_INIT,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("Done"), 0, IDMEMBER_DONE, 
      IMETH_DONE,
      CC_CDECL, 0, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetInfo"), rgpDataGetInfo, IDMEMBER_GETINFO, 
      IMETH_GETINFO,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY }
};

INTERFACEDATA idataDInitDone =
{
    rgmdataDInitDone, lengthof(rgmdataDInitDone)
};

static PARAMDATA rgpRegisterAs[] = {{ OLESTR("ExtensionName"), VT_BSTR|VT_BYREF },
};
static PARAMDATA rgpGetNProp[] = {{ OLESTR("Num"), VT_I4|VT_BYREF },
};
static PARAMDATA rgpFindProp[] = {
  { OLESTR("PropName"), VT_BSTR },
  { OLESTR("PropNum"), VT_I4|VT_BYREF },
};
static PARAMDATA rgpGetPropName[] = {
  { OLESTR("PropNum"), VT_I4 },
  { OLESTR("PropAlias"), VT_I4 },
  { OLESTR("PropName"), VT_BSTR|VT_BYREF },
};
static PARAMDATA rgpGetPropVal[] = {
  { OLESTR("PropName"), VT_BSTR },
  { OLESTR("PropValue"), VT_VARIANT|VT_BYREF },
};
static PARAMDATA rgpSetPropVal[] = {
  { OLESTR("PropName"), VT_BSTR },
  { OLESTR("PropValue"), VT_VARIANT|VT_BYREF },
};
static PARAMDATA rgpIsPropReadable[] = {
  { OLESTR("PropNum"), VT_I4 },
  { OLESTR("PropRead"), VT_BOOL|VT_BYREF },
};
static PARAMDATA rgpIsPropWritable[] = {
  { OLESTR("PropNum"), VT_I4 },
  { OLESTR("PropWrite"), VT_BOOL|VT_BYREF },
};
static PARAMDATA rgpGetNMethods[] = {{ OLESTR("Info"), VT_I4|VT_BYREF },
};
static PARAMDATA rgpFindMethod[] = {
  { OLESTR("MethodName"), VT_BSTR },
  { OLESTR("MethodNum"), VT_I4|VT_BYREF },
};
static PARAMDATA rgpGetMethodName[] = {
  { OLESTR("MethodNum"), VT_I4 },
  { OLESTR("MethodAlias"), VT_I4 },
  { OLESTR("MethodName"), VT_BSTR|VT_BYREF },
};
static PARAMDATA rgpGetNParams[] = {
  { OLESTR("MethodNum"), VT_I4 },
  { OLESTR("NParams"), VT_I4|VT_BYREF },
};
static PARAMDATA rgpGetParamDefValue[] = {
  { OLESTR("MethodNum"), VT_I4 },
  { OLESTR("DefValue"), VT_VARIANT|VT_BYREF },
};
static PARAMDATA rgpHasRetVal[] = {
  { OLESTR("MethodNum"), VT_I4 },
  { OLESTR("HasRetValue"), VT_BOOL|VT_BYREF },
};
static PARAMDATA rgpCallAsProc[] = {
  { OLESTR("MethodNum"), VT_I4 },
  { OLESTR("Params"), VT_ARRAY|VT_BYREF },
};
static PARAMDATA rgpCallAsFunc[] = {
  { OLESTR("MethodNum"), VT_I4 },
  { OLESTR("RetValue"), VT_VARIANT|VT_BYREF },
  { OLESTR("Params"), VT_ARRAY|VT_BYREF },
};


enum IMETH_LANGUAGEEXTENDER
{
    IMETH_REGISTERAS = 0,
    IMETH_GETNPROPS,
    IMETH_FINDPROP,
    IMETH_GETPROPNAME,
    IMETH_GETPROPVAL,
    IMETH_SETPROPVAL,
    IMETH_ISPROPREADABLE,
    IMETH_ISPROPWRITABLE,
    IMETH_GETNMETHODS,
    IMETH_FINDMETHOD,
    IMETH_GETMETHODNAME,
    IMETH_GETNPARAMS,
    IMETH_GETPARAMDEFVALUE,
    IMETH_HASRETVAL,
    IMETH_CALLASPROC,
    IMETH_CALLASFUNC
};

enum IDMEMBER_LANGUAGEEXTENDER
{
    IDMEMBER_REGISTERAS = 0,
    IDMEMBER_GETNPROPS,
    IDMEMBER_FINDPROP,
    IDMEMBER_GETPROPNAME,
    IDMEMBER_GETPROPVAL,
    IDMEMBER_SETPROPVAL,
    IDMEMBER_ISPROPREADABLE,
    IDMEMBER_ISPROPWRITABLE,
    IDMEMBER_GETNMETHODS,
    IDMEMBER_FINDMETHOD,
    IDMEMBER_GETMETHODNAME,
    IDMEMBER_GETNPARAMS,
    IDMEMBER_GETPARAMDEFVALUE,
    IDMEMBER_HASRETVAL,
    IDMEMBER_CALLASPROC,
    IDMEMBER_CALLASFUNC
};

static METHODDATA rgmdataDLanguageExtender[] =
{
    { OLESTR("RegisterExtensionAs"), rgpRegisterAs, IDMEMBER_REGISTERAS, 
      IMETH_REGISTERAS,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetNProps"), rgpGetNProp, IDMEMBER_GETNPROPS, 
      IMETH_GETNPROPS,
      CC_CDECL, 0, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("FindProp"), rgpFindProp, IDMEMBER_FINDPROP, 
      IMETH_FINDPROP,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetPropName"), rgpGetPropName, IDMEMBER_GETPROPNAME, 
      IMETH_GETPROPNAME,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetPropVal"), rgpGetPropVal, IDMEMBER_GETPROPVAL, 
      IMETH_GETPROPVAL,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("SetPropVal"), rgpSetPropVal, IDMEMBER_SETPROPVAL, 
      IMETH_SETPROPVAL,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("IsPropReadable"), rgpIsPropReadable, IDMEMBER_ISPROPREADABLE, 
      IMETH_ISPROPREADABLE,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("IsPropWritable"), rgpIsPropWritable, IDMEMBER_ISPROPWRITABLE, 
      IMETH_ISPROPWRITABLE,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetNMethods"), rgpGetNMethods, IDMEMBER_GETNMETHODS, 
      IMETH_GETNMETHODS,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("FindMethod"), rgpFindMethod, IDMEMBER_FINDMETHOD, 
      IMETH_FINDMETHOD,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetMethodName"), rgpGetMethodName, IDMEMBER_GETMETHODNAME, 
      IMETH_GETMETHODNAME,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetNParams"), rgpGetNParams, IDMEMBER_GETNPARAMS, 
      IMETH_GETNPARAMS,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("GetParamDefValue"), rgpGetParamDefValue, IDMEMBER_GETPARAMDEFVALUE, 
      IMETH_GETPARAMDEFVALUE,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("HasRetVal"), rgpHasRetVal, IDMEMBER_HASRETVAL, 
      IMETH_HASRETVAL,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("CallAsProc"), rgpCallAsProc, IDMEMBER_CALLASPROC, 
      IMETH_CALLASPROC,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("CallAsFunc"), rgpCallAsFunc, IDMEMBER_CALLASFUNC, 
      IMETH_CALLASFUNC,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY }
};
INTERFACEDATA idataDLanguageExtender =
{
    rgmdataDLanguageExtender, lengthof(rgmdataDLanguageExtender)
};

static PARAMDATA rgpGetEnabled[] = {{ OLESTR("Enabled"), VT_BOOL|VT_BYREF },
};

static PARAMDATA rgpSetEnabled[] = {{ OLESTR("Enabled"), VT_BOOL|VT_BYREF },
};

enum IMETH_PROPERTYLINK
{
    IMETH_GETENABLED = 0,
    IMETH_SETENABLED = 1,
};

enum IDMEMBER_PROPERTYLINK
{
    IDMEMBER_GETENABLED = 0,
    IDMEMBER_SETENABLED,
};

static METHODDATA rgmdataDPropertyLink[] =
{
    { OLESTR("GetEnabled"), rgpGetEnabled, IDMEMBER_GETENABLED, 
      IMETH_GETENABLED,
      CC_CDECL, 1, DISPATCH_METHOD, VT_EMPTY },
    { OLESTR("SetEnabled"), rgpSetEnabled, IDMEMBER_SETENABLED, 
      IMETH_SETENABLED,
      CC_CDECL, 0, DISPATCH_METHOD, VT_EMPTY },
};

INTERFACEDATA idataDPropertyLink =
{
    rgmdataDPropertyLink, lengthof(rgmdataDPropertyLink)
};
#endif 0
