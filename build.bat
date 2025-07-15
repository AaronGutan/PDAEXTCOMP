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
    echo Скачивание PDFium с retry логикой...
    powershell -Command "& { $url = 'https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%%2F5790/pdfium-win-x86.tgz'; $output = 'pdfium-win-x86.tgz'; $maxAttempts = 3; $attempt = 1; while ($attempt -le $maxAttempts) { try { Write-Host \"Попытка $attempt из $maxAttempts...\"; if (Get-Command curl -ErrorAction SilentlyContinue) { Write-Host \"Используем curl...\"; & curl -L -o $output $url --retry 3 --retry-delay 5 --max-time 300; if ($LASTEXITCODE -eq 0) { Write-Host \"Загрузка через curl успешна\"; break } }; Write-Host \"Используем WebClient...\"; $webClient = New-Object System.Net.WebClient; $webClient.DownloadFile($url, $output); Write-Host \"Загрузка через WebClient успешна\"; break } catch { Write-Host \"Попытка $attempt не удалась: $($_.Exception.Message)\"; if ($attempt -eq $maxAttempts) { $altUrl = 'https://github.com/bblanchon/pdfium-binaries/releases/latest/download/pdfium-win-x86.tgz'; try { & curl -L -o $output $altUrl --retry 3 --retry-delay 5 --max-time 300; if ($LASTEXITCODE -eq 0) { Write-Host \"Загрузка из альтернативного источника успешна\"; break } } catch { throw \"Не удалось загрузить PDFium библиотеки\" } }; $attempt++; Start-Sleep -Seconds 5 } } }"
    
    echo Распаковка PDFium...
    tar -xzf pdfium-win-x86.tgz
    
    if not exist "include" mkdir include
    if exist "bin" copy bin\*.dll lib\ >NUL 2>&1
    if exist "lib" copy lib\*.lib lib\ >NUL 2>&1
    if exist "include" xcopy include\* include\ /E /I /Q >NUL 2>&1
    
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
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I. /I../../include *.cpp
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
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I. /I..\..\include /I..\Addin PdfImageAddIn.cpp MyClassFactory.cpp PdfDragDropWindow.cpp
if errorlevel 1 (
    echo Ошибка компиляции PdfImageAddIn!
    pause
    exit /b 1
)

link /DLL /OUT:..\..\bin\PdfImageAddIn.dll /DEF:PdfImageAddIn.def PdfImageAddIn.obj MyClassFactory.obj PdfDragDropWindow.obj ..\..\lib\addin.lib ..\..\lib\pdfium.lib ole32.lib oleaut32.lib uuid.lib user32.lib gdi32.lib
if errorlevel 1 (
    echo Ошибка линковки PdfImageAddIn!
    pause
    exit /b 1
)

echo PdfImageAddIn.dll создан
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

echo.
echo ========================================
echo Сборка завершена!
echo ========================================
echo Результаты в папке: bin\
echo Документация: *.htm
echo.

pause 