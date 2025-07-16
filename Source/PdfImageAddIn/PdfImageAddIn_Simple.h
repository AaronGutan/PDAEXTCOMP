#ifndef _PDFIMAGEADDIN_SIMPLE_H
#define _PDFIMAGEADDIN_SIMPLE_H

#ifndef _ADDIN_ADDIN_H
#include "../Addin/Addin.h"
#endif

// Убираем зависимости от PDFium
// #include "fpdfview.h"
// #include "fpdf_edit.h"
// #include "fpdf_save.h"
// #include "fpdf_doc.h"
// #include "fpdf_annot.h"

// Vector для хранения изображений
#include <vector>

// Структура для хранения информации о добавленных изображениях
struct PdfImageInfo {
    double x, y;
    double width, height;
    long pageNum;
    TCHAR imagePath[_MAX_PATH];
};

/////////////////////////////////////////////////////////////////////////////
// DPdfImageAddIn (упрощенная версия)

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
        methTestConnection,         // TestConnection();
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
    TCHAR m_PdfFileName[_MAX_PATH];
    TCHAR m_ImageFileName[_MAX_PATH];
    TCHAR m_LastError[512];
    
    double m_ImageX;
    double m_ImageY;
    double m_ImageWidth;
    double m_ImageHeight;
    long m_PageNumber;
    
    // Упрощенные переменные (без PDFium)
    BOOL m_bPdfLoaded;
    std::vector<PdfImageInfo> m_ImageList;
    
    // Упрощенные методы
    BOOL LoadPdfFile(const TCHAR* filename);
    BOOL AddImageToPdf(const TCHAR* imageFile, double x, double y, double width, double height, long pageNum);
    BOOL SavePdfFile(const TCHAR* filename);
    
    BOOL GetPageDimensions(long pageNum, double* width, double* height);
    BOOL ClearAllImages();
    long GetTotalPageCount();
    BOOL TestConnection();
    
    void SetLastError(const TCHAR* error);
    void ClearLastError();
};

#endif // _PDFIMAGEADDIN_SIMPLE_H 