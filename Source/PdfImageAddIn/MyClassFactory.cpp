#include "commhdr.h"
#pragma hdrstop

#include "myclassfactory.h"
#include "PdfImageAddIn.h"

// GUID для компоненты (сгенерированный случайным образом)
const CLSID CLSID_PdfImageAddIn = {0x12345678,0x9ABC,0xDEF0,{0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0}};

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