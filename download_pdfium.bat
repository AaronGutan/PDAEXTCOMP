@echo off
echo ========================================
echo Скачивание PDFium библиотек
echo ========================================

if exist "include\fpdfview.h" (
    echo ✅ PDFium уже установлен
    goto :end
)

echo 🔄 Скачивание PDFium с retry логикой...

powershell -Command "& {
    $url = 'https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%%2F5790/pdfium-win-x86.tgz'
    $output = 'pdfium-win-x86.tgz'
    $maxAttempts = 3
    $attempt = 1
    
    while ($attempt -le $maxAttempts) {
        try {
            Write-Host \"🔄 Попытка $attempt из $maxAttempts загрузки PDFium...\"
            
            # Попытка 1: curl (более надежный)
            if (Get-Command curl -ErrorAction SilentlyContinue) {
                Write-Host \"📡 Используем curl для загрузки...\"
                & curl -L -o $output $url --retry 3 --retry-delay 5 --max-time 300 --progress-bar
                if ($LASTEXITCODE -eq 0) {
                    Write-Host \"✅ Загрузка через curl успешна\"
                    break
                }
            }
            
            # Попытка 2: WebClient
            Write-Host \"📡 Используем WebClient для загрузки...\"
            $webClient = New-Object System.Net.WebClient
            $webClient.DownloadFile($url, $output)
            Write-Host \"✅ Загрузка через WebClient успешна\"
            break
            
        } catch {
            Write-Host \"❌ Попытка $attempt не удалась: $($_.Exception.Message)\"
            
            if ($attempt -eq $maxAttempts) {
                Write-Host \"🔄 Пробуем альтернативный источник...\"
                $altUrl = 'https://github.com/bblanchon/pdfium-binaries/releases/latest/download/pdfium-win-x86.tgz'
                try {
                    & curl -L -o $output $altUrl --retry 3 --retry-delay 5 --max-time 300 --progress-bar
                    if ($LASTEXITCODE -eq 0) {
                        Write-Host \"✅ Загрузка из альтернативного источника успешна\"
                        break
                    }
                } catch {
                    Write-Host \"❌ Все попытки загрузки не удались\"
                    throw \"Не удалось загрузить PDFium библиотеки\"
                }
            }
            
            $attempt++
            Write-Host \"⏳ Ожидание 10 секунд перед следующей попыткой...\"
            Start-Sleep -Seconds 10
        }
    }
    
    # Проверяем что файл загружен
    if (!(Test-Path $output)) {
        throw \"Файл PDFium не был загружен\"
    }
    
    Write-Host \"✅ Файл PDFium успешно загружен\"
}"

if errorlevel 1 (
    echo ❌ Ошибка загрузки PDFium
    pause
    exit /b 1
)

echo 📦 Распаковка PDFium...
tar -xzf pdfium-win-x86.tgz
if errorlevel 1 (
    echo ❌ Ошибка распаковки PDFium
    pause
    exit /b 1
)

echo 📁 Создание директорий...
if not exist "lib" mkdir lib
if not exist "include" mkdir include

echo 📂 Копирование файлов...
if exist "bin" (
    copy bin\*.dll lib\ >NUL 2>&1
    echo ✅ DLL файлы скопированы
) else (
    echo ⚠️ Папка bin не найдена
)

if exist "lib" (
    copy lib\*.lib lib\ >NUL 2>&1
    echo ✅ LIB файлы скопированы
) else (
    echo ⚠️ Папка lib не найдена
)

if exist "include" (
    xcopy include\* include\ /E /I /Q >NUL 2>&1
    echo ✅ Заголовочные файлы скопированы
) else (
    echo ⚠️ Папка include не найдена
)

echo 🧹 Очистка временных файлов...
del pdfium-win-x86.tgz >NUL 2>&1

echo 📊 Проверка установки:
if exist "include\fpdfview.h" (
    echo ✅ fpdfview.h найден
) else (
    echo ❌ fpdfview.h не найден
)

if exist "lib\pdfium.lib" (
    echo ✅ pdfium.lib найден
) else (
    echo ❌ pdfium.lib не найден
)

echo.
echo ========================================
echo PDFium установлен успешно!
echo ========================================

:end
pause 