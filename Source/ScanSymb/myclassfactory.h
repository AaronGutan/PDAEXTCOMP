#ifndef _MYCLASSFACTORY_H
#define _MYCLASSFACTORY_H

#ifndef _ADDIN_CLASSFACTORY_H
#include "Addin/ClassFactory.h"
#endif
class MyClassFactory : public DClassFactory
{
public:
  MyClassFactory();
  ~MyClassFactory(void);
  virtual HRESULT RealCreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj);
  virtual HRESULT EnumClasses(UINT i,CLSID *clsid);

};
#define FilePrefix _T("scansymb")
#endif // _MYCLASSFACTORY_H
