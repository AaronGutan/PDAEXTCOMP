@echo off
echo ========================================
echo Быстрый тест компиляции
echo ========================================

:: Создаем тестовый файл
echo #include ^<iostream^> > test_compile.cpp
echo using namespace std; >> test_compile.cpp
echo int main() { >> test_compile.cpp
echo     cout ^<^< "Компиляция работает!" ^<^< endl; >> test_compile.cpp
echo     return 0; >> test_compile.cpp
echo } >> test_compile.cpp

:: Проверяем компилятор
echo Проверка компилятора...
cl /? >NUL 2>&1
if errorlevel 1 (
    echo ❌ Компилятор cl не найден
    echo Попробуйте запустить:
    echo call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
    pause
    exit /b 1
) else (
    echo ✅ Компилятор cl найден
)

:: Тестовая компиляция
echo Тестовая компиляция...
cl /EHsc test_compile.cpp /Fe:test_compile.exe
if errorlevel 1 (
    echo ❌ Ошибка компиляции
    pause
    exit /b 1
) else (
    echo ✅ Компиляция прошла успешно
)

:: Запускаем тест
echo Запуск теста...
test_compile.exe
if errorlevel 1 (
    echo ❌ Ошибка выполнения
) else (
    echo ✅ Тест прошел успешно
)

:: Очистка
del test_compile.cpp >NUL 2>&1
del test_compile.exe >NUL 2>&1
del test_compile.obj >NUL 2>&1

echo.
echo ========================================
echo Тест компиляции завершен
echo ========================================
echo.
echo Теперь можно запустить build.bat для полной сборки
echo.
pause 