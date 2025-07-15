# 🚀 PdfImageAddIn - Внешняя компонента для 1С:Предприятие 8

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)
![Version](https://img.shields.io/badge/version-1.0-orange)

Внешняя компонента для работы с PDF документами в 1С:Предприятие 8. Позволяет добавлять изображения в PDF файлы с визуальным редактором drag-and-drop.

## 📋 Содержание

- [🔧 Компонента](#-компонента)
- [🚀 Быстрая сборка](#-быстрая-сборка)
- [🌐 Онлайн компиляция](#-онлайн-компиляция)
- [📖 Документация](#-документация)
- [🛠️ Требования](#️-требования)
- [📝 Примеры использования](#-примеры-использования)

## 🔧 Компонента

### **PdfImageAddIn** - Работа с PDF файлами ⭐
> Внешняя компонента для размещения изображений в PDF документах

**Возможности:**
- ✅ Загрузка PDF файлов
- ✅ Добавление изображений с точными координатами
- ✅ **Drag & Drop редактор** - визуальное добавление изображений
- ✅ Поддержка множественных страниц
- ✅ Сохранение модифицированных PDF
- ✅ Интеграция с PDFium

**Файлы:**
- `PdfImageAddIn.dll` - основная библиотека
- `PdfImageComp.htm` - документация
- `example.1c` - примеры использования

### **AddIn** - Базовая библиотека
> Основа для внешней компоненты

**Возможности:**
- ✅ COM интерфейсы
- ✅ Фабрика классов
- ✅ Общие утилиты

## 🚀 Быстрая сборка

### Автоматическая сборка (Windows)
```batch
# Скачайте проект
git clone <repository-url>
cd PDAEXTCOMP

# Запустите автоматическую сборку
build.bat
```

### Использование Makefile
```bash
# Полная сборка с установкой PDFium
make full_build

# Только компиляция
make all

# Тестирование
make test
```

### Результаты сборки
После успешной сборки в папке `bin/`:
- `PdfImageAddIn.dll` - для работы с PDF

## 🌐 Онлайн компиляция

### GitHub Actions (Рекомендуется) ⭐
```yaml
# Автоматическая сборка настроена!
# 1. Создайте репозиторий на GitHub
# 2. Перейдите в Actions → Build PdfImageAddIn
# 3. Нажмите "Run workflow"
# 4. Скачайте артефакты
```

### Другие способы
- **GitHub Codespaces** - полная IDE в браузере
- **Gitpod** - среда разработки в облаке
- **Replit** - для простых тестов
- **Compiler Explorer** - для анализа кода

> 📖 Подробные инструкции: [ONLINE_BUILD.md](ONLINE_BUILD.md)

### Быстрый тест
```batch
# Проверка компилятора
quick_test.bat

# Интерактивный тест
start online_compiler_test.html
```

## 📖 Документация

### Файлы документации
- 📋 [CompDev.htm](CompDev.htm) - Порядок использования библиотеки AddIn
- 📋 [PdfImageComp.htm](PdfImageComp.htm) - Описание PdfImageAddIn.dll
- 📋 [Readme.htm](Readme.htm) - Общее описание поставки

### Технические документы
- 🔧 [PDFIUM_INSTALL.md](Source/PdfImageAddIn/PDFIUM_INSTALL.md) - Установка PDFium
- 🌐 [ONLINE_BUILD.md](ONLINE_BUILD.md) - Онлайн компиляция
- 📚 [README.md](Source/PdfImageAddIn/README.md) - Подробное описание PdfImageAddIn

## 🛠️ Требования

### Минимальные
- **OS:** Windows 10/11
- **Компилятор:** Visual Studio 2019+
- **Среда:** PowerShell 5.1+

### Рекомендуемые
- **IDE:** Visual Studio 2022 Community
- **SDK:** Windows SDK 10.0.19041.0
- **Система:** Git для Windows

### Зависимости
- **PDFium** - автоматически скачивается
- **COM библиотеки** - встроены в Windows
- **C++ Runtime** - включен в Visual Studio

## 📝 Примеры использования

### PdfImageAddIn в 1С
```javascript
// Создание объекта компоненты
ВнешняяКомпонента = Новый COMОбъект("Addin.PdfImageAddIn");

// Загрузка PDF файла
Если ВнешняяКомпонента.ЗагрузитьПдф("C:\Documents\document.pdf") Тогда
    
    // Добавление логотипа на первую страницу
    ВнешняяКомпонента.ДобавитьИзображение(
        "C:\Images\logo.png",  // Путь к изображению
        100,                   // X координата
        700,                   // Y координата  
        200,                   // Ширина
        100,                   // Высота
        1                      // Номер страницы
    );
    
    // Сохранение результата
    ВнешняяКомпонента.СохранитьПдф("C:\Documents\result.pdf");
    
КонецЕсли;
```

### Drag & Drop редактор
```javascript
// Открытие визуального редактора
Если ВнешняяКомпонента.ОткрытьDragDropРедактор() Тогда
    Сообщить("Drag & Drop редактор открыт!");
    Сообщить("Перетащите изображения в окно редактора");
    
    // Показать справку по использованию
    ВнешняяКомпонента.ПоказатьСправкуDragDrop();
    
    // Окно остается открытым для работы пользователя
    
КонецЕсли;
```



### Регистрация компоненты
```cmd
:: Регистрация
regsvr32 bin\PdfImageAddIn.dll

:: Отмена регистрации
regsvr32 /u bin\PdfImageAddIn.dll
```

## 🔧 Устранение проблем

### Проблемы компиляции
```batch
# Проверка компилятора
where cl
where link

# Настройка окружения
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

# Очистка и пересборка
make clean
make all
```

### Проблемы с PDFium
```batch
# Переустановка PDFium
make install_pdfium

# Проверка установки
dir include\fpdfview.h
dir lib\pdfium.lib
```

### Проблемы с зависимостями
```batch
# Проверка DLL
dumpbin /imports bin\PdfImageAddIn.dll

# Установка Runtime
# https://aka.ms/vs/17/release/vc_redist.x86.exe
```

## 📊 Статистика проекта

| Компонент | Файлы | Размер | Статус |
|-----------|--------|--------|--------|
| PdfImageAddIn | 12 | ~500KB | ✅ Готов |
| AddIn (база) | 6 | ~200KB | ✅ Готов |

## 🤝 Вклад в проект

1. Форкните репозиторий
2. Создайте ветку для функции (`git checkout -b feature/AmazingFeature`)
3. Закоммитьте изменения (`git commit -m 'Add some AmazingFeature'`)
4. Запушьте в ветку (`git push origin feature/AmazingFeature`)
5. Откройте Pull Request

## 📄 Лицензия

Этот проект предоставляется "как есть" для использования с 1С:Предприятие 8.

## 🔗 Полезные ссылки

- 🌐 [1С:Предприятие](https://v8.1c.ru/)
- 🔧 [PDFium](https://pdfium.googlesource.com/pdfium/)
- 💻 [Visual Studio](https://visualstudio.microsoft.com/)
- 🚀 [GitHub Actions](https://github.com/features/actions)

---

**⭐ Поставьте звездочку проекту, если он оказался полезным!**

> 💡 **Рекомендация:** Для первой сборки используйте GitHub Actions - все зависимости будут установлены автоматически. 