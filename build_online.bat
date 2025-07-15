@echo off
echo ========================================
echo Онлайн сборка PdfImageAddIn
echo ========================================

echo Проверяю доступные инструменты...

:: Проверяем наличие компилятора
cl /? >NUL 2>&1
if not errorlevel 1 (
    echo ✅ Visual Studio найден
    goto :build_local
)

echo ❌ Локальные инструменты сборки не найдены
echo.
echo 📋 Доступные варианты:
echo.
echo 1. GitHub Actions (рекомендуется)
echo    - Создайте репозиторий на GitHub
echo    - Загрузите весь проект
echo    - Сборка произойдет автоматически
echo.
echo 2. Установка Visual Studio Build Tools
echo    - Запустите: install_build_tools.bat
echo.
echo 3. Онлайн IDE
echo    - GitHub Codespaces
echo    - Gitpod
echo    - Replit
echo.
echo 4. Тестирование компиляции
echo    - Откройте: online_compiler_test.html
echo.

set /p choice="Выберите вариант (1-4) или Enter для GitHub Actions: "

if "%choice%"=="1" goto :github_actions
if "%choice%"=="2" goto :install_tools
if "%choice%"=="3" goto :online_ide
if "%choice%"=="4" goto :test_page
if "%choice%"=="" goto :github_actions

:github_actions
echo.
echo 🚀 Настройка GitHub Actions:
echo.
echo 1. Создайте новый репозиторий на GitHub
echo 2. Загрузите туда все файлы проекта
echo 3. Перейдите в Actions → Build PdfImageAddIn
echo 4. Нажмите "Run workflow"
echo 5. Скачайте артефакты после завершения
echo.
echo Открываю GitHub...
start https://github.com/new
goto :end

:install_tools
echo.
echo 🔧 Установка Build Tools...
if exist "install_build_tools.bat" (
    install_build_tools.bat
) else (
    echo ❌ Файл install_build_tools.bat не найден
)
goto :end

:online_ide
echo.
echo 💻 Онлайн IDE:
echo.
echo 1. GitHub Codespaces: https://github.com/features/codespaces
echo 2. Gitpod: https://gitpod.io
echo 3. Replit: https://replit.com
echo.
echo Открываю GitHub Codespaces...
start https://github.com/features/codespaces
goto :end

:test_page
echo.
echo 🧪 Открываю страницу тестирования...
if exist "online_compiler_test.html" (
    start online_compiler_test.html
) else (
    echo ❌ Файл online_compiler_test.html не найден
)
goto :end

:build_local
echo.
echo 🔨 Запуск локальной сборки...
call build.bat
goto :end

:end
echo.
echo 📖 Документация по онлайн сборке: ONLINE_BUILD.md
echo.
pause 