#include "commhdr.h"
#pragma hdrstop

#include "PdfImageAddIn.h"
#include "PdfDragDropWindow.h"
#include "../Addin/Log.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

extern HINSTANCE g_hinst;

// Глобальные переменные для PDFium
static BOOL g_bPdfiumInitialized = FALSE;
static LONG g_lPdfiumRefCount = 0;

/////////////////////////////////////////////////////////////////////////////
// DPdfImageAddIn

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
    
    m_pPdfDocument = NULL;
    m_bPdfLoaded = FALSE;
    m_boolEnabled = TRUE;
    
    // Инициализация drag-n-drop
    m_pDragDropWindow = NULL;
    m_bDragDropEnabled = TRUE;
    
    // Инициализация PDFium
    InitializePdfium();
}

DPdfImageAddIn::~DPdfImageAddIn()
{
    // Освобождение ресурсов
    if (m_pPdfDocument) {
        FPDF_CloseDocument(m_pPdfDocument);
        m_pPdfDocument = NULL;
    }
    
    // Очистка списка изображений
    m_ImageList.clear();
    
    // Закрытие drag-n-drop окна
    if (m_pDragDropWindow) {
        delete m_pDragDropWindow;
        m_pDragDropWindow = NULL;
    }
    
    // Завершение работы с PDFium
    FinalizePdfium();
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
    m_nMethods = LastMethod;
    
    LOG(Log(_T("PdfImageAddIn: Properties and methods filled")));
    return TRUE;
}

HRESULT DPdfImageAddIn::GetPropVal(long lPropNum, VARIANT *pvarPropVal)
{
    LOG(Log(_T("PdfImageAddIn: Getting prop by num %d"), lPropNum));
    ::VariantInit(pvarPropVal);
    USES_CONVERSION;
    
    switch(lPropNum)
    {
        case propIsEnabled:
            V_VT(pvarPropVal) = VT_I4;
            V_I4(pvarPropVal) = m_boolEnabled ? 1 : 0;
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
    LOG(Log(_T("PdfImageAddIn: Setting prop by num %d"), lPropNum));
    USES_CONVERSION;
    
    switch(lPropNum)
    {
        case propIsEnabled:
            if (V_VT(pvarPropVal) != VT_I4)
                return S_FALSE;
            m_boolEnabled = V_I4(pvarPropVal) ? TRUE : FALSE;
            break;
            
        case propPdfFileName:
            if (V_VT(pvarPropVal) != VT_BSTR)
                return S_FALSE;
            _tcscpy(m_PdfFileName, OLE2T(V_BSTR(pvarPropVal)));
            break;
            
        case propImageFileName:
            if (V_VT(pvarPropVal) != VT_BSTR)
                return S_FALSE;
            _tcscpy(m_ImageFileName, OLE2T(V_BSTR(pvarPropVal)));
            break;
            
        case propImageX:
            if (V_VT(pvarPropVal) != VT_R8)
                return S_FALSE;
            m_ImageX = V_R8(pvarPropVal);
            break;
            
        case propImageY:
            if (V_VT(pvarPropVal) != VT_R8)
                return S_FALSE;
            m_ImageY = V_R8(pvarPropVal);
            break;
            
        case propImageWidth:
            if (V_VT(pvarPropVal) != VT_R8)
                return S_FALSE;
            m_ImageWidth = V_R8(pvarPropVal);
            break;
            
        case propImageHeight:
            if (V_VT(pvarPropVal) != VT_R8)
                return S_FALSE;
            m_ImageHeight = V_R8(pvarPropVal);
            break;
            
        case propPageNumber:
            if (V_VT(pvarPropVal) != VT_I4)
                return S_FALSE;
            m_PageNumber = V_I4(pvarPropVal);
            break;
            
        case propLastError:
            // Только для чтения
            return S_FALSE;
            
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

HRESULT DPdfImageAddIn::IsPropReadable(long lPropNum, BOOL *pboolPropRead)
{
    LOG(Log(_T("PdfImageAddIn: Getting prop readability by num %d"), lPropNum));
    
    switch(lPropNum)
    {
        case propIsEnabled:
        case propPdfFileName:
        case propImageFileName:
        case propImageX:
        case propImageY:
        case propImageWidth:
        case propImageHeight:
        case propPageNumber:
        case propLastError:
            *pboolPropRead = TRUE;
            break;
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

HRESULT DPdfImageAddIn::IsPropWritable(long lPropNum, BOOL *pboolPropWrite)
{
    LOG(Log(_T("PdfImageAddIn: Getting prop writeability by num %d"), lPropNum));
    
    switch(lPropNum)
    {
        case propIsEnabled:
        case propPdfFileName:
        case propImageFileName:
        case propImageX:
        case propImageY:
        case propImageWidth:
        case propImageHeight:
        case propPageNumber:
            *pboolPropWrite = TRUE;
            break;
        case propLastError:
            *pboolPropWrite = FALSE;  // Только для чтения
            break;
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

HRESULT DPdfImageAddIn::GetNParams(long lMethodNum, long *plParams)
{
    *plParams = 0;
    
    switch(lMethodNum)
    {
        case methLoadPdf:
            *plParams = 1;  // filename
            break;
        case methAddImage:
            *plParams = 6;  // imageFile, x, y, width, height, pageNum
            break;
        case methSavePdf:
            *plParams = 1;  // filename
            break;
        case methGetPageSize:
            *plParams = 1;  // pageNum
            break;
        case methClearImages:
            *plParams = 0;
            break;
        case methGetPageCount:
            *plParams = 0;
            break;
        case methOpenDragDropEditor:
            *plParams = 0;
            break;
        case methCloseDragDropEditor:
            *plParams = 0;
            break;
        case methShowDragDropHelp:
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
    
    switch(lMethodNum)
    {
        case methLoadPdf:
        case methAddImage:
        case methSavePdf:
        case methGetPageSize:
        case methClearImages:
        case methGetPageCount:
            // Нет значений по умолчанию
            break;
        default:
            return S_FALSE;
    }
    
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
            *pboolRetValue = TRUE;  // Возвращают результат операции
            break;
        case methGetPageSize:
        case methGetPageCount:
            *pboolRetValue = TRUE;  // Возвращают значения
            break;
        case methOpenDragDropEditor:
        case methCloseDragDropEditor:
            *pboolRetValue = TRUE;  // Возвращают результат операции
            break;
        case methShowDragDropHelp:
            *pboolRetValue = FALSE;  // Не возвращают значения
            break;
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

HRESULT DPdfImageAddIn::CallAsProc(long lMethodNum, SAFEARRAY **paParams)
{
    switch(lMethodNum)
    {
        case methShowDragDropHelp:
            ShowDragDropHelp();
            return S_OK;
        default:
            return S_FALSE;
    }
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
                // Возвращаем строку с размерами
                _TCHAR sizeStr[256];
                _stprintf(sizeStr, _T("%.2f,%.2f"), width, height);
                V_VT(pvarRetValue) = VT_BSTR;
                V_BSTR(pvarRetValue) = SysAllocString(T2OLE(sizeStr));
            } else {
                V_VT(pvarRetValue) = VT_BSTR;
                V_BSTR(pvarRetValue) = SysAllocString(T2OLE(_T("")));
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
        
        case methOpenDragDropEditor:
        {
            BOOL result = OpenDragDropEditor();
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        }
        
        case methCloseDragDropEditor:
        {
            BOOL result = CloseDragDropEditor();
            V_VT(pvarRetValue) = VT_BOOL;
            V_BOOL(pvarRetValue) = result ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        }
        
        default:
            return S_FALSE;
    }
    
    return S_OK;
}

// Инициализация и завершение PDFium
BOOL DPdfImageAddIn::InitializePdfium()
{
    if (InterlockedIncrement(&g_lPdfiumRefCount) == 1) {
        // Первая инициализация
        FPDF_LIBRARY_CONFIG config;
        config.version = 2;
        config.m_pUserFontPaths = NULL;
        config.m_pIsolate = NULL;
        config.m_v8EmbedderSlot = 0;
        
        FPDF_InitLibraryWithConfig(&config);
        g_bPdfiumInitialized = TRUE;
        
        LOG(Log(_T("PDFium инициализирован")));
    }
    return TRUE;
}

void DPdfImageAddIn::FinalizePdfium()
{
    if (InterlockedDecrement(&g_lPdfiumRefCount) == 0) {
        // Последняя финализация
        if (g_bPdfiumInitialized) {
            FPDF_DestroyLibrary();
            g_bPdfiumInitialized = FALSE;
            
            LOG(Log(_T("PDFium завершен")));
        }
    }
}

// Внутренние методы (реализация с PDFium)
BOOL DPdfImageAddIn::LoadPdfFile(const _TCHAR* filename)
{
    ClearLastError();
    
    // Закрытие предыдущего документа
    if (m_pPdfDocument) {
        FPDF_CloseDocument(m_pPdfDocument);
        m_pPdfDocument = NULL;
    }
    
    // Очистка списка изображений
    m_ImageList.clear();
    
    if (!filename || _tcslen(filename) == 0) {
        SetLastError(_T("Не указано имя файла"));
        return FALSE;
    }
    
    // Проверка существования файла
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(filename, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        SetLastError(_T("Файл не найден"));
        return FALSE;
    }
    FindClose(hFind);
    
    // Загрузка PDF документа с помощью PDFium
    m_pPdfDocument = FPDF_LoadDocument(filename, NULL);
    if (!m_pPdfDocument) {
        unsigned long error = FPDF_GetLastError();
        _TCHAR errorMsg[256];
        _stprintf(errorMsg, _T("Ошибка загрузки PDF: код %lu"), error);
        SetLastError(errorMsg);
        return FALSE;
    }
    
    _tcscpy(m_PdfFileName, filename);
    m_bPdfLoaded = TRUE;
    
    LOG(Log(_T("PdfImageAddIn: PDF file loaded: %s"), filename));
    return TRUE;
}

BOOL DPdfImageAddIn::AddImageToPdf(const _TCHAR* imageFile, double x, double y, double width, double height, long pageNum)
{
    ClearLastError();
    
    if (!m_bPdfLoaded || !m_pPdfDocument) {
        SetLastError(_T("PDF файл не загружен"));
        return FALSE;
    }
    
    // Проверка номера страницы
    int pageCount = FPDF_GetPageCount(m_pPdfDocument);
    if (pageNum < 1 || pageNum > pageCount) {
        SetLastError(_T("Неверный номер страницы"));
        return FALSE;
    }
    
    // Проверка существования файла изображения
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(imageFile, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        SetLastError(_T("Файл изображения не найден"));
        return FALSE;
    }
    FindClose(hFind);
    
    // Загрузка изображения из файла
    void* imageData = NULL;
    size_t imageSize = 0;
    if (!LoadImageFile(imageFile, &imageData, &imageSize)) {
        return FALSE;
    }
    
    // Получение страницы
    FPDF_PAGE page = FPDF_LoadPage(m_pPdfDocument, pageNum - 1);
    if (!page) {
        SetLastError(_T("Не удалось загрузить страницу"));
        free(imageData);
        return FALSE;
    }
    
    // Добавление изображения на страницу
    BOOL result = AddImageToPage(page, imageData, imageSize, x, y, width, height);
    
    // Очистка ресурсов
    FPDF_ClosePage(page);
    free(imageData);
    
    if (result) {
        // Сохранение информации об изображении
        PdfImageInfo imageInfo;
        imageInfo.x = x;
        imageInfo.y = y;
        imageInfo.width = width;
        imageInfo.height = height;
        imageInfo.pageNum = pageNum;
        _tcscpy(imageInfo.imagePath, imageFile);
        
        m_ImageList.push_back(imageInfo);
        
        // Обновление текущих свойств
        _tcscpy(m_ImageFileName, imageFile);
        m_ImageX = x;
        m_ImageY = y;
        m_ImageWidth = width;
        m_ImageHeight = height;
        m_PageNumber = pageNum;
        
        LOG(Log(_T("PdfImageAddIn: Image added: %s at (%.2f,%.2f) size (%.2f,%.2f) page %d"), 
                imageFile, x, y, width, height, pageNum));
    }
    
    return result;
}

BOOL DPdfImageAddIn::SavePdfFile(const _TCHAR* filename)
{
    ClearLastError();
    
    if (!m_bPdfLoaded || !m_pPdfDocument) {
        SetLastError(_T("PDF файл не загружен"));
        return FALSE;
    }
    
    if (!filename || _tcslen(filename) == 0) {
        SetLastError(_T("Не указано имя файла для сохранения"));
        return FALSE;
    }
    
    // Проверка возможности создания файла в указанной директории
    _TCHAR directory[_MAX_PATH];
    _tcscpy(directory, filename);
    _TCHAR* lastSlash = _tcsrchr(directory, _T('\\'));
    if (lastSlash) {
        *lastSlash = _T('\0');
        // Проверка существования директории
        DWORD attributes = GetFileAttributes(directory);
        if (attributes == INVALID_FILE_ATTRIBUTES || 
            !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            SetLastError(_T("Указанная директория не существует"));
            return FALSE;
        }
    }
    
    // Создание файла для сохранения
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, 
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        _TCHAR errorMsg[512];
        _stprintf(errorMsg, _T("Не удалось создать файл для сохранения: %s"), filename);
        SetLastError(errorMsg);
        return FALSE;
    }
    
    // Создание структуры для callback сохранения
    FPDF_FILEWRITE fileWrite;
    fileWrite.version = 1;
    fileWrite.WriteBlock = WriteCallback;
    
    FileWriteData writeData;
    writeData.hFile = hFile;
    writeData.bSuccess = TRUE;
    
    // Сохранение PDF документа
    BOOL result = FPDF_SaveAsCopy(m_pPdfDocument, &fileWrite, 0);
    
    // Закрытие файла
    CloseHandle(hFile);
    
    if (!result) {
        SetLastError(_T("Ошибка при сохранении PDF файла (PDFium)"));
        DeleteFile(filename);
        return FALSE;
    }
    
    if (!writeData.bSuccess) {
        SetLastError(_T("Ошибка записи данных в файл"));
        DeleteFile(filename);
        return FALSE;
    }
    
    // Проверка, что файл действительно создан и не пуст
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(filename, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        SetLastError(_T("Созданный файл не найден"));
        return FALSE;
    }
    FindClose(hFind);
    
    if (findData.nFileSizeLow == 0 && findData.nFileSizeHigh == 0) {
        SetLastError(_T("Созданный файл пуст"));
        DeleteFile(filename);
        return FALSE;
    }
    
    LOG(Log(_T("PdfImageAddIn: PDF file saved successfully: %s (size: %d bytes)"), 
            filename, findData.nFileSizeLow));
    return TRUE;
}

BOOL DPdfImageAddIn::GetPageDimensions(long pageNum, double* width, double* height)
{
    ClearLastError();
    
    if (!m_bPdfLoaded || !m_pPdfDocument) {
        SetLastError(_T("PDF файл не загружен"));
        return FALSE;
    }
    
    // Проверка номера страницы
    int pageCount = FPDF_GetPageCount(m_pPdfDocument);
    if (pageNum < 1 || pageNum > pageCount) {
        SetLastError(_T("Неверный номер страницы"));
        return FALSE;
    }
    
    // Получение размеров страницы
    double pageWidth, pageHeight;
    BOOL result = FPDF_GetPageSizeByIndex(m_pPdfDocument, pageNum - 1, &pageWidth, &pageHeight);
    
    if (!result) {
        SetLastError(_T("Не удалось получить размеры страницы"));
        return FALSE;
    }
    
    *width = pageWidth;
    *height = pageHeight;
    
    LOG(Log(_T("PdfImageAddIn: Page %d dimensions: %.2f x %.2f"), pageNum, *width, *height));
    return TRUE;
}

BOOL DPdfImageAddIn::ClearAllImages()
{
    ClearLastError();
    
    if (!m_bPdfLoaded || !m_pPdfDocument) {
        SetLastError(_T("PDF файл не загружен"));
        return FALSE;
    }
    
    // Очистка списка изображений
    // В PDFium сложно удалить уже добавленные изображения без перезагрузки документа
    // Поэтому мы перезагружаем исходный PDF файл
    if (_tcslen(m_PdfFileName) > 0) {
        _TCHAR originalFilename[_MAX_PATH];
        _tcscpy(originalFilename, m_PdfFileName);
        
        // Закрытие текущего документа
        if (m_pPdfDocument) {
            FPDF_CloseDocument(m_pPdfDocument);
            m_pPdfDocument = NULL;
        }
        
        // Перезагрузка исходного документа
        if (LoadPdfFile(originalFilename)) {
            m_ImageList.clear();
            LOG(Log(_T("PdfImageAddIn: PDF reloaded, all images cleared")));
            return TRUE;
        } else {
            SetLastError(_T("Не удалось перезагрузить PDF файл"));
            return FALSE;
        }
    } else {
        // Если нет исходного файла, просто очищаем список
        m_ImageList.clear();
        LOG(Log(_T("PdfImageAddIn: Image list cleared")));
        return TRUE;
    }
}

long DPdfImageAddIn::GetTotalPageCount()
{
    ClearLastError();
    
    if (!m_bPdfLoaded || !m_pPdfDocument) {
        SetLastError(_T("PDF файл не загружен"));
        return 0;
    }
    
    // Получение количества страниц
    int pageCount = FPDF_GetPageCount(m_pPdfDocument);
    
    LOG(Log(_T("PdfImageAddIn: Total page count: %d"), pageCount));
    return pageCount;
}

// Вспомогательные методы для работы с PDFium

BOOL DPdfImageAddIn::LoadImageFile(const _TCHAR* imagePath, void** imageData, size_t* imageSize)
{
    // Открытие файла изображения
    HANDLE hFile = CreateFile(imagePath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        SetLastError(_T("Не удалось открыть файл изображения"));
        return FALSE;
    }
    
    // Получение размера файла
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        SetLastError(_T("Не удалось получить размер файла изображения"));
        return FALSE;
    }
    
    // Проверка на пустой файл
    if (fileSize == 0) {
        CloseHandle(hFile);
        SetLastError(_T("Файл изображения пуст"));
        return FALSE;
    }
    
    // Выделение памяти для изображения
    *imageData = malloc(fileSize);
    if (!*imageData) {
        CloseHandle(hFile);
        SetLastError(_T("Не удалось выделить память для изображения"));
        return FALSE;
    }
    
    // Чтение файла
    DWORD bytesRead;
    BOOL result = ReadFile(hFile, *imageData, fileSize, &bytesRead, NULL);
    CloseHandle(hFile);
    
    if (!result || bytesRead != fileSize) {
        free(*imageData);
        *imageData = NULL;
        SetLastError(_T("Не удалось прочитать файл изображения"));
        return FALSE;
    }
    
    // Проверка формата изображения по магическим байтам
    unsigned char* data = (unsigned char*)*imageData;
    BOOL isValidFormat = FALSE;
    
    // Проверка JPEG (FF D8 FF)
    if (fileSize >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
        isValidFormat = TRUE;
    }
    // Проверка PNG (89 50 4E 47)
    else if (fileSize >= 8 && data[0] == 0x89 && data[1] == 0x50 && 
             data[2] == 0x4E && data[3] == 0x47) {
        isValidFormat = TRUE;
    }
    // Проверка BMP (42 4D)
    else if (fileSize >= 2 && data[0] == 0x42 && data[1] == 0x4D) {
        isValidFormat = TRUE;
    }
    // Проверка GIF (47 49 46 38)
    else if (fileSize >= 6 && data[0] == 0x47 && data[1] == 0x49 && 
             data[2] == 0x46 && data[3] == 0x38) {
        isValidFormat = TRUE;
    }
    
    if (!isValidFormat) {
        free(*imageData);
        *imageData = NULL;
        SetLastError(_T("Неподдерживаемый формат изображения (поддерживаются JPEG, PNG, BMP, GIF)"));
        return FALSE;
    }
    
    *imageSize = fileSize;
    
    LOG(Log(_T("PdfImageAddIn: Image loaded: %s, size: %d bytes"), imagePath, fileSize));
    return TRUE;
}

BOOL DPdfImageAddIn::AddImageToPage(FPDF_PAGE page, const void* imageData, size_t imageSize, 
                                   double x, double y, double width, double height)
{
    // Создание объекта изображения
    FPDF_PAGEOBJECT imageObj = FPDFPageObj_NewImageObj(m_pPdfDocument);
    if (!imageObj) {
        SetLastError(_T("Не удалось создать объект изображения"));
        return FALSE;
    }
    
    // Определение формата изображения
    unsigned char* data = (unsigned char*)imageData;
    BOOL imageLoaded = FALSE;
    
    // Проверка и загрузка JPEG
    if (imageSize >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
        // Создание файлового доступа для JPEG
        FPDF_FILEACCESS fileAccess;
        fileAccess.m_FileLen = imageSize;
        fileAccess.m_GetBlock = NULL;
        fileAccess.m_Param = (void*)imageData;
        
        if (FPDFImageObj_LoadJpegFile(NULL, 0, imageObj, &fileAccess)) {
            imageLoaded = TRUE;
        }
    }
    // Проверка и загрузка PNG
    else if (imageSize >= 8 && data[0] == 0x89 && data[1] == 0x50 && 
             data[2] == 0x4E && data[3] == 0x47) {
        // Для PNG создаем временный документ из данных
        FPDF_DOCUMENT tempDoc = FPDF_LoadMemDocument(imageData, imageSize, nullptr);
        if (tempDoc) {
            // Попытка установить bitmap для PNG
            if (FPDFImageObj_SetBitmap(NULL, 0, imageObj, NULL)) {
                imageLoaded = TRUE;
            }
            FPDF_CloseDocument(tempDoc);
        }
    }
    // Для других форматов пытаемся загрузить как bitmap
    else {
        // Попытка загрузки как bitmap через временный документ
        FPDF_DOCUMENT tempDoc = FPDF_LoadMemDocument(imageData, imageSize, nullptr);
        if (tempDoc) {
            if (FPDFImageObj_SetBitmap(NULL, 0, imageObj, NULL)) {
                imageLoaded = TRUE;
            }
            FPDF_CloseDocument(tempDoc);
        }
    }
    
    if (!imageLoaded) {
        FPDFPageObj_Destroy(imageObj);
        SetLastError(_T("Не удалось загрузить изображение. Проверьте формат файла."));
        return FALSE;
    }
    
    // Получение размеров страницы для корректного позиционирования
    double pageWidth, pageHeight;
    pageWidth = FPDF_GetPageWidth(page);
    pageHeight = FPDF_GetPageHeight(page);
    
    if (pageWidth <= 0 || pageHeight <= 0) {
        FPDFPageObj_Destroy(imageObj);
        SetLastError(_T("Не удалось получить размеры страницы"));
        return FALSE;
    }
    
    // Коррекция Y-координаты для системы координат PDF (снизу вверх)
    double correctedY = pageHeight - y - height;
    
    // Установка матрицы трансформации для позиционирования и масштабирования
    FS_MATRIX matrix;
    matrix.a = width;       // Масштаб по X
    matrix.b = 0;           // Наклон по Y
    matrix.c = 0;           // Наклон по X
    matrix.d = height;      // Масштаб по Y
    matrix.e = x;           // Смещение по X
    matrix.f = correctedY;  // Смещение по Y (с коррекцией)
    
    // Применение трансформации к изображению
    FPDFPageObj_Transform(imageObj, matrix.a, matrix.b, matrix.c, matrix.d, matrix.e, matrix.f);
    
    // Добавление объекта на страницу
    FPDFPage_InsertObject(page, imageObj);
    
    // Генерация содержимого страницы
    if (!FPDFPage_GenerateContent(page)) {
        SetLastError(_T("Не удалось сгенерировать содержимое страницы"));
        return FALSE;
    }
    
    LOG(Log(_T("PdfImageAddIn: Image added to page at (%.2f,%.2f) size (%.2f,%.2f)"), 
            x, y, width, height));
    return TRUE;
}

// Callback для сохранения PDF
int DPdfImageAddIn::WriteCallback(FPDF_FILEWRITE* pThis, const void* pData, unsigned long size)
{
    // Простая заглушка для callback
    // В реальном коде должна быть более сложная логика с сохранением в файл
    if (!pData || size == 0) {
        return 1; // Пустые данные - не ошибка
    }
    
    // Заглушка - всегда возвращаем успех
    return 1;
}

void DPdfImageAddIn::SetLastError(const _TCHAR* error)
{
    _tcscpy(m_LastError, error);
    LOG(Log(_T("PdfImageAddIn: Error: %s"), error));
}

void DPdfImageAddIn::ClearLastError()
{
    _tcscpy(m_LastError, _T(""));
}

// Методы для работы с drag-n-drop
BOOL DPdfImageAddIn::OpenDragDropEditor()
{
    ClearLastError();
    
    // Проверяем, что окно еще не открыто
    if (m_pDragDropWindow != NULL) {
        SetLastError(_T("Drag & Drop редактор уже открыт"));
        return FALSE;
    }
    
    // Создаем новое окно
    m_pDragDropWindow = new PdfDragDropWindow(this);
    if (!m_pDragDropWindow) {
        SetLastError(_T("Не удалось создать объект drag-n-drop окна"));
        return FALSE;
    }
    
    // Создаем окно
    if (!m_pDragDropWindow->Create()) {
        delete m_pDragDropWindow;
        m_pDragDropWindow = NULL;
        SetLastError(_T("Не удалось создать drag-n-drop окно"));
        return FALSE;
    }
    
    // Если PDF уже загружен, загружаем его в окно
    if (m_bPdfLoaded && _tcslen(m_PdfFileName) > 0) {
        m_pDragDropWindow->LoadPdf(m_PdfFileName);
    }
    
    // Показываем окно
    m_pDragDropWindow->Show(TRUE);
    
    LOG(Log(_T("PdfImageAddIn: Drag & Drop editor opened")));
    return TRUE;
}

BOOL DPdfImageAddIn::CloseDragDropEditor()
{
    ClearLastError();
    
    if (m_pDragDropWindow == NULL) {
        SetLastError(_T("Drag & Drop редактор не открыт"));
        return FALSE;
    }
    
    // Закрываем окно
    m_pDragDropWindow->Close();
    
    // Удаляем объект
    delete m_pDragDropWindow;
    m_pDragDropWindow = NULL;
    
    LOG(Log(_T("PdfImageAddIn: Drag & Drop editor closed")));
    return TRUE;
}

void DPdfImageAddIn::ShowDragDropHelp()
{
    // Показываем справку
    HWND hParent = NULL;
    if (m_pDragDropWindow) {
        hParent = m_pDragDropWindow->GetHWnd();
    }
    
    ::ShowDragDropHelp(hParent);
}

BOOL DPdfImageAddIn::IsDragDropEditorOpen() const
{
    return (m_pDragDropWindow != NULL && m_pDragDropWindow->IsWindowValid());
} 