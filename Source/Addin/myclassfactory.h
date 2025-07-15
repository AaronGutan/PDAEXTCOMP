#ifndef _MYCLASSFACTORY_H
#define _MYCLASSFACTORY_H

#include "ClassFactory.h"

class MyClassFactory : public DClassFactory
{
public:
  MyClassFactory();
  virtual ~MyClassFactory();
  
  virtual HRESULT RealCreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj);
  virtual HRESULT EnumClasses(UINT i,CLSID *clsid);
};

#endif // _MYCLASSFACTORY_H 