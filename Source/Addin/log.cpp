#include "commhdr.h"
#pragma hdrstop
#include <windows.h>
#include <stdio.h>
#include "addin/log.h"

typedef unsigned char uchar;
bool bLogEnabled = false;
_TCHAR dstbuf[2000];
_TCHAR *Bin2Hex(const void * buf, size_t buflen)
{
  const uchar *src;
  _TCHAR *dst = dstbuf;
  for(src = (const uchar *)buf; buflen; src++, buflen-- ){
    int digit = *src / 16;
    *dst++ = (digit < 10) ? (_TCHAR)(digit + _T('0')) : (_TCHAR)(digit + _T('A') - 10);

    digit = *src % 16;
    *dst++ = (digit < 10) ? (_TCHAR)(digit + _T('0')) : (_TCHAR)(digit + _T('A') - 10);
  }
  *dst = _T('\0');
  return dstbuf;
}

const _TCHAR *memtohex(const void *p, size_t len)
{
  return Bin2Hex(p, len);
}

BOOL bStart = TRUE;


_TCHAR name[_MAX_PATH] = _T("");
_TCHAR filename[_MAX_PATH] = _T("");

uchar UnicodeMark[3] = "\xff\xfe";
const _TCHAR * startnewname(HINSTANCE hInstance, const _TCHAR *fname)
{
  _TCHAR *cp;

  ::GetModuleFileName( hInstance, name, _MAX_PATH );
  if((cp=_tcsrchr(name,_T('\\')))!=NULL) {
    if(cp-name==2) *(cp+1)=0; // cut \filename.ext
    else {
      if (cp - name == 0)
        *(cp+1)=0;
      else *cp=0;
    }
  }

  #ifdef _WIN32_WCE
    if(fname[0]==_T('\\')) return fname; // already full path
    int i=_tcslen(name);
    if(i) {
      if(name[i-1]!=_T('\\')) {
        _tcscat(name, _T("\\"));
      }
    }
    _tcscat(name, fname);    
  #else

  _TCHAR drive[_MAX_DRIVE];
  _TCHAR dir[_MAX_DIR];
  _TCHAR dir2[_MAX_DIR];
  _TCHAR file[_MAX_FNAME];
  _TCHAR ext[_MAX_EXT];
  int k;


  _tsplitpath(fname,drive,dir,file,ext);
  if(!drive[0]) {
    if(name[1]==_T(':')) {
      drive[0]=name[0];
      drive[1]=_T(':');
      drive[2]=_T('\0');
      _tcscpy(dir2,name+2);
    }
    else if(name[0]==_T('\\') && name[1]==_T('\\')) { // network path : \\computer\name\subdir\subdir\subdir...
      _tcscpy(dir2,name);
    }
    else {
      dir2[0]=_T('\0');
    }
  }
  else {
    k=drive[0]-((drive[0]>=_T('a'))?_T('a'):_T('A'))+1;
    _tgetdcwd(k,dir2,_MAX_DIR);
    if(dir2[0]&&dir2[1]==_T(':')) {
      _tcscpy(dir2,dir2+2);
    }
  }
  if(dir[0]==_T('.')) {
    if(dir[1]==_T('.')) {        // got current directory name
      if(_tcschr(dir2+1,_T('\\'))) {
        // it is subsubdir
        _TCHAR * cp=_tcsrchr(dir2+1,_T('\\'));
        _tcscpy(cp+1,dir+3);  // prev level dir
      }
      else {  // it is not subdir
        _tcscpy(dir2,dir); // nothing to do;
      }
    }
    else {  // only one dot in new name
      _tcscat(dir2,dir+1);
    }
  }
  else {  // no dots in new name
    if(dir[0]==_T('\\')) _tcscpy(dir2,dir);
    else { _tcscat(dir2,_T("\\")); _tcscat(dir2,dir); }
  }
  _tmakepath(name,drive,dir2,file,ext);
  #endif
  return name;
}

void _Event(const _TCHAR *filename, SYSTEMTIME * t, const _TCHAR * str, bool bPrintTime)
{
  FILE *fp;
  static _TCHAR eos[] = _T("\r\n"); // anti-CodeGuard zalyapa!
  fp =  _tfopen(filename, _T("ab"));
  if( !fp ) return;

  if( bPrintTime )
  {
    _TCHAR str1[20];
    // "00:00:00.00  "
    _stprintf(str1, _T("%02d:%02d:%02d.%02d  "), t->wHour, t->wMinute, t->wSecond, t->wMilliseconds/10);
    fputws(str1, fp);
  }

  fputws(str, fp);
  fputws(eos, fp);

  fclose(fp);
}


void _Event(const _TCHAR *filename, SYSTEMTIME * t, const _TCHAR * fmt, va_list ap)
{
  _TCHAR str[2048];
  if( ap ){
    int nchar;
    nchar = _vsntprintf(str, 2000, fmt, ap);
  }
  else {
    _tcscpy(str, fmt);
  }

  bool bPrintTime = t->wYear != 0;
  _Event(filename, t, str, bPrintTime);
}

void Log(const _TCHAR * fmt, ...)
{
  if (bStart) {
    if (!filename[0])
      _tcscpy(filename, startnewname(0, _T("addin.log")));
    FILE *fp = _tfopen(filename, _T("w"));
    fwrite(UnicodeMark, 2, 1, fp);
    fclose(fp);
    bStart = FALSE;
  }
  va_list ap;
  va_start(ap, fmt);
  SYSTEMTIME t;
  GetSystemTime(&t);
  FILETIME ft, ftLocal;
  ::SystemTimeToFileTime(&t, &ft);
  ::FileTimeToLocalFileTime(&ft, &ftLocal);
  ::FileTimeToSystemTime(&ftLocal, &t);

  _Event(filename, &t, fmt, ap);
  va_end(ap);
}

const _TCHAR * GetFullFileName(HINSTANCE hInstance, const _TCHAR *filename)
{
  return startnewname(hInstance, filename);
}

void OutputString(const _TCHAR *string, const _TCHAR *filename, bool bPrintTime)
{
  long len = 0;
  FILE *fp = _tfopen(filename, _T("r"));
  if (fp) {
    fseek(fp,  0, SEEK_END);
    len = ftell(fp);
    fclose(fp);
  }
  if (!len) {
    FILE *fp = _tfopen(filename, _T("w"));
    fwrite(UnicodeMark, 2, 1, fp);
    fclose(fp);
  }
  SYSTEMTIME t;
  GetSystemTime(&t);
  FILETIME ft, ftLocal;
  ::SystemTimeToFileTime(&t, &ft);
  ::FileTimeToLocalFileTime(&ft, &ftLocal);
  ::FileTimeToSystemTime(&ftLocal, &t);
  _Event(filename, &t, string, bPrintTime);
}

void DeleteLogFile(const _TCHAR *filename)
{
  DeleteFile(filename);
}


void SetRelativeFileName(HINSTANCE hInstance, const _TCHAR *newfilename)
{
  _tcscpy(filename, startnewname(hInstance, newfilename));
}

int GetChar(HANDLE hFile, char *c)
{
  DWORD nBytesRead;
  BOOL bResult = ReadFile(hFile, c, 1, &nBytesRead, NULL) ; 
  if (bResult == FALSE)
    return -1; // error
  if (nBytesRead == 0)
    return 0; // eof
  return 1; // good
}

int GetLine(HANDLE hFile, char *line, unsigned int nMax)
{
  int ret = 0;
  *line = 0;
  char *cp = line;
  for (;;) {
    int ret1 = GetChar(hFile, cp);
    if (ret1 < 0)
      return ret1;
    
    if (ret1 == 0 ) {
      if (ret == 0)
        return -1;
      *cp = 0;
      return 1;
    }
    if (*cp == '\r')
      continue;
    if (*cp == '\n') {
      *cp = 0;
      return ret;
    }
    ret++;
    if (--nMax == 0)
      break;
    cp++;
  }
  *cp = 0;
  return ret;
}

bool GetLogEnabled(const _TCHAR *filename)
{
  _TCHAR str[21];
  str[0] = 0;
  size_t size = 0;
  HANDLE hFile = CreateFile(filename,        // Open filename
                      GENERIC_READ,           // Open for reading
                      FILE_SHARE_READ,        // Share for reading
                      NULL,                   // No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
    return false;
  char line[200];
  int ret = GetLine(hFile, line, 200);
  if (ret <= 0) {
    CloseHandle(hFile);
    return false;
  }
  while (ret >= 0 ) {
    char section[200];
    char *cp;
    if (ret) {
      if (line[0] == '[') {
        const char *cpin = line+1;
        cp = section;

        while (*cpin) { // section name
          if ( *cpin == ']' ) {
            break;
          } else {
            *cp++ = *cpin++;
          }
        }
        if (!*cpin) {
          CloseHandle(hFile);
          return false;
        }
        *cp = 0;
      }
      else {
        const char *cpin = line;

        char recordname[200];
        cp = recordname;
        while (*cpin && *cpin == ' ') {
          cpin++;
        }
        char name[200];
        char *cName = name;
        //*cName++ = *cpin;
        *cName = 0;
        char value[200];
        char *cValue = value;
        char comment[200];
        char *cComment = comment;

        bool bContinue = true;
        bool bComment = false;
        bool bEq = false;

        while ( *cpin && bContinue ) {
          switch ( *cpin ) {

            case '=' : {
              if ( bComment ) {
                *cComment++ = *cpin;
                *cComment = 0;
              }
              else if ( bEq ) {
                *cValue++ = *cpin;
                *cValue = 0;
              }
              else
                bEq = true;
              break;
            }

            case '\n' : {
              bContinue = false;
              break;
            }
      /*
            case ' ' : {
              if ( !e.IsEmpty() ) {
                e += c;
              }
              break;
            }
      */
            case '\r' : { // this is not written
              break;
            }

            case ';' : {
              if ( !name[0] && !value[0] && !bEq ) {
                bComment = true;
                *cComment++ = *cpin;
                *cComment = 0;
                break;
              }
              // no break; pass to default
            }

            default : {
              if ( bComment ) {
                *cComment++ = *cpin;
                *cComment = 0;
              }
              else if ( bEq ) {
                *cValue++ = *cpin;
                *cValue = 0;
              }
              else {
                *cName++ = *cpin;
                *cName = 0;
              }
              break;
            }

          } //switch
          cpin++;
        } //while
        if (_stricmp(section, "Options")==0 && _stricmp(name, "Log")==0) {
          int i = atoi(value);
          CloseHandle(hFile);
          return i != 0;
        }
      }
    } // if (ret)
    ret = GetLine(hFile, line, 200);
  }
  CloseHandle(hFile);
  return false;

}

void SetupLog(HINSTANCE hInstance, const _TCHAR *fileprefix)
{
  _TCHAR str[_MAX_PATH];
   _tcscpy(str, fileprefix);
   _tcscat(str, _T(".log"));
  _tcscpy(filename, startnewname(hInstance, str));
   _tcscpy(str, fileprefix);
   _tcscat(str, _T(".ini"));
   bLogEnabled = GetLogEnabled(startnewname(hInstance, str));
}


