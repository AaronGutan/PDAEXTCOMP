#ifndef _ADDIN_CLASSFACTORY_H
#define _ADDIN_CLASSFACTORY_H

extern ULONG     g_cObj;
extern ULONG     g_cLock;

class DClassFactory : public IClassFactory
{
 protected:
  ULONG           m_cRef;
  CLSID clsid;

 public:
  DClassFactory(REFCLSID rclsid);
  ~DClassFactory(void);

  //IUnknown members
  STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
  STDMETHODIMP_(ULONG) AddRef(void);
  STDMETHODIMP_(ULONG) Release(void);

  //IClassFactory members
  STDMETHODIMP         CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
  STDMETHODIMP         LockServer(BOOL);
  HRESULT CreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj);
  virtual HRESULT RealCreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj)=0;
  HRESULT CanCreateObject(REFCLSID rclsid);
  virtual HRESULT EnumClasses(UINT i,CLSID *clsid) = 0;
};

#endif // _ADDIN_CLASSFACTORY_H
