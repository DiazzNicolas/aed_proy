@echo off
echo 🚀 Iniciando servidor HTTP local...
echo.
echo 📡 Abre tu navegador en: http://localhost:8000
echo.
echo ⛔ Presiona Ctrl+C para detener el servidor
echo.

cd /d "%~dp0"

REM Intentar Python 3
python --version >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ Usando Python...
    python -m http.server 8000
    goto :end
)

REM Intentar Python 2
python -m SimpleHTTPServer 8000 >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ Usando Python 2...
    python -m SimpleHTTPServer 8000
    goto :end
)

REM Intentar PHP
php --version >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ Usando PHP...
    php -S localhost:8000
    goto :end
)

REM Intentar Node.js
where npx >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ Usando Node.js (npx http-server)...
    npx --yes http-server -p 8000
    goto :end
)

REM Si ninguno funciona
echo ❌ Error: No se encontró ningún servidor HTTP disponible.
echo.
echo Por favor instala uno de los siguientes:
echo   • Python 3:  https://www.python.org/downloads/
echo   • PHP:       https://www.php.net/downloads
echo   • Node.js:   https://nodejs.org/
echo.
pause
goto :end

:end