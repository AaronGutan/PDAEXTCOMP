# PdfImageAddIn - Упрощенная версия (без PDFium)

## Описание

Упрощенная версия внешней компоненты для 1С, которая предоставляет интерфейс для работы с PDF файлами без использования PDFium. Компонента работает в режиме заглушек и предназначена для тестирования COM-интеграции.

## Преимущества упрощенной версии

- ✅ **Нет зависимостей** от PDFium
- ✅ **Простая сборка** без внешних библиотек
- ✅ **Быстрое тестирование** COM-интеграции
- ✅ **Меньший размер** DLL
- ✅ **Простое развертывание**

## Сборка

### Требования
- Visual Studio 2015/2017/2019/2022
- Windows SDK
- 32-битная сборка (для совместимости с 1С)

### Автоматическая сборка
```batch
build_simple.bat
```

### Ручная сборка
```batch
cd Source\Addin
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL *.cpp
lib /OUT:..\..\lib\addin.lib *.obj
cd ..\PdfImageAddIn
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /I..\..\include /I..\Addin PdfImageAddIn_Simple.cpp MyClassFactory.cpp
link /DLL /OUT:..\..\bin\PdfImageAddIn_Simple.dll /DEF:PdfImageAddIn_Simple.def *.obj ..\..\lib\addin.lib ole32.lib oleaut32.lib uuid.lib
```

## Регистрация

После сборки зарегистрируйте компоненту:

```batch
regsvr32 bin\PdfImageAddIn_Simple.dll
```

## Использование в 1С

```1c
// Создание объекта
ВнешняяКомпонента = Новый COMОбъект("Addin.PdfImageAddIn");

// Тест соединения
Если ВнешняяКомпонента.TestConnection() Тогда
    Сообщить("Компонента работает!");
КонецЕсли;

// Загрузка PDF (заглушка)
ВнешняяКомпонента.LoadPdf("C:\test.pdf");

// Добавление изображения (заглушка)
ВнешняяКомпонента.AddImage("C:\image.jpg", 100, 100, 200, 150, 1);

// Сохранение PDF (заглушка)
ВнешняяКомпонента.SavePdf("C:\output.pdf");
```

## Доступные методы

### Свойства
- `IsEnabled` - Включена/выключена компонента
- `PdfFileName` - Имя PDF файла
- `ImageFileName` - Имя файла изображения
- `ImageX`, `ImageY` - Координаты изображения
- `ImageWidth`, `ImageHeight` - Размеры изображения
- `PageNumber` - Номер страницы
- `LastError` - Последняя ошибка

### Методы
- `LoadPdf(filename)` - Загрузка PDF (заглушка)
- `AddImage(imageFile, x, y, width, height, pageNum)` - Добавление изображения (заглушка)
- `SavePdf(filename)` - Сохранение PDF (заглушка)
- `GetPageSize(pageNum)` - Получение размера страницы
- `ClearImages()` - Очистка списка изображений
- `GetPageCount()` - Получение количества страниц
- `TestConnection()` - Тест соединения

## Тестирование

Запустите тестовый скрипт в 1С:

```1c
// Выполните файл test_simple_component.1c
```

## Ограничения упрощенной версии

⚠️ **Важно**: Это тестовая версия с заглушками!

- Методы `LoadPdf`, `AddImage`, `SavePdf` не выполняют реальных операций
- Всегда возвращают `Истина` для успешных операций
- Размер страницы всегда A4 (595x842 точек)
- Количество страниц всегда 1

## Диагностика проблем

### Ошибка "Invalid class string"
1. Проверьте регистрацию: `regsvr32 bin\PdfImageAddIn_Simple.dll`
2. Убедитесь, что используется 32-битная версия regsvr32
3. Проверьте ключи реестра в `HKEY_CLASSES_ROOT\Addin.PdfImageAddIn`

### Ошибка "Class not registered"
1. Проверьте, что DLL создана успешно
2. Убедитесь, что все зависимости доступны
3. Проверьте права администратора при регистрации

### COM не работает в 1С
1. Проверьте настройки безопасности 1С
2. Убедитесь, что COM-объекты разрешены
3. Проверьте работу с `Scripting.FileSystemObject`

## Следующие шаги

После успешного тестирования упрощенной версии можно:

1. **Добавить реальную функциональность** - интегрировать другую PDF библиотеку
2. **Использовать системные API** - для работы с изображениями
3. **Создать гибридную версию** - с опциональным PDFium
4. **Добавить веб-интеграцию** - для работы с PDF через API

## Файлы проекта

- `Source/PdfImageAddIn/PdfImageAddIn_Simple.h` - Заголовочный файл
- `Source/PdfImageAddIn/PdfImageAddIn_Simple.cpp` - Реализация
- `Source/PdfImageAddIn/PdfImageAddIn_Simple.rc` - Ресурсы
- `Source/PdfImageAddIn/PdfImageAddIn_Simple.def` - Экспорт функций
- `build_simple.bat` - Скрипт сборки
- `test_simple_component.1c` - Тест для 1С

## Лицензия

См. файл `LICENSE` в корне проекта. 