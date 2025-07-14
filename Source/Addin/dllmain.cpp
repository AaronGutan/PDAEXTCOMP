#include "commhdr.h"
#pragma hdrstop


#include "myclassfactory.h"
#include "log.h"
#include <stdio.h>


extern HINSTANCE g_hinst = 0;
#ifdef __TURBOC__
BOOL WINAPI DllEntryPoint(HINSTANCE hinstDLL,DWORD /*fdwReason*/,LPVOID)//lpvReserved)
#else 
#ifdef _WIN32_WCE
BOOL WINAPI
   DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpReserved)
#else //_WIN32_WCE
// MS VC++ rename it in 6.0?
extern "C" int APIENTRY
   DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
#endif _WIN32_WCE
#endif
{
  if (dwReason == DLL_PROCESS_ATTACH) {
    #if USE_LOG()
    SetupLog((HINSTANCE)hinstDLL, FilePrefix);
    #endif //USE_LOG()
  }
  LOG(Log(_T("DllMain %d"), dwReason));
  g_hinst=(HINSTANCE)hinstDLL;
  return TRUE;
}

int WINAPI DllEntryPoint(HINSTANCE hInstance, ULONG reason, LPVOID)
{
  g_hinst=(HINSTANCE)hInstance;
  switch (reason) {
    case DLL_PROCESS_ATTACH: ;
      #if USE_LOG()
      SetupLog((HINSTANCE)hinstDLL, FilePrefix);
      #endif //USE_LOG()
      LOG(Log(_T("DllEntryPoint")));
      break;
  }
  return true;
}

/*
 * DllGetClassObject
 *
 * Purpose:
 *  Provides an IClassFactory for a given CLSID that this DLL is
 *  registered to support.  This DLL is placed under the CLSID
 *  in the registration database as the InProcServer.
 *
 * Parameters:
 *  clsID       REFCLSID that identifies the class factory
 *          desired.  Since this parameter is passed this
 *          DLL can handle any number of objects simply
 *          by returning different class factories here
 *          for different CLSIDs.
 *
 *  riid      REFIID specifying the interface the caller wants
 *          on the class object, usually IID_ClassFactory.
 *
 *  ppv       LPVOID FAR * in which to return the interface
 *          pointer.
 *
 * Return Value:
 *  HRESULT     NOERROR on success, otherwise an error code.
 */

#if !defined __TURBOC__ //?? MS VC++ like use .DEF file - <objbase.h> has prototype without declspec().
  #undef  DLLEXPORT
  #define DLLEXPORT
#endif

STDAPI  DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID FAR* ppv)
{
  *ppv=0;

  LOG(Log(_T("DllGetClassObject")));
  DClassFactory *pClassFactory = new MyClassFactory();
  if (pClassFactory == NULL)
    return ResultFromScode(E_OUTOFMEMORY);

  HRESULT hr=pClassFactory->CanCreateObject(rclsid);
  if(FAILED(hr)) {
    pClassFactory->Release();
    return hr;
  }

  HRESULT hRes = pClassFactory->QueryInterface(riid, ppv);
  pClassFactory->Release();

  return hRes;
}

/*
 * DllCanUnloadNow
 *
 * Purpose:
 *  Answers if the DLL can be freed, that is, if there are no
 *  references to anything this DLL provides.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL      TRUE if nothing is using us, FALSE otherwise.
 */
STDAPI  DllCanUnloadNow(void)
{
  SCODE   sc;

  //Our answer is whether there are any object or locks
  sc=(0L==g_cObj && 0L==g_cLock) ? S_OK : S_FALSE;
  return ResultFromScode(sc);
}

/*
HKEY_CLASSES_ROOT\AX = AX
HKEY_CLASSES_ROOT\AX\CLSID = {06C08DC0-8EAB-11d2-B17E-0020AF38D993}
HKEY_CLASSES_ROOT\CLSID\{06C08DC0-8EAB-11d2-B17E-0020AF38D993} = AX Component
HKEY_CLASSES_ROOT\CLSID\{06C08DC0-8EAB-11d2-B17E-0020AF38D993}\InprocServer32 = E:\OBJEDIT\AX\ax.dll
HKEY_CLASSES_ROOT\CLSID\{06C08DC0-8EAB-11d2-B17E-0020AF38D993}\ProgID = AX
HKEY_CLASSES_ROOT\CLSID\{06C08DC0-8EAB-11d2-B17E-0020AF38D993}\NotInsertable =
*/
//#include "lib/str.h"
///static char ServerName[] = "DPumpServer";
//_TCHAR MenuText[] = _T("Send Picture To Mobile");
// OLE 2.0 entry point for registering DLL, no locale info passed
STDAPI DllRegisterServer()
{
  LOG(Log(_T("DllRegisterServer")));
  DClassFactory *pClassFactory = new MyClassFactory();
  if (pClassFactory == NULL)
    return ResultFromScode(E_OUTOFMEMORY);

  _TCHAR filename[_MAX_PATH];
  GetModuleFileName(g_hinst,filename,sizeof filename);

  _TCHAR objName[300];
  _tcscpy(objName, _T("Addin."));
  _TCHAR strbuff[200];
  ::LoadString(g_hinst, 100, strbuff, 200);
  _tcscat(objName, strbuff);

  _TCHAR objName1[300];
  _tcscpy(objName1, objName);
  _tcscat(objName1, _T(".1"));
  HRESULT hr;
  HKEY hkeyObject1;
  HKEY hkeyCLSID1;
  hr = RegCreateKeyEx(HKEY_CLASSES_ROOT, objName, 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hkeyObject1, 0);
  
  hr = RegCreateKeyEx(hkeyObject1,_T("CLSID"), 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hkeyCLSID1, 0);

  HKEY hkeyObject2;
  HKEY hkeyCLSID2;
  hr = RegCreateKeyEx(HKEY_CLASSES_ROOT, objName1, 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hkeyObject2, 0);
  
  hr = RegCreateKeyEx(hkeyObject2,_T("CLSID"), 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hkeyCLSID2, 0);
  
  HKEY hkeyCLSID;
  RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("CLSID"), 0, KEY_WRITE, &hkeyCLSID);
  ///Str ProgID=ServerName;
  CLSID clsid;

  for(int i=0; pClassFactory->EnumClasses(i,&clsid)==0; i++) {
    HRESULT  hr;
    LPOLESTR str;
    hr = StringFromCLSID (clsid, &str);
    if (SUCCEEDED(hr)) {
      _TCHAR  key[255];
      if (i ==0) {
        _stprintf (key, _T("%s"), str);
        RegSetValueEx(hkeyCLSID1,_T(""),0, REG_SZ, (BYTE *)key, (_tcslen(key)+1)*sizeof(_TCHAR));
        RegSetValueEx(hkeyCLSID2,_T(""),0, REG_SZ, (BYTE *)key, (_tcslen(key)+1)*sizeof(_TCHAR));
      }
      //Str temp= WStr(str).Get();
      HKEY hKeyClass;
      HKEY hKey;
      hr = RegCreateKeyEx(hkeyCLSID,str, 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hKeyClass, 0);
      hr = RegCreateKeyEx(hKeyClass,_T("InprocServer32"), 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hKey, 0);
      RegSetValueEx(hKey,_T(""),0, REG_SZ,(BYTE *)filename,(_tcslen(filename)+1)*sizeof(_TCHAR));
      _TCHAR both[] = _T("both");
      RegSetValueEx(hKey,_T("ThreadingModel"),0, REG_SZ,(BYTE *)both,(_tcslen(both)+1)*sizeof(_TCHAR));
      RegCloseKey(hKey);
      hr = RegCreateKeyEx(hKeyClass,_T("NotInsertable"), 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hKey, 0);
      RegSetValueEx(hKey,_T(""),0, REG_SZ,(BYTE *)_T(""),0);
      RegCloseKey(hKey);
      hr = RegCreateKeyEx(hKeyClass,_T("ProgID"), 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hKey, 0);
      RegSetValueEx(hKey,_T(""),0, REG_SZ,(BYTE *)objName1,(_tcslen(objName1)+1)*sizeof(_TCHAR));
      RegCloseKey(hKey);
      hr = RegCreateKeyEx(hKeyClass,_T("VersionIndependentProgID"), 0, _T(""), 0, KEY_READ | KEY_WRITE, 0, &hKey, 0);
      RegSetValueEx(hKey,_T(""),0, REG_SZ,(BYTE *)objName,(_tcslen(objName)+1)*sizeof(_TCHAR));
      RegCloseKey(hKey);
      RegCloseKey(hKeyClass);

      /*if(ERROR_SUCCESS!=RegCreateKeyEx(hkeyCLSID, temp, 0, _T(""), 0, 
              KEY_READ | KEY_WRITE, 0, &hKeyClass, 0))
              return -1;
      if(ERROR_SUCCESS!=RegCreateKeyEx(hKeyClass, _T("InprocServer32"), 0, _T(""), 0, 
              KEY_READ | KEY_WRITE, 0, &hKey, 0))
              return -1;
      LONG ret;
      Str Apartment = _T("Apartment");
      ret = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (uchar*)filename,_tcslen(filename));
      ret = RegSetValueEx(hKey, _T("ThreadModel"), 0, REG_SZ, (uchar*)Apartment.Get(),_tcslen(Apartment));
      RegCloseKey(hKey);
      if(ERROR_SUCCESS!=RegCreateKeyEx(hKeyClass, _T("NotInsertable"), 0, _T(""), 0, 
              KEY_READ | KEY_WRITE, 0, &hKey, 0))
              return -1;
      RegCloseKey(hKeyClass);*/
      CoTaskMemFree(str);
    }
  }
  RegCloseKey(hkeyCLSID1);
  RegCloseKey(hkeyObject1);
  RegCloseKey(hkeyCLSID2);
  RegCloseKey(hkeyObject2);
  RegCloseKey(hkeyCLSID);
  return 0;
}
// OLE 2.0 entry point for unregistering DLL
STDAPI DllUnregisterServer()
{
  LOG(Log(_T("DllUnregisterServer")));
  DClassFactory *pClassFactory = new MyClassFactory();
  if (pClassFactory == NULL)
    return ResultFromScode(E_OUTOFMEMORY);

  _TCHAR objName[300];
  _tcscpy(objName, _T("Addin."));
  _TCHAR strbuff[200];
  ::LoadString(g_hinst, 100, strbuff, 200);
  _tcscat(objName, strbuff);

  HRESULT hr;
  HKEY hkeyObject1;
  hr = RegOpenKeyEx(HKEY_CLASSES_ROOT, objName, 0, KEY_READ | KEY_WRITE, &hkeyObject1);
  hr = RegDeleteKey(hkeyObject1, _T("CLSID"));
  RegCloseKey(hkeyObject1);
  hr = RegDeleteKey(HKEY_CLASSES_ROOT, objName);
  
  _tcscat(objName, _T(".1"));
  hr = RegOpenKeyEx(HKEY_CLASSES_ROOT, objName, 0, KEY_READ | KEY_WRITE, &hkeyObject1);
  hr = RegDeleteKey(hkeyObject1, _T("CLSID"));
  RegCloseKey(hkeyObject1);
  hr = RegDeleteKey(HKEY_CLASSES_ROOT, objName);
  

  HKEY hkeyCLSID;
  RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("CLSID"), 0, KEY_WRITE, &hkeyCLSID);
  ///Str ProgID=ServerName;
  CLSID clsid;

  for(int i=0; pClassFactory->EnumClasses(i,&clsid)==0; i++) {
    HRESULT  hr;
    LPOLESTR str;
    hr = StringFromCLSID (clsid, &str);
    if (SUCCEEDED(hr)) {
      _TCHAR  key[255];
      wsprintf (key, _T("%s"), str);
      hr = RegOpenKeyEx(hkeyCLSID,key, 0, KEY_READ | KEY_WRITE, &hkeyObject1);
      hr = RegDeleteKey(hkeyObject1, _T("InprocServer32"));
      hr = RegDeleteKey(hkeyObject1, _T("NotInsertable"));
      hr = RegDeleteKey(hkeyObject1, _T("ProgID"));
      hr = RegDeleteKey(hkeyObject1, _T("VersionIndependentProgID"));
      RegCloseKey(hkeyObject1);
      RegDeleteKey(hkeyCLSID,key);
      CoTaskMemFree(str);
    }
  }
  return 0;
}

