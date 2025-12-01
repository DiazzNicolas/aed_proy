@echo off
echo Iniciando servidor HTTP local...
echo.
echo Abre tu navegador en: http://localhost:8000
echo.
echo Presiona Ctrl+C para detener el servidor
echo.

cd /d "%~dp0"

REM Intentar Python 3 primero
python -m http.server 8000 2>nul
if %errorlevel% neq 0 (
    REM Si Python 3 falla, intentar Python 2
    python -m SimpleHTTPServer 8000 2>nul
    if %errorlevel% neq 0 (
        echo Error: Python no encontrado.
        echo.
        echo Por favor instala Python o usa otro metodo del README.md
        pause
    )
)

