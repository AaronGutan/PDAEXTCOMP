#include "PdfDragDropWindow.h"
#include "PdfImageAddIn.h"
#include <tchar.h>
#include <shlwapi.h>
#include <gdiplus.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")

// Константы класса
const TCHAR* PdfDragDropWindow::WINDOW_CLASS_NAME = _T("PdfDragDropWindow");

// Конструктор
PdfDragDropWindow::PdfDragDropWindow(DPdfImageAddIn* pParent) :
    m_hWnd(NULL),
    m_hParent(NULL),
    m_pParent(pParent),
    m_bPdfLoaded(FALSE),
    m_currentPage(1),
    m_totalPages(0),
    m_pageWidth(0),
    m_pageHeight(0),
    m_zoomLevel(DEFAULT_ZOOM),
    m_scrollX(0),
    m_scrollY(0),
    m_bDragDropEnabled(TRUE),
    m_bDragActive(FALSE),
    m_defaultImageWidth(100),
    m_defaultImageHeight(100),
    m_hBrushBg(NULL),
    m_hBrushDrop(NULL),
    m_hPenBorder(NULL),
    m_hFont(NULL)
{
    ZeroMemory(&m_clientRect, sizeof(m_clientRect));
    ZeroMemory(&m_lastMousePos, sizeof(m_lastMousePos));
    ZeroMemory(&m_dragInfo, sizeof(m_dragInfo));
    ZeroMemory(m_pdfPath, sizeof(m_pdfPath));
}

// Деструктор
PdfDragDropWindow::~PdfDragDropWindow()
{
    CleanupResources();
    if (m_hWnd && IsWindow(m_hWnd)) {
        DestroyWindow(m_hWnd);
    }
}

// Создание окна
BOOL PdfDragDropWindow::Create(HWND hParent)
{
    m_hParent = hParent;
    
    // Регистрация класса окна
    if (!RegisterWindowClass()) {
        return FALSE;
    }
    
    // Создание окна
    m_hWnd = CreateWindowEx(
        WS_EX_ACCEPTFILES | WS_EX_CLIENTEDGE,
        WINDOW_CLASS_NAME,
        _T("PDF Drag & Drop - Редактор изображений"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        hParent,
        NULL,
        GetModuleHandle(NULL),
        this
    );
    
    if (!m_hWnd) {
        return FALSE;
    }
    
    // Инициализация drag-n-drop
    if (!InitializeDragDrop()) {
        return FALSE;
    }
    
    // Создание элементов управления
    CreateControls();
    
    return TRUE;
}

// Регистрация класса окна
BOOL PdfDragDropWindow::RegisterWindowClass()
{
    static BOOL bRegistered = FALSE;
    if (bRegistered) return TRUE;
    
    WNDCLASSEX wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(PdfDragDropWindow*);
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    if (!RegisterClassEx(&wcex)) {
        return FALSE;
    }
    
    bRegistered = TRUE;
    return TRUE;
}

// Инициализация drag-n-drop
BOOL PdfDragDropWindow::InitializeDragDrop()
{
    // Принимаем файлы
    DragAcceptFiles(m_hWnd, TRUE);
    
    // Создаем ресурсы для визуализации
    m_hBrushBg = CreateSolidBrush(RGB(240, 240, 240));
    m_hBrushDrop = CreateSolidBrush(RGB(200, 255, 200));
    m_hPenBorder = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    
    // Создаем шрифт
    m_hFont = CreateFont(
        14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        _T("Arial")
    );
    
    return TRUE;
}

// Создание элементов управления
BOOL PdfDragDropWindow::CreateControls()
{
    // Создаем toolbar с кнопками управления
    HWND hToolbar = CreateWindowEx(
        0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT,
        0, 0, 0, 0,
        m_hWnd, (HMENU)100, GetModuleHandle(NULL), NULL
    );
    
    if (hToolbar) {
        // Инициализация common controls
        InitCommonControls();
        
        // Добавляем кнопки
        TBBUTTON buttons[] = {
            {0, ID_ZOOM_IN, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)_T("Увеличить")},
            {1, ID_ZOOM_OUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)_T("Уменьшить")},
            {2, ID_ZOOM_FIT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)_T("По размеру")},
            {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
            {3, ID_PREV_PAGE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)_T("Предыдущая")},
            {4, ID_NEXT_PAGE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)_T("Следующая")},
            {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
            {5, ID_CLOSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)_T("Закрыть")}
        };
        
        SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
        SendMessage(hToolbar, TB_ADDBUTTONS, sizeof(buttons)/sizeof(TBBUTTON), (LPARAM)buttons);
        SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
    }
    
    return TRUE;
}

// Главная оконная процедура
LRESULT CALLBACK PdfDragDropWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PdfDragDropWindow* pThis = NULL;
    
    if (message == WM_CREATE) {
        CREATESTRUCT* pCreateStruct = (CREATESTRUCT*)lParam;
        pThis = (PdfDragDropWindow*)pCreateStruct->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (PdfDragDropWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }
    
    if (pThis) {
        return pThis->HandleMessage(message, wParam, lParam);
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Обработчик сообщений
LRESULT PdfDragDropWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE:
        return 0;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    case WM_PAINT:
        return OnPaint();
        
    case WM_ERASEBKGND:
        return OnEraseBkgnd(wParam);
        
    case WM_DROPFILES:
        return OnDropFiles(wParam);
        
            case WM_DROPFILES:
            return OnDropFiles(wParam);
        
        case WM_MOUSEMOVE:
            return OnMouseMove(wParam, lParam);
        
    case WM_MOUSEMOVE:
        return OnMouseMove(wParam, lParam);
        
    case WM_LBUTTONDOWN:
        return OnLButtonDown(wParam, lParam);
        
    case WM_LBUTTONUP:
        return OnLButtonUp(wParam, lParam);
        
    case WM_MOUSEWHEEL:
        return OnMouseWheel(wParam, lParam);
        
    case WM_KEYDOWN:
        return OnKeyDown(wParam, lParam);
        
    case WM_COMMAND:
        return OnCommand(wParam, lParam);
        
    case WM_SIZE:
        return OnSize(wParam, lParam);
        
    default:
        return DefWindowProc(m_hWnd, message, wParam, lParam);
    }
}

// Обработка сброса файлов
LRESULT PdfDragDropWindow::OnDropFiles(WPARAM wParam)
{
    HDROP hDrop = (HDROP)wParam;
    TCHAR szFile[MAX_PATH];
    POINT dropPoint;
    
    // Получаем количество файлов
    UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    
    // Получаем координаты сброса
    DragQueryPoint(hDrop, &dropPoint);
    
    // Обрабатываем каждый файл
    for (UINT i = 0; i < fileCount; i++) {
        if (DragQueryFile(hDrop, i, szFile, MAX_PATH) > 0) {
            // Проверяем, является ли файл изображением
            if (IsValidImageFile(szFile)) {
                // Конвертируем экранные координаты в координаты страницы
                double pageX, pageY;
                if (ConvertScreenToPage(dropPoint, &pageX, &pageY)) {
                    // Добавляем изображение
                    if (AddImageAt(szFile, pageX, pageY, m_currentPage)) {
                        // Обновляем отображение
                        InvalidatePageArea();
                    }
                }
            }
        }
    }
    
    // Завершаем операцию drag-n-drop
    DragFinish(hDrop);
    m_bDragActive = FALSE;
    
    return 0;
}

// Обработка входа в drag-n-drop
LRESULT PdfDragDropWindow::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    if (m_bDragDropEnabled && m_bPdfLoaded) {
        m_bDragActive = TRUE;
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    return 0;
}

// Обработка движения во время drag-n-drop
LRESULT PdfDragDropWindow::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    if (m_bDragActive) {
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        m_lastMousePos = pt;
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    return 0;
}

// Обработка выхода из drag-n-drop
LRESULT PdfDragDropWindow::OnDragLeave()
{
    m_bDragActive = FALSE;
    InvalidateRect(m_hWnd, NULL, FALSE);
    return 0;
}

// Обработка рисования
LRESULT PdfDragDropWindow::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    
    // Получаем размеры клиентской области
    GetClientRect(m_hWnd, &m_clientRect);
    
    // Рисуем фон
    FillRect(hdc, &m_clientRect, m_hBrushBg);
    
    if (m_bPdfLoaded) {
        // Рисуем страницу PDF
        DrawPdfPage(hdc);
        
        // Рисуем границы страницы
        DrawPageBorder(hdc);
        
        // Рисуем индикатор drag-n-drop
        if (m_bDragActive) {
            DrawDragIndicator(hdc, m_lastMousePos);
        }
    } else {
        // Рисуем инструкции
        DrawStatusText(hdc);
    }
    
    EndPaint(m_hWnd, &ps);
    return 0;
}

// Отрисовка страницы PDF
void PdfDragDropWindow::DrawPdfPage(HDC hdc)
{
    if (!m_bPdfLoaded) return;
    
    // Вычисляем размеры и позицию страницы на экране
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    int pageX = (m_clientRect.right - pageScreenWidth) / 2 - m_scrollX;
    int pageY = (m_clientRect.bottom - pageScreenHeight) / 2 - m_scrollY;
    
    // Рисуем белый фон страницы
    RECT pageRect = {pageX, pageY, pageX + pageScreenWidth, pageY + pageScreenHeight};
    FillRect(hdc, &pageRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // Рисуем сетку для удобства позиционирования
    DrawGrid(hdc);
    
    // Здесь можно добавить отрисовку содержимого PDF через PDFium
    // Пока рисуем placeholder
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // Рисуем крестик для обозначения страницы
    MoveToEx(hdc, pageX, pageY, NULL);
    LineTo(hdc, pageX + pageScreenWidth, pageY + pageScreenHeight);
    MoveToEx(hdc, pageX + pageScreenWidth, pageY, NULL);
    LineTo(hdc, pageX, pageY + pageScreenHeight);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// Отрисовка индикатора drag-n-drop
void PdfDragDropWindow::DrawDragIndicator(HDC hdc, const POINT& pos)
{
    if (!m_bDragActive) return;
    
    // Рисуем окружность в месте предполагаемого размещения
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, m_hBrushDrop);
    HPEN hOldPen = (HPEN)SelectObject(hdc, m_hPenBorder);
    
    int radius = 20;
    Ellipse(hdc, pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius);
    
    // Рисуем крестик в центре
    MoveToEx(hdc, pos.x - 10, pos.y, NULL);
    LineTo(hdc, pos.x + 10, pos.y);
    MoveToEx(hdc, pos.x, pos.y - 10, NULL);
    LineTo(hdc, pos.x, pos.y + 10);
    
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
}

// Отрисовка границ страницы
void PdfDragDropWindow::DrawPageBorder(HDC hdc)
{
    if (!m_bPdfLoaded) return;
    
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    int pageX = (m_clientRect.right - pageScreenWidth) / 2 - m_scrollX;
    int pageY = (m_clientRect.bottom - pageScreenHeight) / 2 - m_scrollY;
    
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // Рисуем рамку страницы
    Rectangle(hdc, pageX, pageY, pageX + pageScreenWidth, pageY + pageScreenHeight);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// Отрисовка текста статуса
void PdfDragDropWindow::DrawStatusText(HDC hdc)
{
    HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);
    SetTextColor(hdc, RGB(100, 100, 100));
    SetBkMode(hdc, TRANSPARENT);
    
    const TCHAR* text = _T("Загрузите PDF файл для начала работы\n\nИли перетащите изображения сюда для добавления в PDF");
    
    RECT textRect = m_clientRect;
    DrawText(hdc, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    
    SelectObject(hdc, hOldFont);
}

// Отрисовка сетки
void PdfDragDropWindow::DrawGrid(HDC hdc)
{
    if (!m_bPdfLoaded || m_zoomLevel < 75) return;
    
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    int pageX = (m_clientRect.right - pageScreenWidth) / 2 - m_scrollX;
    int pageY = (m_clientRect.bottom - pageScreenHeight) / 2 - m_scrollY;
    
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(230, 230, 230));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // Рисуем вертикальные линии сетки
    int gridStep = 50; // 50 пикселей
    for (int x = pageX; x < pageX + pageScreenWidth; x += gridStep) {
        MoveToEx(hdc, x, pageY, NULL);
        LineTo(hdc, x, pageY + pageScreenHeight);
    }
    
    // Рисуем горизонтальные линии сетки
    for (int y = pageY; y < pageY + pageScreenHeight; y += gridStep) {
        MoveToEx(hdc, pageX, y, NULL);
        LineTo(hdc, pageX + pageScreenWidth, y);
    }
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// Загрузка PDF файла
BOOL PdfDragDropWindow::LoadPdf(const TCHAR* pdfPath)
{
    if (!pdfPath || !m_pParent) return FALSE;
    
    // Загружаем PDF через основную компоненту
    if (!m_pParent->LoadPdfFile(pdfPath)) {
        return FALSE;
    }
    
    // Сохраняем путь
    _tcscpy_s(m_pdfPath, MAX_PATH, pdfPath);
    
    // Получаем информацию о PDF
    m_totalPages = m_pParent->GetTotalPageCount();
    m_currentPage = 1;
    
    // Получаем размеры первой страницы
    m_pParent->GetPageDimensions(1, &m_pageWidth, &m_pageHeight);
    
    m_bPdfLoaded = TRUE;
    
    // Обновляем отображение
    InvalidateRect(m_hWnd, NULL, TRUE);
    
    return TRUE;
}

// Добавление изображения в указанную позицию
BOOL PdfDragDropWindow::AddImageAt(const TCHAR* imagePath, double x, double y, int pageNum)
{
    if (!m_pParent || !m_bPdfLoaded) return FALSE;
    
    // Используем размеры по умолчанию
    return m_pParent->AddImageToPdf(imagePath, x, y, m_defaultImageWidth, m_defaultImageHeight, pageNum);
}

// Проверка валидности файла изображения
BOOL PdfDragDropWindow::IsValidImageFile(const TCHAR* filePath)
{
    if (!filePath) return FALSE;
    
    const TCHAR* ext = PathFindExtension(filePath);
    if (!ext) return FALSE;
    
    // Поддерживаемые форматы
    const TCHAR* supportedExts[] = {
        _T(".jpg"), _T(".jpeg"), _T(".png"), _T(".bmp"), _T(".gif"), _T(".tiff")
    };
    
    for (int i = 0; i < sizeof(supportedExts) / sizeof(supportedExts[0]); i++) {
        if (_tcsicmp(ext, supportedExts[i]) == 0) {
            return TRUE;
        }
    }
    
    return FALSE;
}

// Конвертация экранных координат в координаты страницы
BOOL PdfDragDropWindow::ConvertScreenToPage(const POINT& screenPos, double* pageX, double* pageY)
{
    if (!m_bPdfLoaded || !pageX || !pageY) return FALSE;
    
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    int pageScreenX = (m_clientRect.right - pageScreenWidth) / 2 - m_scrollX;
    int pageScreenY = (m_clientRect.bottom - pageScreenHeight) / 2 - m_scrollY;
    
    // Проверяем, что точка находится в пределах страницы
    if (screenPos.x < pageScreenX || screenPos.x > pageScreenX + pageScreenWidth ||
        screenPos.y < pageScreenY || screenPos.y > pageScreenY + pageScreenHeight) {
        return FALSE;
    }
    
    // Конвертируем координаты
    *pageX = (screenPos.x - pageScreenX) * 100.0 / m_zoomLevel;
    *pageY = m_pageHeight - (screenPos.y - pageScreenY) * 100.0 / m_zoomLevel; // Инвертируем Y
    
    return TRUE;
}

// Очистка ресурсов
void PdfDragDropWindow::CleanupResources()
{
    if (m_hBrushBg) {
        DeleteObject(m_hBrushBg);
        m_hBrushBg = NULL;
    }
    
    if (m_hBrushDrop) {
        DeleteObject(m_hBrushDrop);
        m_hBrushDrop = NULL;
    }
    
    if (m_hPenBorder) {
        DeleteObject(m_hPenBorder);
        m_hPenBorder = NULL;
    }
    
    if (m_hFont) {
        DeleteObject(m_hFont);
        m_hFont = NULL;
    }
} 

// Обработка стирания фона
LRESULT PdfDragDropWindow::OnEraseBkgnd(WPARAM wParam)
{
    // Не стираем фон, чтобы избежать мерцания
    return 1;
}

// Обработка движения мыши
LRESULT PdfDragDropWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    POINT pt = {LOWORD(lParam), HIWORD(lParam)};
    m_lastMousePos = pt;
    
    // Обновляем статус если нужно
    if (m_bDragActive) {
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    
    return 0;
}

// Обработка нажатия левой кнопки мыши
LRESULT PdfDragDropWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    POINT pt = {LOWORD(lParam), HIWORD(lParam)};
    
    // Устанавливаем фокус на окно
    SetFocus(m_hWnd);
    
    // Можно добавить дополнительную логику для выбора изображений
    return 0;
}

// Обработка отпускания левой кнопки мыши
LRESULT PdfDragDropWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    // Обработка завершения выбора или перемещения
    return 0;
}

// Обработка колесика мыши (масштабирование)
LRESULT PdfDragDropWindow::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
    if (!m_bPdfLoaded) return 0;
    
    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    UINT keys = GET_KEYSTATE_WPARAM(wParam);
    
    // Масштабирование при нажатом Ctrl
    if (keys & MK_CONTROL) {
        double newZoom = m_zoomLevel;
        if (zDelta > 0) {
            newZoom *= 1.1; // Увеличиваем на 10%
        } else {
            newZoom /= 1.1; // Уменьшаем на 10%
        }
        
        // Ограничиваем масштаб
        if (newZoom < MIN_ZOOM) newZoom = MIN_ZOOM;
        if (newZoom > MAX_ZOOM) newZoom = MAX_ZOOM;
        
        if (newZoom != m_zoomLevel) {
            m_zoomLevel = newZoom;
            InvalidateRect(m_hWnd, NULL, TRUE);
        }
    }
    
    return 0;
}

// Обработка нажатия клавиш
LRESULT PdfDragDropWindow::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
    switch (wParam) {
    case VK_ESCAPE:
        // Отменяем drag-n-drop операцию
        m_bDragActive = FALSE;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
        
    case VK_PRIOR: // Page Up
        if (m_currentPage > 1) {
            SetCurrentPage(m_currentPage - 1);
        }
        break;
        
    case VK_NEXT: // Page Down
        if (m_currentPage < m_totalPages) {
            SetCurrentPage(m_currentPage + 1);
        }
        break;
        
    case VK_HOME:
        SetCurrentPage(1);
        break;
        
    case VK_END:
        SetCurrentPage(m_totalPages);
        break;
        
    case 0xBB: // VK_PLUS
    case VK_ADD:
        // Увеличить масштаб
        SetZoomLevel(m_zoomLevel * 1.1);
        break;
        
    case 0xBD: // VK_MINUS
    case VK_SUBTRACT:
        // Уменьшить масштаб
        SetZoomLevel(m_zoomLevel / 1.1);
        break;
    }
    
    return 0;
}

// Обработка команд (кнопки toolbar)
LRESULT PdfDragDropWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    WORD commandId = LOWORD(wParam);
    
    switch (commandId) {
    case ID_ZOOM_IN:
        SetZoomLevel(m_zoomLevel * 1.25);
        break;
        
    case ID_ZOOM_OUT:
        SetZoomLevel(m_zoomLevel / 1.25);
        break;
        
    case ID_ZOOM_FIT:
        // Подогнать по размеру окна
        if (m_bPdfLoaded) {
            double scaleX = (double)m_clientRect.right / m_pageWidth;
            double scaleY = (double)m_clientRect.bottom / m_pageHeight;
            double scale = min(scaleX, scaleY) * 0.9; // 90% от размера окна
            SetZoomLevel(scale * 100);
        }
        break;
        
    case ID_PREV_PAGE:
        if (m_currentPage > 1) {
            SetCurrentPage(m_currentPage - 1);
        }
        break;
        
    case ID_NEXT_PAGE:
        if (m_currentPage < m_totalPages) {
            SetCurrentPage(m_currentPage + 1);
        }
        break;
        
    case ID_CLOSE:
        Close();
        break;
    }
    
    return 0;
}

// Обработка изменения размера окна
LRESULT PdfDragDropWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
    GetClientRect(m_hWnd, &m_clientRect);
    
    // Обновляем полосы прокрутки
    UpdateScrollBars();
    
    // Перерисовываем окно
    InvalidateRect(m_hWnd, NULL, TRUE);
    
    return 0;
}

// Установка масштаба
BOOL PdfDragDropWindow::SetZoomLevel(double zoom)
{
    if (zoom < MIN_ZOOM) zoom = MIN_ZOOM;
    if (zoom > MAX_ZOOM) zoom = MAX_ZOOM;
    
    if (zoom != m_zoomLevel) {
        m_zoomLevel = zoom;
        UpdateScrollBars();
        InvalidateRect(m_hWnd, NULL, TRUE);
    }
    
    return TRUE;
}

// Установка текущей страницы
BOOL PdfDragDropWindow::SetCurrentPage(int pageNum)
{
    if (pageNum < 1 || pageNum > m_totalPages) return FALSE;
    
    if (pageNum != m_currentPage) {
        m_currentPage = pageNum;
        
        // Получаем размеры новой страницы
        if (m_pParent) {
            m_pParent->GetPageDimensions(pageNum, &m_pageWidth, &m_pageHeight);
        }
        
        UpdateScrollBars();
        InvalidateRect(m_hWnd, NULL, TRUE);
    }
    
    return TRUE;
}

// Включение/выключение drag-n-drop
BOOL PdfDragDropWindow::SetDragDropEnabled(BOOL bEnabled)
{
    m_bDragDropEnabled = bEnabled;
    DragAcceptFiles(m_hWnd, bEnabled);
    return TRUE;
}

// Установка размеров изображения по умолчанию
BOOL PdfDragDropWindow::SetImageSize(double width, double height)
{
    if (width > 0 && height > 0) {
        m_defaultImageWidth = width;
        m_defaultImageHeight = height;
        return TRUE;
    }
    return FALSE;
}

// Показ окна
BOOL PdfDragDropWindow::Show(BOOL bShow)
{
    if (m_hWnd) {
        ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
        if (bShow) {
            SetForegroundWindow(m_hWnd);
        }
        return TRUE;
    }
    return FALSE;
}

// Закрытие окна
BOOL PdfDragDropWindow::Close()
{
    if (m_hWnd) {
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        return TRUE;
    }
    return FALSE;
}

// Получение размеров страницы
BOOL PdfDragDropWindow::GetPageDimensions(int pageNum, double* width, double* height)
{
    if (!m_pParent || !width || !height) return FALSE;
    
    return m_pParent->GetPageDimensions(pageNum, width, height);
}

// Обновление полос прокрутки
void PdfDragDropWindow::UpdateScrollBars()
{
    if (!m_bPdfLoaded) return;
    
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    // Обновляем горизонтальную полосу прокрутки
    if (pageScreenWidth > m_clientRect.right) {
        // Нужна горизонтальная прокрутка
        SCROLLINFO si = {0};
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = pageScreenWidth - m_clientRect.right;
        si.nPage = m_clientRect.right / 4;
        si.nPos = m_scrollX;
        SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
    } else {
        m_scrollX = 0;
        SetScrollRange(m_hWnd, SB_HORZ, 0, 0, TRUE);
    }
    
    // Обновляем вертикальную полосу прокрутки
    if (pageScreenHeight > m_clientRect.bottom) {
        // Нужна вертикальная прокрутка
        SCROLLINFO si = {0};
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = pageScreenHeight - m_clientRect.bottom;
        si.nPage = m_clientRect.bottom / 4;
        si.nPos = m_scrollY;
        SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
    } else {
        m_scrollY = 0;
        SetScrollRange(m_hWnd, SB_VERT, 0, 0, TRUE);
    }
}

// Обновление области страницы
void PdfDragDropWindow::InvalidatePageArea()
{
    if (!m_bPdfLoaded) return;
    
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    int pageX = (m_clientRect.right - pageScreenWidth) / 2 - m_scrollX;
    int pageY = (m_clientRect.bottom - pageScreenHeight) / 2 - m_scrollY;
    
    RECT pageRect = {pageX, pageY, pageX + pageScreenWidth, pageY + pageScreenHeight};
    InvalidateRect(m_hWnd, &pageRect, FALSE);
}

// Конвертация координат страницы в экранные
BOOL PdfDragDropWindow::ConvertPageToScreen(double pageX, double pageY, POINT* screenPos)
{
    if (!m_bPdfLoaded || !screenPos) return FALSE;
    
    int pageScreenWidth = (int)(m_pageWidth * m_zoomLevel / 100.0);
    int pageScreenHeight = (int)(m_pageHeight * m_zoomLevel / 100.0);
    
    int pageScreenX = (m_clientRect.right - pageScreenWidth) / 2 - m_scrollX;
    int pageScreenY = (m_clientRect.bottom - pageScreenHeight) / 2 - m_scrollY;
    
    // Конвертируем координаты
    screenPos->x = pageScreenX + (long)(pageX * m_zoomLevel / 100.0);
    screenPos->y = pageScreenY + (long)((m_pageHeight - pageY) * m_zoomLevel / 100.0); // Инвертируем Y
    
    return TRUE;
}

// Вспомогательные функции
BOOL IsImageFile(const TCHAR* fileName)
{
    if (!fileName) return FALSE;
    
    const TCHAR* ext = _tcsrchr(fileName, _T('.'));
    if (!ext) return FALSE;
    
    const TCHAR* supportedExts[] = {
        _T(".jpg"), _T(".jpeg"), _T(".png"), _T(".bmp"), _T(".gif"), _T(".tiff"), _T(".tif")
    };
    
    for (int i = 0; i < sizeof(supportedExts) / sizeof(supportedExts[0]); i++) {
        if (_tcsicmp(ext, supportedExts[i]) == 0) {
            return TRUE;
        }
    }
    
    return FALSE;
}

// Получение размеров изображения
BOOL GetImageDimensions(const TCHAR* imagePath, int* width, int* height)
{
    if (!imagePath || !width || !height) return FALSE;
    
    // Используем GDI+ для получения размеров изображения
    using namespace Gdiplus;
    
    static BOOL gdiplusInitialized = FALSE;
    static GdiplusStartupInput gdiplusStartupInput;
    static ULONG_PTR gdiplusToken;
    
    if (!gdiplusInitialized) {
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        gdiplusInitialized = TRUE;
    }
    
    Image* image = new Image((const WCHAR*)imagePath);
    if (image && image->GetLastStatus() == Ok) {
        *width = image->GetWidth();
        *height = image->GetHeight();
        delete image;
        return TRUE;
    }
    
    if (image) delete image;
    return FALSE;
}

// Показ справки по drag-n-drop
void ShowDragDropHelp(HWND hParent)
{
    const TCHAR* helpText = 
        _T("Использование Drag & Drop редактора:\n\n")
        _T("1. Загрузите PDF файл через компоненту или метод LoadPdf\n")
        _T("2. Перетащите файлы изображений в окно\n")
        _T("3. Изображения будут добавлены в место, где вы их отпустили\n\n")
        _T("Горячие клавиши:\n")
        _T("• Page Up/Down - переключение страниц\n")
        _T("• +/- - масштабирование\n")
        _T("• Ctrl + колесико мыши - масштабирование\n")
        _T("• Escape - отмена drag-n-drop операции\n\n")
        _T("Поддерживаемые форматы изображений:\n")
        _T("JPG, PNG, BMP, GIF, TIFF");
    
    MessageBox(hParent, helpText, _T("Справка - Drag & Drop"), MB_OK | MB_ICONINFORMATION);
} 