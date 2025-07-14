
#define USE_LOG() 0

#if USE_LOG()
extern bool bLogEnabled;
#define LOG(exp) if(bLogEnabled) exp
_TCHAR *Bin2Hex(const void * buf, size_t buflen);
void Log(const _TCHAR * fmt, ...);
const _TCHAR *memtohex(const void *p, size_t len);
//bool GetLogEnabled(void);
void SetupLog(HINSTANCE hInstance, const _TCHAR *fileprefix);
#else
#define LOG(exp)
#endif // USE_LOG()
const _TCHAR * GetFullFileName(HINSTANCE hInstance, const _TCHAR *filename);
void SetFileName(const _TCHAR *filename);
//void SetRelativeFileName(HINSTANCE hInstance, const _TCHAR *filename);
void OutputString(const _TCHAR *string, const _TCHAR *filename, bool bPrintTime);
void DeleteLogFile(const _TCHAR *filename);

