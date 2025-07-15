# 🔧 Решение проблемы с компиляцией

## Проблема
При попытке сборки возникают ошибки:
- `'cl' не является внутренней или внешней командой`
- `Не удается скачать PDFium`

## ✅ Рекомендуемые решения

### 🚀 Вариант 1: GitHub Actions (Самый простой)

**Преимущества:**
- ✅ Автоматическая установка всех зависимостей
- ✅ Не требует установки ничего на локальной машине
- ✅ Бесплатно для публичных репозиториев
- ✅ Готовые артефакты для скачивания

**Шаги:**
1. Создайте новый репозиторий на GitHub
2. Загрузите туда все файлы проекта
3. Перейдите в `Actions` → `Build PdfImageAddIn`
4. Нажмите `Run workflow`
5. Скачайте артефакты после завершения

### 🔧 Вариант 2: Установка Visual Studio Build Tools

**Для локальной сборки:**
```batch
# Запустите автоматическую установку
.\install_build_tools.bat
```

**Ручная установка:**
1. Скачайте [Visual Studio Build Tools](https://aka.ms/vs/17/release/vs_buildtools.exe)
2. Установите с компонентами:
   - C++ build tools
   - Windows 10/11 SDK
   - MSVC v143 - VS 2022 C++ x64/x86 build tools
3. Перезапустите командную строку
4. Запустите `build.bat`

### 💻 Вариант 3: Онлайн IDE

**GitHub Codespaces:**
1. Откройте репозиторий на GitHub
2. Нажмите `Code` → `Codespaces` → `New codespace`
3. В терминале выполните: `./build.bat`

**Gitpod:**
1. Добавьте префикс к URL: `gitpod.io/#<your-repo-url>`
2. Дождитесь запуска окружения
3. Выполните: `make full_build`

### 📋 Вариант 4: Альтернативные компиляторы

**MinGW-w64:**
```batch
# Установите через chocolatey
choco install mingw

# Или scoop
scoop install mingw
```

## 🧪 Проверка инструментов

Для проверки доступных инструментов запустите:
```batch
.\quick_test.bat
```

Или откройте в браузере:
```batch
start online_compiler_test.html
```

## 🛠️ Устранение конкретных проблем

### PDFium не скачивается
```batch
# Используйте надежный скрипт загрузки
.\download_pdfium.bat

# Или попробуйте альтернативный способ
curl -L "https://github.com/bblanchon/pdfium-binaries/releases/download/chromium%2F5790/pdfium-win-x86.tgz" -o pdfium.tgz
tar -xzf pdfium.tgz

# Альтернативный источник
curl -L "https://github.com/bblanchon/pdfium-binaries/releases/latest/download/pdfium-win-x86.tgz" -o pdfium.tgz
```

### Компилятор не найден
```batch
# Найдите Visual Studio
dir "C:\Program Files*\Microsoft Visual Studio" /s /b

# Или поищите build tools
where cl
where link
```

### Проблемы с путями
```batch
# Добавьте в PATH
set PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.30.30705\bin\Hostx64\x64
```

## 📖 Дополнительная информация

- 📋 [Полная документация по онлайн сборке](ONLINE_BUILD.md)
- 🌐 [Тест онлайн компиляторов](online_compiler_test.html)
- 📖 [Инструкции по установке PDFium](Source/PdfImageAddIn/PDFIUM_INSTALL.md)

## 🎯 Рекомендация

**Для первой сборки:** Используйте GitHub Actions - это самый надежный способ.

**Для разработки:** Установите Visual Studio Build Tools локально.

**Для экспериментов:** Попробуйте онлайн IDE.

---

💡 **Совет:** Если вы не планируете часто компилировать проект, GitHub Actions - идеальный выбор! 