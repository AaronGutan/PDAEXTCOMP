@echo off
echo ========================================
echo Сборка компонентов 1С:Предприятие 8
echo ========================================

:: Настройка окружения Visual Studio
echo Настройка среды Visual Studio...
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat" 2>NUL
if errorlevel 1 (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" 2>NUL
)
if errorlevel 1 (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" 2>NUL
)
if errorlevel 1 (
    echo Внимание: Visual Studio не найден. Пытаемся продолжить...
)

:: Создание директорий
echo Создание директорий...
if not exist "lib" mkdir lib
if not exist "bin" mkdir bin

:: Скачивание PDFium (если нужно)
if not exist "include\fpdfview.h" (
    echo Скачивание PDFium...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%%2F5790/pdfium-win-x86.tgz' -OutFile 'pdfium-win-x86.tgz'}"
    
    echo Распаковка PDFium...
    tar -xzf pdfium-win-x86.tgz
    
    if not exist "include" mkdir include
    copy bin\*.dll lib\ >NUL 2>&1
    copy lib\*.lib lib\ >NUL 2>&1
    xcopy include\* include\ /E /I /Q >NUL 2>&1
    
    echo PDFium установлен
) else (
    echo PDFium уже установлен
)

:: Сборка базовой библиотеки AddIn
echo.
echo ========================================
echo Сборка базовой библиотеки AddIn...
echo ========================================

cd Source\Addin
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I. *.cpp
if errorlevel 1 (
    echo Ошибка компиляции базовой библиотеки!
    pause
    exit /b 1
)

lib /OUT:..\..\lib\addin.lib *.obj
if errorlevel 1 (
    echo Ошибка создания библиотеки!
    pause
    exit /b 1
)

echo Базовая библиотека создана
cd ..\..

:: Сборка PdfImageAddIn
echo.
echo ========================================
echo Сборка PdfImageAddIn...
echo ========================================

cd Source\PdfImageAddIn
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I..\..\include /I..\Addin /I. PdfImageAddIn.cpp MyClassFactory.cpp
if errorlevel 1 (
    echo Ошибка компиляции PdfImageAddIn!
    pause
    exit /b 1
)

link /DLL /OUT:..\..\bin\PdfImageAddIn.dll /DEF:PdfImageAddIn.def PdfImageAddIn.obj MyClassFactory.obj ..\..\lib\addin.lib ..\..\lib\pdfium.lib ole32.lib oleaut32.lib uuid.lib
if errorlevel 1 (
    echo Ошибка линковки PdfImageAddIn!
    pause
    exit /b 1
)

echo PdfImageAddIn.dll создан
cd ..\..

:: Сборка LogAddIn
echo.
echo ========================================
echo Сборка LogAddIn...
echo ========================================

cd Source\LogAddin
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I..\Addin /I. LogAddIn.cpp MyClassFactory.cpp
if errorlevel 1 (
    echo Ошибка компиляции LogAddIn!
    pause
    exit /b 1
)

link /DLL /OUT:..\..\bin\LogAddIn.dll /DEF:ADDIN.DEF LogAddIn.obj MyClassFactory.obj ..\..\lib\addin.lib ole32.lib oleaut32.lib uuid.lib
if errorlevel 1 (
    echo Ошибка линковки LogAddIn!
    pause
    exit /b 1
)

echo LogAddIn.dll создан
cd ..\..

:: Сборка ScanSymb
echo.
echo ========================================
echo Сборка ScanSymb...
echo ========================================

cd Source\ScanSymb
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I..\Addin /I. ScanAddIn.cpp MyClassFactory.cpp ScanWindow.cpp
if errorlevel 1 (
    echo Ошибка компиляции ScanSymb!
    pause
    exit /b 1
)

link /DLL /OUT:..\..\bin\ScanSymb.dll /DEF:ScanSymb.DEF ScanAddIn.obj MyClassFactory.obj ScanWindow.obj ..\..\lib\addin.lib ole32.lib oleaut32.lib uuid.lib user32.lib
if errorlevel 1 (
    echo Ошибка линковки ScanSymb!
    pause
    exit /b 1
)

echo ScanSymb.dll создан
cd ..\..

:: Проверка результатов
echo.
echo ========================================
echo Проверка результатов сборки...
echo ========================================

if exist "bin\PdfImageAddIn.dll" (
    echo ✅ PdfImageAddIn.dll успешно создан
) else (
    echo ❌ PdfImageAddIn.dll не найден
)

if exist "bin\LogAddIn.dll" (
    echo ✅ LogAddIn.dll успешно создан
) else (
    echo ❌ LogAddIn.dll не найден
)

if exist "bin\ScanSymb.dll" (
    echo ✅ ScanSymb.dll успешно создан
) else (
    echo ❌ ScanSymb.dll не найден
)

echo.
echo ========================================
echo Сборка завершена!
echo ========================================
echo Результаты в папке: bin\
echo Документация: *.htm
echo.

pause 