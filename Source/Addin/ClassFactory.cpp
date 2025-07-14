#include "commhdr.h"
#pragma hdrstop

#include "ClassFactory.h"

//Count number of objects and number of locks.
extern ULONG     g_cObj=0;
extern ULONG     g_cLock=0;

/*
 * ObjectDestroyed
 *
 * Purpose:
 *  Function for the objects to call when it gets destroyed.
 *  Since we're in a DLL we only track the number of objects here
 *  letting DllCanUnloadNow take care of the rest.
 */

void ObjectDestroyed(void)
{
  g_cObj--;
  return;
}

void ObjectCreated(void)
{
  ++g_cObj;
  return;
}


/*
 * ClassFactory::ClassFactory
 * ClassFactory::~ClassFactory
 */

DClassFactory::DClassFactory(REFCLSID rclsid)
 : clsid(rclsid)
 , m_cRef(1L)
{
  ObjectCreated();
}

DClassFactory::~DClassFactory(void)
{
  ObjectDestroyed();
}

/*
 * ClassFactory::QueryInterface
 * ClassFactory::AddRef
 * ClassFactory::Release
 */

STDMETHODIMP DClassFactory::QueryInterface(REFIID riid, LPVOID FAR * ppv)
{
  if (IID_IUnknown==riid || IID_IClassFactory==riid) {
    *ppv=this;
    ((IUnknown*)*ppv)->AddRef();
    return NOERROR;
  }
  *ppv=NULL;
  return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) DClassFactory::AddRef(void)
{
  return ++m_cRef;
}

STDMETHODIMP_(ULONG) DClassFactory::Release(void)
{
  if (0L!=--m_cRef)
    return m_cRef;

  delete this;
  return 0L;
}

/*
 * ClassFactory::CreateInstance
 *
 * Purpose:
 *  Instantiates a Polyline object.
 *
 * Parameters:
 *  pUnkOuter     LPUNKNOWN to the controlling IUnknown if we
 *          are being used in an aggregation.
 *  riid      REFIID identifying the interface the caller
 *          desires to have for the new object.
 *  ppvObj      LPVOID FAR * in which to store the desired
 *          interface pointer for the new object.
 *
 * Return Value:
 *  HRESULT     NOERROR if successful, otherwise E_NOINTERFACE
 *          if we cannot support the requested interface.
 */

STDMETHODIMP DClassFactory::CreateInstance
  (LPUNKNOWN pUnkOuter, REFIID riid, LPVOID FAR * ppvObj)
{
  *ppvObj=NULL;

  //Verify that a controlling unknown asks for IUnknown
  if (NULL!=pUnkOuter && IID_IUnknown!=riid)
    return ResultFromScode(CLASS_E_NOAGGREGATION);

  IUnknown *pObj=NULL;
  HRESULT hr=RealCreateInstance(clsid, pUnkOuter, &pObj);
  if(FAILED(hr)) return hr;
  // already AddRef'ed!

  hr=pObj->QueryInterface(riid, ppvObj);
//  pObj->Release();

  //if (!FAILED(hr))
  //  ObjectCreated(); in object constructor..

  return hr;
}

/*
 * ClassFactory::LockServer
 *
 * Purpose:
 *  Increments or decrements the lock count of the DLL.  If the lock
 *  count goes to zero and there are no objects, the DLL is allowed
 *  to unload.  See DllCanUnloadNow.
 *
 * Parameters:
 *  fLock       BOOL specifying whether to increment or
 *          decrement the lock count.
 *
 * Return Value:
 *  HRESULT     NOERROR always.
 */

STDMETHODIMP DClassFactory::LockServer(BOOL fLock)
{
  if (fLock)
    g_cLock++;
  else
    g_cLock--;

  return NOERROR;
}

//static 
HRESULT DClassFactory::CanCreateObject(REFCLSID rclsid)
{
  CLSID clsid;
  for(int i=0; EnumClasses(i,&clsid)==0; i++) {
    if(rclsid==clsid)
      return ResultFromScode(S_OK);
  }
  return ResultFromScode(CLASS_E_CLASSNOTAVAILABLE);
}
