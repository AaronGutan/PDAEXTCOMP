#ifndef _PDFIMAGEADDIN_H
#define _PDFIMAGEADDIN_H

#ifndef _ADDIN_ADDIN_H
#include "Addin/Addin.h"
#endif

// PDFium headers
#include "fpdfview.h"
#include "fpdf_edit.h"
#include "fpdf_save.h"
#include "fpdf_doc.h"
#include "fpdf_annot.h"

// Vector для хранения изображений
#include <vector>

// Структура для хранения информации о добавленных изображениях
struct PdfImageInfo {
    double x, y;
    double width, height;
    long pageNum;
    _TCHAR imagePath[_MAX_PATH];
};

/////////////////////////////////////////////////////////////////////////////
// DPdfImageAddIn

class DPdfImageAddIn : 
    public DAddIn
{

public:
    DPdfImageAddIn();
    ~DPdfImageAddIn();

    // Свойства компоненты
    enum 
    {
        propIsEnabled = 0,
        propPdfFileName,
        propImageFileName,
        propImageX,
        propImageY,
        propImageWidth,
        propImageHeight,
        propPageNumber,
        propLastError,
        LastProp      // Всегда последний
    };

    // Методы компоненты
    enum 
    {
        methLoadPdf = 0,            // LoadPdf(filename);
        methAddImage,               // AddImage(imageFile, x, y, width, height, pageNum);
        methSavePdf,                // SavePdf(filename);
        methGetPageSize,            // GetPageSize(pageNum, width, height);
        methClearImages,            // ClearImages();
        methGetPageCount,           // GetPageCount();
        LastMethod      // Всегда последний
    };

    virtual BOOL FillPropsAndMethods(void);

    virtual HRESULT GetPropVal(long lPropNum, VARIANT *pvarPropVal);
    virtual HRESULT SetPropVal(long lPropNum, VARIANT *pvarPropVal);
    virtual HRESULT IsPropReadable(long lPropNum, BOOL *pboolPropRead);
    virtual HRESULT IsPropWritable(long lPropNum, BOOL *pboolPropWrite);

    virtual HRESULT GetNParams(long lMethodNum, long *plParams);
    virtual HRESULT GetParamDefValue(long lMethodNum, long lParamNum, VARIANT *pvarParamDefValue);
    virtual HRESULT HasRetVal(long lMethodNum, BOOL *pboolRetValue);
    virtual HRESULT CallAsProc(long lMethodNum, SAFEARRAY **paParams);
    virtual HRESULT CallAsFunc(long lMethodNum, VARIANT *pvarRetValue, SAFEARRAY **paParams);

private:
    // Внутренние переменные
    _TCHAR m_PdfFileName[_MAX_PATH];
    _TCHAR m_ImageFileName[_MAX_PATH];
    _TCHAR m_LastError[512];
    
    double m_ImageX;
    double m_ImageY;
    double m_ImageWidth;
    double m_ImageHeight;
    long m_PageNumber;
    
    // PDFium переменные
    FPDF_DOCUMENT m_pPdfDocument;
    BOOL m_bPdfLoaded;
    std::vector<PdfImageInfo> m_ImageList;
    
    // Инициализация и завершение PDFium
    BOOL InitializePdfium();
    void FinalizePdfium();
    
    // Внутренние методы
    BOOL LoadPdfFile(const _TCHAR* filename);
    BOOL AddImageToPdf(const _TCHAR* imageFile, double x, double y, double width, double height, long pageNum);
    BOOL SavePdfFile(const _TCHAR* filename);
    BOOL GetPageDimensions(long pageNum, double* width, double* height);
    BOOL ClearAllImages();
    long GetTotalPageCount();
    
    void SetLastError(const _TCHAR* error);
    void ClearLastError();
    
    // Вспомогательные методы для работы с PDFium
    BOOL LoadImageFile(const _TCHAR* imagePath, void** imageData, size_t* imageSize);
    BOOL AddImageToPage(FPDF_PAGE page, const void* imageData, size_t imageSize, 
                       double x, double y, double width, double height);
    
    // Callback для сохранения PDF
    static int WriteCallback(FPDF_FILEWRITE* pThis, const void* pData, unsigned long size);
    
    // Структура для callback сохранения
    struct FileWriteData {
        HANDLE hFile;
        BOOL bSuccess;
    };
};

#endif // _PDFIMAGEADDIN_H 