#include <windows.h>
#include <tchar.h>

// GUID для тестовой компоненты
const CLSID CLSID_TestComponent = {0x12345678,0x1234,0x1234,{0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0}};

// Простой класс компоненты
class CTestComponent : public IUnknown
{
private:
    LONG m_cRef;
public:
    CTestComponent() : m_cRef(1) {}
    
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (riid == IID_IUnknown) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }
    
    STDMETHODIMP_(ULONG) Release()
    {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) delete this;
        return cRef;
    }
};

// Фабрика классов
class CTestClassFactory : public IClassFactory
{
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
    
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
    {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        
        CTestComponent* pObj = new CTestComponent();
        if (!pObj) return E_OUTOFMEMORY;
        
        HRESULT hr = pObj->QueryInterface(riid, ppv);
        pObj->Release();
        return hr;
    }
    
    STDMETHODIMP LockServer(BOOL fLock) { return S_OK; }
};

// Глобальные переменные
static LONG g_cLocks = 0;
static LONG g_cObjects = 0;

// Экспортируемые функции
STDAPI DllCanUnloadNow()
{
    return (g_cLocks == 0 && g_cObjects == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid == CLSID_TestComponent) {
        CTestClassFactory* pCF = new CTestClassFactory();
        if (!pCF) return E_OUTOFMEMORY;
        
        HRESULT hr = pCF->QueryInterface(riid, ppv);
        pCF->Release();
        return hr;
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllRegisterServer()
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH);
    
    TCHAR szKey[256];
    _stprintf(szKey, _T("CLSID\\{12345678-1234-1234-1234-123456789ABC}"));
    
    HKEY hKey;
    if (RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hKey) == ERROR_SUCCESS) {
        RegSetValue(hKey, NULL, REG_SZ, _T("Test Component"), 0);
        
        HKEY hSubKey;
        if (RegCreateKey(hKey, _T("InprocServer32"), &hSubKey) == ERROR_SUCCESS) {
            RegSetValue(hSubKey, NULL, REG_SZ, szPath, 0);
            RegSetValueEx(hSubKey, _T("ThreadingModel"), 0, REG_SZ, (BYTE*)_T("both"), sizeof(_T("both")));
            RegCloseKey(hSubKey);
        }
        
        if (RegCreateKey(hKey, _T("ProgID"), &hSubKey) == ERROR_SUCCESS) {
            RegSetValue(hSubKey, NULL, REG_SZ, _T("TestComponent.1"), 0);
            RegCloseKey(hSubKey);
        }
        
        if (RegCreateKey(hKey, _T("VersionIndependentProgID"), &hSubKey) == ERROR_SUCCESS) {
            RegSetValue(hSubKey, NULL, REG_SZ, _T("TestComponent"), 0);
            RegCloseKey(hSubKey);
        }
        
        RegCloseKey(hKey);
    }
    
    // Создаем ProgID
    if (RegCreateKey(HKEY_CLASSES_ROOT, _T("TestComponent"), &hKey) == ERROR_SUCCESS) {
        RegSetValue(hKey, NULL, REG_SZ, _T("Test Component"), 0);
        
        if (RegCreateKey(hKey, _T("CLSID"), &hSubKey) == ERROR_SUCCESS) {
            RegSetValue(hSubKey, NULL, REG_SZ, _T("{12345678-1234-1234-1234-123456789ABC}"), 0);
            RegCloseKey(hSubKey);
        }
        
        RegCloseKey(hKey);
    }
    
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteKey(HKEY_CLASSES_ROOT, _T("TestComponent\\CLSID"));
    RegDeleteKey(HKEY_CLASSES_ROOT, _T("TestComponent"));
    RegDeleteKey(HKEY_CLASSES_ROOT, _T("CLSID\\{12345678-1234-1234-1234-123456789ABC}\\VersionIndependentProgID"));
    RegDeleteKey(HKEY_CLASSES_ROOT, _T("CLSID\\{12345678-1234-1234-1234-123456789ABC}\\ProgID"));
    RegDeleteKey(HKEY_CLASSES_ROOT, _T("CLSID\\{12345678-1234-1234-1234-123456789ABC}\\InprocServer32"));
    RegDeleteKey(HKEY_CLASSES_ROOT, _T("CLSID\\{12345678-1234-1234-1234-123456789ABC}"));
    return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
} 