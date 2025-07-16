#include "commhdr.h"
#pragma hdrstop

#include "PdfImageAddIn_Simple.h"
#include "../Addin/Log.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

extern HINSTANCE g_hinst;

/////////////////////////////////////////////////////////////////////////////
// DPdfImageAddIn (упрощенная версия)

DPdfImageAddIn::DPdfImageAddIn()
{
    // Инициализация переменных
    _tcscpy(m_PdfFileName, _T(""));
    _tcscpy(m_ImageFileName, _T(""));
    _tcscpy(m_LastError, _T(""));
    
    m_ImageX = 0.0;
    m_ImageY = 0.0;
    m_ImageWidth = 0.0;
    m_ImageHeight = 0.0;
    m_PageNumber = 1;
    
    m_bPdfLoaded = FALSE;
    m_boolEnabled = TRUE;
    
    LOG(Log(_T("PdfImageAddIn (Simple) создан")));
}

DPdfImageAddIn::~DPdfImageAddIn()
{
    // Очистка списка изображений
    m_ImageList.clear();
    
    LOG(Log(_T("PdfImageAddIn (Simple) уничтожен")));
}

BOOL DPdfImageAddIn::FillPropsAndMethods(void)
{
    // Заполнение свойств
    m_props[0].m_propIds = IDS_TERM_ENABLED;
    m_props[0].m_propNum = propIsEnabled;
    m_props[1].m_propIds = IDS_TERM_PDF_FILENAME;
    m_props[1].m_propNum = propPdfFileName;
    m_props[2].m_propIds = IDS_TERM_IMAGE_FILENAME;
    m_props[2].m_propNum = propImageFileName;
    m_props[3].m_propIds = IDS_TERM_IMAGE_X;
    m_props[3].m_propNum = propImageX;
    m_props[4].m_propIds = IDS_TERM_IMAGE_Y;
    m_props[4].m_propNum = propImageY;
    m_props[5].m_propIds = IDS_TERM_IMAGE_WIDTH;
    m_props[5].m_propNum = propImageWidth;
    m_props[6].m_propIds = IDS_TERM_IMAGE_HEIGHT;
    m_props[6].m_propNum = propImageHeight;
    m_props[7].m_propIds = IDS_TERM_PAGE_NUMBER;
    m_props[7].m_propNum = propPageNumber;
    m_props[8].m_propIds = IDS_TERM_LAST_ERROR;
    m_props[8].m_propNum = propLastError;
    m_nProps = LastProp;
    
    // Заполнение методов
    m_methods[0].m_methIds = IDS_TERM_LOAD_PDF;
    m_methods[0].m_methNum = methLoadPdf;
    m_methods[1].m_methIds = IDS_TERM_ADD_IMAGE;
    m_methods[1].m_methNum = methAddImage;
    m_methods[2].m_methIds = IDS_TERM_SAVE_PDF;
    m_methods[2].m_methNum = methSavePdf;
    m_methods[3].m_methIds = IDS_TERM_GET_PAGE_SIZE;
    m_methods[3].m_methNum = methGetPageSize;
    m_methods[4].m_methIds = IDS_TERM_CLEAR_IMAGES;
    m_methods[4].m_methNum = methClearImages;
    m_methods[5].m_methIds = IDS_TERM_GET_PAGE_COUNT;
    m_methods[5].m_methNum = methGetPageCount;
    m_methods[6].m_methIds = IDS_TERM_TEST_CONNECTION;
    m_methods[6].m_methNum = methTestConnection;
    m_nMethods = LastMethod;
    
    LOG(Log(_T("PdfImageAddIn (Simple): Properties and methods filled")));
    return TRUE;
}

// Упрощенные методы (заглушки)
BOOL DPdfImageAddIn::LoadPdfFile(const TCHAR* filename)
{
    // Простая проверка существования файла
    FILE* file = _tfopen(filename, _T("rb"));
    if (file) {
        fclose(file);
        _tcscpy(m_PdfFileName, filename);
        m_bPdfLoaded = TRUE;
        SetLastError(_T("PDF файл загружен (упрощенная версия)"));
        LOG(Log(_T("PDF загружен: %s"), filename));
        return TRUE;
    } else {
        SetLastError(_T("Файл не найден"));
        return FALSE;
    }
}

BOOL DPdfImageAddIn::AddImageToPdf(const TCHAR* imageFile, double x, double y, double width, double height, long pageNum)
{
    if (!m_bPdfLoaded) {
        SetLastError(_T("PDF не загружен"));
        return FALSE;
    }
    
    // Проверяем существование файла изображения
    FILE* file = _tfopen(imageFile, _T("rb"));
    if (!file) {
        SetLastError(_T("Файл изображения не найден"));
        return FALSE;
    }
    fclose(file);
    
    // Добавляем информацию в список (заглушка)
    PdfImageInfo info;
    info.x = x;
    info.y = y;
    info.width = width;
    info.height = height;
    info.pageNum = pageNum;
    _tcscpy(info.imagePath, imageFile);
    
    m_ImageList.push_back(info);
    
    SetLastError(_T("Изображение добавлено (упрощенная версия)"));
    LOG(Log(_T("Изображение добавлено: %s"), imageFile));
    return TRUE;
}

BOOL DPdfImageAddIn::SavePdfFile(const TCHAR* filename)
{
    if (!m_bPdfLoaded) {
        SetLastError(_T("PDF не загружен"));
        return FALSE;
    }
    
    // Создаем пустой файл (заглушка)
    FILE* file = _tfopen(filename, _T("wb"));
    if (file) {
        fclose(file);
        SetLastError(_T("PDF сохранен (упрощенная версия)"));
        LOG(Log(_T("PDF сохранен: %s"), filename));
        return TRUE;
    } else {
        SetLastError(_T("Ошибка создания файла"));
        return FALSE;
    }
}

BOOL DPdfImageAddIn::GetPageDimensions(long pageNum, double* width, double* height)
{
    if (!m_bPdfLoaded) {
        SetLastError(_T("PDF не загружен"));
        return FALSE;
    }
    
    // Возвращаем стандартные размеры A4 (заглушка)
    *width = 595.0;  // A4 ширина в точках
    *height = 842.0; // A4 высота в точках
    
    return TRUE;
}

BOOL DPdfImageAddIn::ClearAllImages()
{
    m_ImageList.clear();
    SetLastError(_T("Изображения очищены"));
    return TRUE;
}

long DPdfImageAddIn::GetTotalPageCount()
{
    if (!m_bPdfLoaded) {
        return 0;
    }
    
    // Возвращаем 1 страницу (заглушка)
    return 1;
}

BOOL DPdfImageAddIn::TestConnection()
{
    SetLastError(_T("Соединение установлено (упрощенная версия)"));
    return TRUE;
}

void DPdfImageAddIn::SetLastError(const TCHAR* error)
{
    _tcscpy(m_LastError, error);
}

void DPdfImageAddIn::ClearLastError()
{
    _tcscpy(m_LastError, _T(""));
}

// Реализация методов интерфейса
HRESULT DPdfImageAddIn::GetPropVal(long lPropNum, VARIANT *pvarPropVal)
{
    USES_CONVERSION;
    ::VariantInit(pvarPropVal);
    
    switch(lPropNum)
    {
        case propIsEnabled:
            V_VT(pvarPropVal) = VT_BOOL;
            V_BOOL(pvarPropVal) = m_boolEnabled ? VARIANT_TRUE : VARIANT_FALSE;
            break;
            
        case propPdfFileName:
            V_VT(pvarPropVal) = VT_BSTR;
            V_BSTR(pvarPropVal) = SysAllocString(T2OLE(m_PdfFileName));
            break;
            
        case propImageFileName:
            V_VT(pvarPropVal) = VT_BSTR;
            V_BSTR(pvarPropVal) = SysAllocString(T2OLE(m_ImageFileName));
            break;
            
        case propImageX:
            V_VT(pvarPropVal) = VT_R8;
            V_R8(pvarPropVal) = m_ImageX;
            break;
            
        case propImageY:
            V_VT(pvarPropVal) = VT_R8;
            V_R8(pvarPropVal) = m_ImageY;
            break;
            
        case propImageWidth:
            V_VT(pvarPropVal) = VT_R8;
            V_R8(pvarPropVal) = m_ImageWidth;
            break;
            
        case propImageHeight:
            V_VT(pvarPropVal) = VT_R8;
            V_R8(pvarPropVal) = m_ImageHeight;
            break;
            
        case propPageNumber:
            V_VT(pvarPropVal) = VT_I4;
            V_I4(pvarPropVal) = m_PageNumber;
            break;
            
        case propLastError:
            V_VT(pvarPropVal) = VT_BSTR;
            V_BSTR(pvarPropVal) = SysAllocString(T2OLE(m_LastError));
            break;
            
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

HRESULT DPdfImageAddIn::SetPropVal(long lPropNum, VARIANT *pvarPropVal)
{
    USES_CONVERSION;
    
    switch(lPropNum)
    {
        case propIsEnabled:
            if (V_VT(pvarPropVal) == VT_BOOL) {
                m_boolEnabled = V_BOOL(pvarPropVal) ? TRUE : FALSE;
            }
            break;
            
        case propImageX:
            if (V_VT(pvarPropVal) == VT_R8) {
                m_ImageX = V_R8(pvarPropVal);
            }
            break;
            
        case propImageY:
            if (V_VT(pvarPropVal) == VT_R8) {
                m_ImageY = V_R8(pvarPropVal);
            }
            break;
            
        case propImageWidth:
            if (V_VT(pvarPropVal) == VT_R8) {
                m_ImageWidth = V_R8(pvarPropVal);
            }
            break;
            
        case propImageHeight:
            if (V_VT(pvarPropVal) == VT_R8) {
                m_ImageHeight = V_R8(pvarPropVal);
            }
            break;
            
        case propPageNumber:
            if (V_VT(pvarPropVal) == VT_I4) {
                m_PageNumber = V_I4(pvarPropVal);
            }
            break;
            
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

HRESULT DPdfImageAddIn::IsPropReadable(long lPropNum, BOOL *pboolPropRead)
{
    *pboolPropRead = TRUE; // Все свойства читаемые
    return S_OK;
}

HRESULT DPdfImageAddIn::IsPropWritable(long lPropNum, BOOL *pboolPropWrite)
{
    switch(lPropNum)
    {
        case propIsEnabled:
        case propImageX:
        case propImageY:
        case propImageWidth:
        case propImageHeight:
        case propPageNumber:
            *pboolPropWrite = TRUE;
            break;
        default:
            *pboolPropWrite = FALSE;
    }
    return S_OK;
}

HRESULT DPdfImageAddIn::GetNParams(long lMethodNum, long *plParams)
{
    switch(lMethodNum)
    {
        case methLoadPdf:
        case methSavePdf:
            *plParams = 1;
            break;
        case methAddImage:
            *plParams = 6;
            break;
        case methGetPageSize:
            *plParams = 1;
            break;
        case methClearImages:
        case methGetPageCount:
        case methTestConnection:
            *plParams = 0;
            break;
        default:
            return S_FALSE;
    }
    return S_OK;
}

HRESULT DPdfImageAddIn::GetParamDefValue(long lMethodNum, long lParamNum, VARIANT *pvarParamDefValue)
{
    ::VariantInit(pvarParamDefValue);
    return S_OK;
}

HRESULT DPdfImageAddIn::HasRetVal(long lMethodNum, BOOL *pboolRetValue)
{
    switch(lMethodNum)
    {
        case methLoadPdf:
        case methAddImage:
        case methSavePdf:
        case methClearImages:
        case methTestConnection:
            *pboolRetValue = TRUE;
            break;
        case methGetPageSize:
        case methGetPageCount:
            *pboolRetValue = TRUE;
            break;
        default:
            return S_FALSE;
    }
    return S_OK;
}

HRESULT DPdfImageAddIn::CallAsProc(long lMethodNum, SAFEARRAY **paParams)
{
    return S_FALSE; // Все методы возвращают значения
}

HRESULT DPdfImageAddIn::CallAsFunc(long lMethodNum, VARIANT *pvarRetValue, SAFEARRAY **paParams)
{
    USES_CONVERSION;
    ::VariantInit(pvarRetValue);
    
    switch(lMethodNum)
    {
        case methLoadPdf:
        {
            VARIANT var = GetNParam(*paParams, 0);
            if (V_VT(&var) != VT_BSTR) {
                ::VariantClear(&var);
                return S_FALSE;
            }
            
            BOOL result = LoadPdfFile(OLE2T(V_BSTR(&var)));
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            
            ::VariantClear(&var);
            break;
        }
        
        case methAddImage:
        {
            VARIANT varImageFile = GetNParam(*paParams, 0);
            VARIANT varX = GetNParam(*paParams, 1);
            VARIANT varY = GetNParam(*paParams, 2);
            VARIANT varWidth = GetNParam(*paParams, 3);
            VARIANT varHeight = GetNParam(*paParams, 4);
            VARIANT varPageNum = GetNParam(*paParams, 5);
            
            if (V_VT(&varImageFile) != VT_BSTR ||
                V_VT(&varX) != VT_R8 ||
                V_VT(&varY) != VT_R8 ||
                V_VT(&varWidth) != VT_R8 ||
                V_VT(&varHeight) != VT_R8 ||
                V_VT(&varPageNum) != VT_I4) {
                
                ::VariantClear(&varImageFile);
                ::VariantClear(&varX);
                ::VariantClear(&varY);
                ::VariantClear(&varWidth);
                ::VariantClear(&varHeight);
                ::VariantClear(&varPageNum);
                return S_FALSE;
            }
            
            BOOL result = AddImageToPdf(OLE2T(V_BSTR(&varImageFile)), V_R8(&varX), V_R8(&varY), 
                                      V_R8(&varWidth), V_R8(&varHeight), V_I4(&varPageNum));
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            
            ::VariantClear(&varImageFile);
            ::VariantClear(&varX);
            ::VariantClear(&varY);
            ::VariantClear(&varWidth);
            ::VariantClear(&varHeight);
            ::VariantClear(&varPageNum);
            break;
        }
        
        case methSavePdf:
        {
            VARIANT var = GetNParam(*paParams, 0);
            if (V_VT(&var) != VT_BSTR) {
                ::VariantClear(&var);
                return S_FALSE;
            }
            
            BOOL result = SavePdfFile(OLE2T(V_BSTR(&var)));
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            
            ::VariantClear(&var);
            break;
        }
        
        case methGetPageSize:
        {
            VARIANT var = GetNParam(*paParams, 0);
            if (V_VT(&var) != VT_I4) {
                ::VariantClear(&var);
                return S_FALSE;
            }
            
            double width, height;
            BOOL result = GetPageDimensions(V_I4(&var), &width, &height);
            
            if (result) {
                _TCHAR sizeStr[100];
                _stprintf(sizeStr, _T("%.2f,%.2f"), width, height);
                V_VT(pvarRetValue) = VT_BSTR;
                V_BSTR(pvarRetValue) = SysAllocString(T2OLE(sizeStr));
            } else {
                V_VT(pvarRetValue) = VT_BSTR;
                V_BSTR(pvarRetValue) = SysAllocString(T2OLE(_T("0,0")));
            }
            
            ::VariantClear(&var);
            break;
        }
        
        case methClearImages:
        {
            BOOL result = ClearAllImages();
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        }
        
        case methGetPageCount:
        {
            long pageCount = GetTotalPageCount();
            V_VT(pvarRetValue) = VT_I4;
            V_I4(pvarRetValue) = pageCount;
            break;
        }
        
        case methTestConnection:
        {
            BOOL result = TestConnection();
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        }
        
        default:
            return S_FALSE;
    }
    
    return S_OK;
} 