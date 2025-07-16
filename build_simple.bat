@echo off
echo Сборка упрощенной компоненты PdfImageAddIn (без PDFium)...

REM Проверяем наличие Visual Studio
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo Настройка окружения Visual Studio...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" 2>nul
    if %errorlevel% neq 0 (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" 2>nul
        if %errorlevel% neq 0 (
            call "C:\Program Files (x86)\Microsoft Visual Studio\2015\Community\VC\bin\vcvars32.bat" 2>nul
            if %errorlevel% neq 0 (
                echo Ошибка: Не найден Visual Studio!
                pause
                exit /b 1
            )
        )
    )
)

echo Создание директорий...
if not exist "bin" mkdir bin
if not exist "lib" mkdir lib

echo Компиляция базовой библиотеки AddIn...
cd Source\Addin
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL *.cpp
if %errorlevel% neq 0 (
    echo Ошибка компиляции AddIn!
    pause
    exit /b 1
)

echo Создание библиотеки AddIn...
lib /OUT:..\..\lib\addin.lib *.obj
if %errorlevel% neq 0 (
    echo Ошибка создания библиотеки AddIn!
    pause
    exit /b 1
)
cd ..\..

echo Компиляция упрощенной компоненты...
cd Source\PdfImageAddIn
cl /c /EHsc /DWIN32 /D_WINDOWS /D_USRDLL /I..\..\include /I..\Addin PdfImageAddIn_Simple.cpp MyClassFactory.cpp
if %errorlevel% neq 0 (
    echo Ошибка компиляции PdfImageAddIn_Simple!
    pause
    exit /b 1
)

echo Создание DLL...
link /DLL /OUT:..\..\bin\PdfImageAddIn_Simple.dll /DEF:PdfImageAddIn_Simple.def PdfImageAddIn_Simple.obj MyClassFactory.obj ..\..\lib\addin.lib ole32.lib oleaut32.lib uuid.lib
if %errorlevel% neq 0 (
    echo Ошибка создания DLL!
    pause
    exit /b 1
)
cd ..\..

echo Очистка временных файлов...
del /q Source\Addin\*.obj 2>nul
del /q Source\PdfImageAddIn\*.obj 2>nul

echo Регистрация компоненты...
regsvr32 /s bin\PdfImageAddIn_Simple.dll
if %errorlevel% neq 0 (
    echo Предупреждение: Не удалось зарегистрировать компоненту автоматически.
    echo Зарегистрируйте вручную: regsvr32 bin\PdfImageAddIn_Simple.dll
)

echo.
echo ========================================
echo Сборка завершена успешно!
echo ========================================
echo.
echo Файлы созданы:
echo - bin\PdfImageAddIn_Simple.dll
echo.
echo Для использования в 1С:
echo ВнешняяКомпонента = Новый COMОбъект("Addin.PdfImageAddIn");
echo.
echo Компонента работает в упрощенном режиме (заглушки).
echo.

pause 