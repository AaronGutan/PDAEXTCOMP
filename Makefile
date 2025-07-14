# Makefile для сборки компонентов 1С
# Использование: make all

# Переменные компилятора
CC = cl
LINK = link
LIB = lib

# Пути
ADDIN_PATH = Source/Addin
PDFIMAGE_PATH = Source/PdfImageAddIn
LOGADDIN_PATH = Source/LogAddin
SCANSYMB_PATH = Source/ScanSymb

# Флаги компилятора
CFLAGS = /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL /I$(ADDIN_PATH) /Iinclude

# Библиотеки
LIBS = ole32.lib oleaut32.lib uuid.lib user32.lib
PDFIUM_LIBS = lib/pdfium.lib

# Цели
all: setup base_lib pdf_addin log_addin scan_addin

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
	cd $(PDFIMAGE_PATH) && $(CC) $(CFLAGS) /I../../include PdfImageAddIn.cpp MyClassFactory.cpp
	cd $(PDFIMAGE_PATH) && $(LINK) /DLL /OUT:../../bin/PdfImageAddIn.dll /DEF:PdfImageAddIn.def PdfImageAddIn.obj MyClassFactory.obj ../../lib/addin.lib $(PDFIUM_LIBS) $(LIBS)
	@echo PdfImageAddIn.dll создан

# LogAddIn
log_addin: base_lib
	@echo Сборка LogAddIn...
	cd $(LOGADDIN_PATH) && $(CC) $(CFLAGS) LogAddIn.cpp MyClassFactory.cpp
	cd $(LOGADDIN_PATH) && $(LINK) /DLL /OUT:../../bin/LogAddIn.dll /DEF:ADDIN.DEF LogAddIn.obj MyClassFactory.obj ../../lib/addin.lib $(LIBS)
	@echo LogAddIn.dll создан

# ScanSymb
scan_addin: base_lib
	@echo Сборка ScanSymb...
	cd $(SCANSYMB_PATH) && $(CC) $(CFLAGS) ScanAddIn.cpp MyClassFactory.cpp ScanWindow.cpp
	cd $(SCANSYMB_PATH) && $(LINK) /DLL /OUT:../../bin/ScanSymb.dll /DEF:ScanSymb.DEF ScanAddIn.obj MyClassFactory.obj ScanWindow.obj ../../lib/addin.lib $(LIBS)
	@echo ScanSymb.dll создан

# Очистка
clean:
	@echo Очистка...
	@if exist "lib" rmdir /s /q lib
	@if exist "bin" rmdir /s /q bin
	@for /d /r . %%d in (*.obj) do @if exist "%%d" del /q "%%d"
	@echo Очистка завершена

# Установка PDFium
install_pdfium:
	@echo Скачивание PDFium...
	powershell -Command "Invoke-WebRequest -Uri 'https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%%2F5790/pdfium-win-x86.tgz' -OutFile 'pdfium-win-x86.tgz'"
	@echo Распаковка PDFium...
	tar -xzf pdfium-win-x86.tgz
	@if not exist "lib" mkdir lib
	@if not exist "include" mkdir include
	copy bin\*.dll lib\
	copy lib\*.lib lib\
	xcopy include\* include\ /E /I
	@echo PDFium установлен

# Тест сборки
test:
	@echo Тестирование собранных компонентов...
	@if exist "bin\PdfImageAddIn.dll" (echo ✅ PdfImageAddIn.dll найден) else (echo ❌ PdfImageAddIn.dll не найден)
	@if exist "bin\LogAddIn.dll" (echo ✅ LogAddIn.dll найден) else (echo ❌ LogAddIn.dll не найден)
	@if exist "bin\ScanSymb.dll" (echo ✅ ScanSymb.dll найден) else (echo ❌ ScanSymb.dll не найден)

# Полная сборка с установкой PDFium
full_build: install_pdfium all test

.PHONY: all setup base_lib pdf_addin log_addin scan_addin clean install_pdfium test full_build 