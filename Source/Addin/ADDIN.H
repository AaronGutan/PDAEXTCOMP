#ifndef _ADDIN_ADDIN_H
#define _ADDIN_ADDIN_H


#include "addintlb.h"

struct Property
{
  unsigned int m_propIds;
  unsigned int m_propNum;
};

struct Method
{
  unsigned int m_methIds;
  unsigned int m_methNum;
};

// AddIn.h: Definition of the CAddIn class
//
//////////////////////////////////////////////////////////////////////

/*
  These are error codes that should be used in AddError
*/
enum AddInErrors {
                  ADDIN_E_NONE = 1000,
                  ADDIN_E_ORDINARY = 1001,
                  ADDIN_E_ATTENTION = 1002,
                  ADDIN_E_IMPORTANT = 1003,
                  ADDIN_E_VERY_IMPORTANT = 1004,
                  ADDIN_E_INFO = 1005,
                  ADDIN_E_FAIL = 1006,
                  ADDIN_E_MSGBOX_ATTENTION = 1007,
                  ADDIN_E_MSGBOX_INFO = 1008,
                  ADDIN_E_MSGBOX_FAIL = 1009
                 };

class DAddIn;

/////////////////////////////////////////////////////////////////////////////
// DInitDone
class DInitDone 
: public IInitDone
{
public:
  DInitDone(DAddIn *parent);
  virtual ~DInitDone();
  STDMETHOD (QueryInterface)(THIS_ REFIID riid, VOID FAR * FAR * ppvObj) ; 
  STDMETHOD_(ULONG,AddRef)  (THIS);                                       
  STDMETHOD_(ULONG,Release) (THIS);
  STDMETHOD(Init)(IDispatch *pConnection);
  STDMETHOD(Done)();
  STDMETHOD(GetInfo)(SAFEARRAY **pInfo);
  DAddIn *m_parent;
};

/////////////////////////////////////////////////////////////////////////////
// DLanguageExtender
class DLanguageExtender  
: public ILanguageExtender
{
public:
  DLanguageExtender(DAddIn *parent);
  virtual ~DLanguageExtender();
  STDMETHOD (QueryInterface)(THIS_ REFIID riid, VOID FAR * FAR * ppvObj) ; 
  STDMETHOD_(ULONG,AddRef)  (THIS);                                       
  STDMETHOD_(ULONG,Release) (THIS);

  // ILanguageExtender
public:

  STDMETHOD(RegisterExtensionAs)(BSTR *bstrExtensionName);

  STDMETHOD(GetNProps)(long *plProps);
  STDMETHOD(FindProp)(BSTR bstrPropName,long *plPropNum);
  STDMETHOD(GetPropName)(long lPropNum,long lPropAlias,BSTR *pbstrPropName);
  STDMETHOD(GetPropVal)(long lPropNum,VARIANT *pvarPropVal);
  STDMETHOD(SetPropVal)(long lPropNum,VARIANT *pvarPropVal);
  STDMETHOD(IsPropReadable)(long lPropNum,BOOL *pboolPropRead);
  STDMETHOD(IsPropWritable)(long lPropNum,BOOL *pboolPropWrite);

  STDMETHOD(GetNMethods)(long *plMethods);
  STDMETHOD(FindMethod)(BSTR bstrMethodName,long *plMethodNum);
  STDMETHOD(GetMethodName)(long lMethodNum,long lMethodAlias,BSTR *pbstrMethodName);
  STDMETHOD(GetNParams)(long lMethodNum,long *plParams);
  STDMETHOD(GetParamDefValue)(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue);
  STDMETHOD(HasRetVal)(long lMethodNum,BOOL *pboolRetValue);
  STDMETHOD(CallAsProc)(long lMethodNum,SAFEARRAY **paParams);
  STDMETHOD(CallAsFunc)(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams);
  DAddIn *m_parent;
};

/////////////////////////////////////////////////////////////////////////////
// DPropertyLink
class DPropertyLink 
: public IPropertyLink 
{
public:
  DPropertyLink(DAddIn *parent);
  virtual ~DPropertyLink();
  STDMETHOD (QueryInterface)(THIS_ REFIID riid, VOID FAR * FAR * ppvObj) ; 
  STDMETHOD_(ULONG,AddRef)  (THIS);                                       
  STDMETHOD_(ULONG,Release) (THIS);
// IPropertyLink
public:
  STDMETHOD(get_Enabled)(BOOL *boolEnabled);
  STDMETHOD(put_Enabled)(BOOL boolEnabled);
  DAddIn *m_parent;
    
};

/////////////////////////////////////////////////////////////////////////////
// DPropertyPage
class DPropertyPage : 
    public IPropertyPage
{
public:
  DPropertyPage(DAddIn *parent);
  ~DPropertyPage();
  STDMETHOD (QueryInterface)(THIS_ REFIID riid, VOID FAR * FAR * ppvObj) ; 
  STDMETHOD_(ULONG,AddRef)  (THIS);                                       
  STDMETHOD_(ULONG,Release) (THIS);
// IPropertyPage
public: 
      /* Unnecessary functions are simply commented instead of calling 
         parent class implementation 
      */
    //STDMETHOD(SetPageSite)(IPropertyPageSite *pPageSite);
    //STDMETHOD(Activate)(HWND hWndParent,LPCRECT prc,BOOL bModal);
    STDMETHOD(Deactivate)();
    //STDMETHOD(GetPageInfo)(PROPPAGEINFO *pPageInfo);
    //STDMETHOD(SetObjects)(ULONG cObjects,IUnknown **ppUnk);
    //STDMETHOD(Show)(UINT nCmdShow);
    //STDMETHOD(Move)(LPCRECT prc);
    //STDMETHOD(IsPageDirty)();
    STDMETHOD(Apply)();
  DAddIn *m_parent;
    
};
/////////////////////////////////////////////////////////////////////////////
// DAddIn

class DAddIn : 
    public IUnknown
{
  ULONG counter;
public:
  STDMETHOD (QueryInterface)(THIS_ REFIID riid, VOID FAR * FAR * ppvObj) ; 
  STDMETHOD_(ULONG,AddRef)  (THIS);                                       
  STDMETHOD_(ULONG,Release) (THIS);
  DAddIn(); // { m_dwTitleID = IDS_PROPPAGE_CAPTION; }
  virtual ~DAddIn();
  virtual BOOL FillPropsAndMethods(void) = 0;

    IErrorLog *m_iErrorLog;
    IPropertyProfile *m_iProfile;
    IStatusLine *m_iStatusLine;
    IAsyncEvent *m_iAsyncEvent;
    IDispatch * m_iConnect;

    DInitDone m_InitDone;
    DLanguageExtender m_LanguageExtender;
    DPropertyLink m_PropertyLink;
// Methods

    BOOL LoadProperties();
    void SaveProperties();
    void TermString(UINT uiResID,long nAlias, _TCHAR *buff, unsigned int lenbuff);
      // These two methods is convenient way to access function 
      // parameters from SAFEARRAY vector of variants
    VARIANT GetNParam(SAFEARRAY *pArray,long lIndex);
    void PutNParam(SAFEARRAY *pArray,long lIndex,VARIANT vt);
    BOOL m_boolEnabled;    // This property enables or disables component
      // This method is very useful in ILanguageExtender implementation
    Property m_props[100];
    Method m_methods[100];
    unsigned int m_nProps;
    unsigned int m_nMethods;
// placeholders
  virtual HRESULT Init(void);
  virtual HRESULT Done(void);
  virtual HRESULT GetInfo(SAFEARRAY **pInfo);
  HRESULT RegisterExtensionAs(BSTR *bstrExtensionName);
  HRESULT GetNProps(long *plProps);
  HRESULT FindProp(BSTR bstrPropName,long *plPropNum);
  HRESULT GetPropName(long lPropNum,long lPropAlias,BSTR *pbstrPropName);
  virtual HRESULT GetPropVal(long lPropNum,VARIANT *pvarPropVal);
  virtual HRESULT SetPropVal(long lPropNum,VARIANT *pvarPropVal);
  virtual HRESULT IsPropReadable(long lPropNum,BOOL *pboolPropRead);
  virtual HRESULT IsPropWritable(long lPropNum,BOOL *pboolPropWrite);

  HRESULT GetNMethods(long *plMethods);
  HRESULT FindMethod(BSTR bstrMethodName,long *plMethodNum);
  HRESULT GetMethodName(long lMethodNum,long lMethodAlias,BSTR *pbstrMethodName);
  virtual HRESULT GetNParams(long lMethodNum,long *plParams);
  virtual HRESULT GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue);
  virtual HRESULT HasRetVal(long lMethodNum,BOOL *pboolRetValue);
  virtual HRESULT CallAsProc(long lMethodNum,SAFEARRAY **paParams);
  virtual HRESULT CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams);
  virtual HRESULT get_Enabled(BOOL *boolEnabled);
  virtual HRESULT put_Enabled(BOOL boolEnabled);
};

#endif // _ADDIN_ADDIN_H
