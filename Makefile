# Makefile для сборки компонентов 1С
# Использование: make all

# Переменные компилятора
CC = cl
LINK = link
LIB = lib

# Пути
ADDIN_PATH = Source/Addin
PDFIMAGE_PATH = Source/PdfImageAddIn

# Флаги компилятора
CFLAGS = /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I$(ADDIN_PATH) /Iinclude

# Библиотеки
LIBS = ole32.lib oleaut32.lib uuid.lib user32.lib
PDFIUM_LIBS = lib/pdfium.lib

# Цели
all: setup base_lib pdf_addin

# Создание директорий
setup:
	@if not exist "lib" mkdir lib
	@if not exist "bin" mkdir bin
	@echo Настройка окружения завершена

# Базовая библиотека AddIn
base_lib:
	@echo Сборка базовой библиотеки AddIn...
	cd $(ADDIN_PATH) && $(CC) $(CFLAGS) *.cpp
	cd $(ADDIN_PATH) && $(LIB) /OUT:../../lib/addin.lib *.obj
	@echo Базовая библиотека создана

# PdfImageAddIn
pdf_addin: base_lib
	@echo Сборка PdfImageAddIn...
	cd $(PDFIMAGE_PATH) && $(CC) $(CFLAGS) /I../../include PdfImageAddIn.cpp MyClassFactory.cpp PdfDragDropWindow.cpp
	cd $(PDFIMAGE_PATH) && $(LINK) /DLL /OUT:../../bin/PdfImageAddIn.dll /DEF:PdfImageAddIn.def PdfImageAddIn.obj MyClassFactory.obj PdfDragDropWindow.obj ../../lib/addin.lib $(PDFIUM_LIBS) $(LIBS)
	@echo PdfImageAddIn.dll создан



# Очистка
clean:
	@echo Очистка...
	@if exist "lib" rmdir /s /q lib
	@if exist "bin" rmdir /s /q bin
	@for /d /r . %%d in (*.obj) do @if exist "%%d" del /q "%%d"
	@echo Очистка завершена

# Установка PDFium
install_pdfium:
	@echo Скачивание PDFium с retry логикой...
	powershell -Command "& { $url = 'https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%%2F5790/pdfium-win-x86.tgz'; $output = 'pdfium-win-x86.tgz'; $maxAttempts = 3; $attempt = 1; while ($attempt -le $maxAttempts) { try { Write-Host \"Попытка $attempt из $maxAttempts...\"; if (Get-Command curl -ErrorAction SilentlyContinue) { Write-Host \"Используем curl...\"; & curl -L -o $output $url --retry 3 --retry-delay 5 --max-time 300; if ($LASTEXITCODE -eq 0) { Write-Host \"Загрузка через curl успешна\"; break } }; Write-Host \"Используем WebClient...\"; $webClient = New-Object System.Net.WebClient; $webClient.DownloadFile($url, $output); Write-Host \"Загрузка через WebClient успешна\"; break } catch { Write-Host \"Попытка $attempt не удалась: $($_.Exception.Message)\"; if ($attempt -eq $maxAttempts) { $altUrl = 'https://github.com/bblanchon/pdfium-binaries/releases/latest/download/pdfium-win-x86.tgz'; try { & curl -L -o $output $altUrl --retry 3 --retry-delay 5 --max-time 300; if ($LASTEXITCODE -eq 0) { Write-Host \"Загрузка из альтернативного источника успешна\"; break } } catch { throw \"Не удалось загрузить PDFium библиотеки\" } }; $attempt++; Start-Sleep -Seconds 5 } } }"
	@echo Распаковка PDFium...
	tar -xzf pdfium-win-x86.tgz
	@if not exist "lib" mkdir lib
	@if not exist "include" mkdir include
	@if exist "bin" copy bin\*.dll lib\
	@if exist "lib" copy lib\*.lib lib\
	@if exist "include" xcopy include\* include\ /E /I
	@echo PDFium установлен

# Тест сборки
test:
	@echo Тестирование собранных компонентов...
	@if exist "bin\PdfImageAddIn.dll" (echo ✅ PdfImageAddIn.dll найден) else (echo ❌ PdfImageAddIn.dll не найден)

# Полная сборка с установкой PDFium
full_build: install_pdfium all test

.PHONY: all setup base_lib pdf_addin clean install_pdfium test full_build 