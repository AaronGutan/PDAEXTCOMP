#include "commhdr.h"
#pragma hdrstop

//#include "lib/lang.h"
#include "MyClassFactory.h"
#include "ScanAddIn.h"
#include "addin/log.h"

const CLSID CLSID_ScanAddIn = {0xBB44CFF2,0x503B,0x4BAF,{0xA3,0x83,0x40,0x9A,0x20,0x9A,0xCA,0xFD}};

MyClassFactory::MyClassFactory()
: DClassFactory(CLSID_ScanAddIn)
{
}

MyClassFactory::~MyClassFactory(void)
{
}

HRESULT MyClassFactory::RealCreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj)
{
  *ppvObj=NULL;
  if(CLSID_ScanAddIn==rclsid) {
    DScanAddIn *pAddin = new DScanAddIn();
    *ppvObj = (IUnknown *)pAddin;
    LOG(Log(_T("Object created %d"), pAddin));
  }
  if(!*ppvObj)
    return ResultFromScode(E_OUTOFMEMORY);

  return NOERROR;
}


HRESULT MyClassFactory::EnumClasses(UINT i,CLSID *clsid)
{
  static const CLSID *array[]={
    &CLSID_ScanAddIn,
  };
  if(i<sizeof(array)/sizeof(array[0])) {
    *clsid=*array[i];
    return ResultFromScode(S_OK);
  }
  return ResultFromScode(S_FALSE);
}

