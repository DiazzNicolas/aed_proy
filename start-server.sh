#!/bin/bash

echo "🚀 Iniciando servidor HTTP local..."
echo ""
echo "📡 Abre tu navegador en: http://localhost:8000"
echo ""
echo "⛔ Presiona Ctrl+C para detener el servidor"
echo ""

# Cambiar al directorio del script
cd "$(dirname "$0")"

# Función para verificar si un comando existe
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Intentar Python 3
if command_exists python3; then
    echo "✅ Usando Python 3..."
    python3 -m http.server 8000
    exit 0
fi

# Intentar Python (puede ser 2 o 3 dependiendo del sistema)
if command_exists python; then
    echo "✅ Usando Python..."
    python -m http.server 8000 2>/dev/null || python -m SimpleHTTPServer 8000
    exit 0
fi

# Intentar PHP
if command_exists php; then
    echo "✅ Usando PHP..."
    php -S localhost:8000
    exit 0
fi

# Intentar Node.js con http-server
if command_exists npx; then
    echo "✅ Usando Node.js (npx http-server)..."
    npx --yes http-server -p 8000
    exit 0
fi

# Intentar Ruby
if command_exists ruby; then
    echo "✅ Usando Ruby..."
    ruby -run -e httpd . -p 8000
    exit 0
fi

# Si ninguno funciona
echo "❌ Error: No se encontró ningún servidor HTTP disponible."
echo ""
echo "Por favor instala uno de los siguientes:"
echo "  • Python 3:  https://www.python.org/downloads/"
echo "  • PHP:       https://www.php.net/downloads"
echo "  • Node.js:   https://nodejs.org/"
echo "  • Ruby:      https://www.ruby-lang.org/"
echo ""
read -p "Presiona Enter para salir..."
exit 1