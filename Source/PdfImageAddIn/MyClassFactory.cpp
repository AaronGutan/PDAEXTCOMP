#include "commhdr.h"
#pragma hdrstop

#include "myclassfactory.h"
#include "PdfImageAddIn.h"

// GUID для компоненты (уникальный идентификатор)
const CLSID CLSID_PdfImageAddIn = {0x5CFC0429,0xC04D,0x4B47,{0x98,0x52,0xB6,0xC3,0xBA,0xF4,0xB7,0xF9}};

MyClassFactory::MyClassFactory()
  : DClassFactory(CLSID_PdfImageAddIn)
{
}

MyClassFactory::~MyClassFactory()
{
}

HRESULT MyClassFactory::RealCreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj)
{
  HRESULT hr = E_FAIL;
  IUnknown * punkObject = NULL;
  
  if (rclsid == CLSID_PdfImageAddIn) {
    punkObject = new DPdfImageAddIn();
  }
  
  if (punkObject) {
    punkObject->AddRef();
    hr = S_OK;
  }
  
  *ppvObj = punkObject;
  return hr;
}

HRESULT MyClassFactory::EnumClasses(UINT i, CLSID *clsid)
{
  switch (i) {
    case 0:
      *clsid = CLSID_PdfImageAddIn;
      return S_OK;
    default:
      return S_FALSE;
  }
} 