#ifndef _PDFDRAGDROPWINDOW_H
#define _PDFDRAGDROPWINDOW_H

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <commdlg.h>

// Forward declaration
class DPdfImageAddIn;

// Структура для хранения информации о перетаскивании
struct DragDropInfo {
    POINT dropPoint;        // Координаты сброса
    TCHAR imagePath[MAX_PATH];  // Путь к изображению
    BOOL isValid;           // Валидность операции
    int pageNumber;         // Номер страницы
    double pageX, pageY;    // Координаты на странице PDF
};

// Класс окна для drag-n-drop операций
class PdfDragDropWindow {
public:
    PdfDragDropWindow(DPdfImageAddIn* pParent);
    ~PdfDragDropWindow();
    
    // Основные методы
    BOOL Create(HWND hParent = NULL);
    BOOL Show(BOOL bShow = TRUE);
    BOOL LoadPdf(const TCHAR* pdfPath);
    BOOL IsWindowValid() const { return m_hWnd != NULL; }
    
    // Методы для работы с изображениями
    BOOL AddImageAt(const TCHAR* imagePath, double x, double y, int pageNum);
    BOOL SetImageSize(double width, double height);
    
    // Настройки
    BOOL SetZoomLevel(double zoom);
    BOOL SetCurrentPage(int pageNum);
    BOOL SetDragDropEnabled(BOOL bEnabled);
    
    // Получение информации
    int GetCurrentPage() const { return m_currentPage; }
    double GetZoomLevel() const { return m_zoomLevel; }
    BOOL GetPageDimensions(int pageNum, double* width, double* height);
    
    // Управление окном
    HWND GetHWnd() const { return m_hWnd; }
    BOOL Close();
    
protected:
    // Обработчики событий Windows
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    
    // Обработчики drag-n-drop
    LRESULT OnDropFiles(WPARAM wParam);
    LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    LRESULT OnDragLeave();
    
    // Обработчики рисования
    LRESULT OnPaint();
    LRESULT OnEraseBkgnd(WPARAM wParam);
    
    // Обработчики мыши
    LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
    
    // Обработчики клавиатуры
    LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
    
    // Обработчики управления
    LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
    LRESULT OnSize(WPARAM wParam, LPARAM lParam);
    
private:
    // Основные переменные
    HWND m_hWnd;
    HWND m_hParent;
    DPdfImageAddIn* m_pParent;
    
    // Состояние PDF
    TCHAR m_pdfPath[MAX_PATH];
    BOOL m_bPdfLoaded;
    int m_currentPage;
    int m_totalPages;
    double m_pageWidth, m_pageHeight;
    
    // Параметры отображения
    double m_zoomLevel;
    int m_scrollX, m_scrollY;
    RECT m_clientRect;
    
    // Drag-n-drop состояние
    BOOL m_bDragDropEnabled;
    BOOL m_bDragActive;
    POINT m_lastMousePos;
    DragDropInfo m_dragInfo;
    
    // Настройки изображения
    double m_defaultImageWidth;
    double m_defaultImageHeight;
    
    // Визуальные элементы
    HBRUSH m_hBrushBg;
    HBRUSH m_hBrushDrop;
    HPEN m_hPenBorder;
    HFONT m_hFont;
    
    // Вспомогательные методы
    BOOL RegisterWindowClass();
    BOOL CreateControls();
    BOOL InitializeDragDrop();
    void CleanupResources();
    
    // Методы отрисовки
    void DrawPdfPage(HDC hdc);
    void DrawDragIndicator(HDC hdc, const POINT& pos);
    void DrawPageBorder(HDC hdc);
    void DrawStatusText(HDC hdc);
    void DrawGrid(HDC hdc);
    
    // Utility методы
    BOOL IsValidImageFile(const TCHAR* filePath);
    BOOL ConvertScreenToPage(const POINT& screenPos, double* pageX, double* pageY);
    BOOL ConvertPageToScreen(double pageX, double pageY, POINT* screenPos);
    void UpdateScrollBars();
    void InvalidatePageArea();
    
    // Вспомогательные константы
    static const TCHAR* WINDOW_CLASS_NAME;
    static const int MIN_ZOOM = 25;
    static const int MAX_ZOOM = 400;
    static const int DEFAULT_ZOOM = 100;
    
    // Идентификаторы команд
    enum {
        ID_ZOOM_IN = 1001,
        ID_ZOOM_OUT = 1002,
        ID_ZOOM_FIT = 1003,
        ID_PREV_PAGE = 1004,
        ID_NEXT_PAGE = 1005,
        ID_CLOSE = 1006
    };
};

// Вспомогательные функции
BOOL IsImageFile(const TCHAR* fileName);
BOOL GetImageDimensions(const TCHAR* imagePath, int* width, int* height);
void ShowDragDropHelp(HWND hParent);

#endif // _PDFDRAGDROPWINDOW_H 