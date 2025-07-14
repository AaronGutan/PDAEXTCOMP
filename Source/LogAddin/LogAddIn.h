#ifndef _LOGADDIN_H
#define _LOGADDIN_H

#ifndef _ADDIN_ADDIN_H
#include "Addin/Addin.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// DLogAddIn

class DLogAddIn : 
    public DAddIn
{

public:
  DLogAddIn(); // { m_dwTitleID = IDS_PROPPAGE_CAPTION; }
  ~DLogAddIn();

    enum 
    {
        propIsLogEnabled =0,
        propFileName,
        propPrintTime,
        LastProp      // Always last
    };

    enum 
    {
        methOutput = 0,             // Output(str);
        methDelete,                 // Delete();
        methSetFileName,            // SetFileName(name, bRelative);
        LastMethod      // Always last
    };

  //virtual HRESULT Init(void);
  //virtual HRESULT Done(void);
  //virtual HRESULT GetInfo(SAFEARRAY **pInfo);
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
  //virtual HRESULT get_Enabled(BOOL *boolEnabled);
  //virtual HRESULT put_Enabled(BOOL boolEnabled);
  _TCHAR m_FileName[_MAX_PATH];
  BOOL m_bPrintTime;
};

#endif // _LOGADDIN_H
