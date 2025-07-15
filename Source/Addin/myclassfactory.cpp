#include "commhdr.h"
#pragma hdrstop

#include "myclassfactory.h"

// Для базовой библиотеки AddIn не создаем конкретные объекты
// Это должно быть реализовано в конкретных проектах
const CLSID CLSID_DummyAddIn = {0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

MyClassFactory::MyClassFactory()
  : DClassFactory(CLSID_DummyAddIn)
{
}

MyClassFactory::~MyClassFactory()
{
}

HRESULT MyClassFactory::RealCreateInstance(REFCLSID rclsid, LPUNKNOWN /*pUnkOuter*/, LPUNKNOWN * ppvObj)
{
  // Базовая библиотека не создает конкретные объекты
  // Это должно быть переопределено в конкретных проектах
  *ppvObj = NULL;
  return E_NOTIMPL;
}

HRESULT MyClassFactory::EnumClasses(UINT i, CLSID *clsid)
{
  // Базовая библиотека не имеет классов
  // Это должно быть переопределено в конкретных проектах
  return S_FALSE;
} 