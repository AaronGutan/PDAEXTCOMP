# 🚀 Онлайн компиляция проекта

## Способы онлайн компиляции

### 1. GitHub Actions (Рекомендуется)

Проект настроен для автоматической сборки через GitHub Actions:

1. **Загрузите код в репозиторий GitHub**
2. **Запустите workflow:**
   - Перейдите в раздел "Actions"
   - Выберите "Build PdfImageAddIn"
   - Нажмите "Run workflow"

3. **Скачайте результаты:**
   - После завершения сборки скачайте артефакты
   - Все DLL файлы будут в папке `artifacts`

### 2. Онлайн IDE

#### Replit
```bash
# Создайте новый проект C++
# Загрузите исходный код
# Запустите в терминале:
chmod +x build.sh
./build.sh
```

#### CodeSandbox
```bash
# Создайте новый проект
# Загрузите файлы
# Установите зависимости
npm install --save-dev @msvc/compiler
```

### 3. Онлайн компиляторы

#### Compiler Explorer (godbolt.org)
```cpp
// Скопируйте код отдельных файлов
// Настройте компилятор MSVC
// Добавьте флаги: /EHsc /DWIN32 /D_WINDOWS
```

#### OnlineGDB
```bash
# Выберите C++ компилятор
# Загрузите main файл
# Добавьте дополнительные файлы
```

### 4. Cloud Development Environments

#### GitHub Codespaces
```bash
# Откройте репозиторий на GitHub
# Нажмите "Code" -> "Codespaces" -> "New codespace"
# В терминале выполните:
./build.bat
```

#### Gitpod
```bash
# Добавьте префикс к URL: gitpod.io/#<your-repo-url>
# Дождитесь запуска окружения
# Выполните:
make full_build
```

## Быстрая сборка

### Автоматическая сборка (Windows)
```batch
# Запустите build.bat
build.bat
```

### Ручная сборка
```batch
# 1. Настройте Visual Studio
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

# 2. Создайте директории
mkdir lib
mkdir bin

# 3. Скачайте PDFium
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%2F5790/pdfium-win-x86.tgz' -OutFile 'pdfium.tgz'"
tar -xzf pdfium.tgz

# 4. Соберите базовую библиотеку
cd Source\Addin
cl /c /EHsc /DWIN32 /D_WINDOWS *.cpp
lib /OUT:..\..\lib\addin.lib *.obj
cd ..\..

# 5. Соберите PdfImageAddIn
cd Source\PdfImageAddIn
cl /c /EHsc /DWIN32 /D_WINDOWS /I..\..\include /I..\Addin PdfImageAddIn.cpp MyClassFactory.cpp
link /DLL /OUT:..\..\bin\PdfImageAddIn.dll /DEF:PdfImageAddIn.def PdfImageAddIn.obj MyClassFactory.obj ..\..\lib\addin.lib ..\..\lib\pdfium.lib ole32.lib oleaut32.lib uuid.lib
cd ..\..
```

## Использование Makefile

```bash
# Полная сборка с установкой PDFium
make full_build

# Только компиляция
make all

# Очистка
make clean

# Установка PDFium отдельно
make install_pdfium

# Тестирование
make test
```

## Требования

### Минимальные
- Windows 10/11
- Visual Studio 2019 или новее
- PowerShell 5.1+

### Рекомендуемые
- Visual Studio 2022 Community
- Windows SDK 10.0.19041.0
- Git для Windows

## Структура результатов

После успешной сборки в папке `bin/`:
- `PdfImageAddIn.dll` - Основная компонента для работы с PDF
- `LogAddIn.dll` - Компонента для логирования
- `ScanSymb.dll` - Компонента для сканирования штрих-кодов

## Устранение проблем

### Ошибки компиляции
```bash
# Проверьте настройки Visual Studio
where cl
where link

# Обновите переменные окружения
set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx86\x86

# Очистите временные файлы
make clean
```

### Проблемы с PDFium
```bash
# Проверьте скачивание
dir include\fpdfview.h
dir lib\pdfium.lib

# Переустановите PDFium
make install_pdfium
```

### Проблемы с зависимостями
```bash
# Проверьте наличие библиотек
dumpbin /imports bin\PdfImageAddIn.dll

# Установите Visual C++ Redistributable
# https://aka.ms/vs/17/release/vc_redist.x86.exe
```

## Онлайн ресурсы

- 🔗 [Compiler Explorer](https://godbolt.org/)
- 🔗 [OnlineGDB](https://www.onlinegdb.com/)
- 🔗 [Replit](https://replit.com/)
- 🔗 [CodeSandbox](https://codesandbox.io/)
- 🔗 [GitHub Codespaces](https://github.com/features/codespaces)
- 🔗 [Gitpod](https://gitpod.io/)

## Примеры использования

### Тестирование в 1С
```javascript
// Подключение компоненты
ВнешняяКомпонента = Новый COMОбъект("Addin.PdfImageAddIn");

// Загрузка PDF
ВнешняяКомпонента.ЗагрузитьПдф("C:\test.pdf");

// Добавление изображения
ВнешняяКомпонента.ДобавитьИзображение("C:\logo.png", 100, 700, 200, 100, 1);

// Сохранение
ВнешняяКомпонента.СохранитьПдф("C:\result.pdf");
```

### Регистрация компоненты
```cmd
# Регистрация в системе
regsvr32 PdfImageAddIn.dll

# Отмена регистрации
regsvr32 /u PdfImageAddIn.dll
```

---

**💡 Совет:** Для первой сборки рекомендуется использовать GitHub Actions, так как все зависимости будут установлены автоматически. 