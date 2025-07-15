@echo off
echo ========================================
echo Установка Visual Studio Build Tools
echo ========================================

:: Проверяем, есть ли уже компилятор
cl /? >NUL 2>&1
if not errorlevel 1 (
    echo ✅ Visual Studio Build Tools уже установлены
    goto :build
)

echo Скачивание Visual Studio Build Tools...
powershell -Command "Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_buildtools.exe' -OutFile 'vs_buildtools.exe'"

if exist "vs_buildtools.exe" (
    echo Запуск установки Build Tools...
    echo.
    echo ВАЖНО: В установщике выберите:
    echo - C++ build tools
    echo - Windows 10/11 SDK
    echo - MSVC v143 - VS 2022 C++ x64/x86 build tools
    echo.
    
    vs_buildtools.exe --quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows10SDK.19041 --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64
    
    if errorlevel 1 (
        echo ❌ Ошибка установки Build Tools
        pause
        exit /b 1
    )
    
    echo ✅ Build Tools установлены
    del vs_buildtools.exe
) else (
    echo ❌ Не удалось скачать Build Tools
    pause
    exit /b 1
)

:build
echo.
echo Попробуем собрать проект...
.\build.bat

pause 