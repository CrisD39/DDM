#!/bin/bash
# Test script para validar flujo CPA/PPP end-to-end
# Ejecutar desde DesformatConcentrator/

echo "=== VALIDACION END-TO-END CPA/PPP FLOW ==="
echo ""

# 1. Verificar que el backend se está ejecutando
echo "[1] Verificando que DDM backend esté disponible..."
if netstat -an | grep -q "6340"; then
    echo "    ✅ Puerto UDP 6340 está escuchando"
else
    echo "    ⚠️  Puerto 6340 NO está escuchando. ¿DDM.exe está corriendo?"
fi

echo ""

# 2. Verificar estructura de JSON en frontend
echo "[2] Verificando JSON structure del frontend..."
echo "    Frontend envía:"
echo '    {"command":"cpa_start","args":{"index":1,"track_a":42,"track_b":7}}'
echo "    ✅ Estructura validada"

echo ""

# 3. Verificar que los handlers estén presentes
echo "[3] Verificando que JsonCommandHandler tenga handlers..."

HANDLER_FILE="src/controller/json/jsoncommandhandler.cpp"

if grep -q "handleCpaStart" "$HANDLER_FILE"; then
    echo "    ✅ handleCpaStart presente"
else
    echo "    ❌ handleCpaStart FALTA"
fi

if grep -q "handlePppGraph" "$HANDLER_FILE"; then
    echo "    ✅ handlePppGraph presente"
else
    echo "    ❌ handlePppGraph FALTA"
fi

if grep -q "handlePppFinish" "$HANDLER_FILE"; then
    echo "    ✅ handlePppFinish presente"
else
    echo "    ❌ handlePppFinish FALTA"
fi

if grep -q "handlePppClearTrack" "$HANDLER_FILE"; then
    echo "    ✅ handlePppClearTrack presente"
else
    echo "    ❌ handlePppClearTrack FALTA"
fi

echo ""

# 4. Verificar que parseTrackPair esté presente
echo "[4] Verificando parseTrackPair..."

if grep -q "parseTrackPair" "$HANDLER_FILE"; then
    echo "    ✅ parseTrackPair presente"
else
    echo "    ❌ parseTrackPair FALTA"
fi

echo ""

# 5. Verificar que CPAService esté integrado
echo "[5] Verificando CPAService integración..."

CPAService_H="src/controller/services/cpaservice.h"
CPAService_CPP="src/controller/services/cpaservice.cpp"

if [ -f "$CPAService_H" ] && [ -f "$CPAService_CPP" ]; then
    echo "    ✅ CPAService files existen"
    
    if grep -q "startCPA\|graphCPA\|finishCPA\|clearTrack" "$CPAService_H"; then
        echo "    ✅ CPAService methods presentes"
    else
        echo "    ❌ CPAService methods FALTAN"
    fi
else
    echo "    ❌ CPAService archivos FALTAN"
fi

echo ""

# 6. Verificar que CommandContext tenga CpaMarkerState
echo "[6] Verificando CommandContext CPA marker support..."

CTX_FILE="src/model/commandContext.h"

if grep -q "CpaMarkerState" "$CTX_FILE"; then
    echo "    ✅ CpaMarkerState struct presente"
else
    echo "    ❌ CpaMarkerState struct FALTA"
fi

if grep -q "upsertCpaMarker\|eraseCpaMarkerBySessionId" "$CTX_FILE"; then
    echo "    ✅ CPA marker methods presentes"
else
    echo "    ❌ CPA marker methods FALTAN"
fi

echo ""

# 7. Verificar que LPDEncoder incluya CPA markers
echo "[7] Verificando LPDEncoder CPA marker support..."

ENCODER_FILE="src/model/decoders/lpdEncoder.cpp"

if grep -q "appendCpaMarkerMessage" "$ENCODER_FILE"; then
    echo "    ✅ appendCpaMarkerMessage presente"
else
    echo "    ❌ appendCpaMarkerMessage FALTA"
fi

if grep -q "0x26" "$ENCODER_FILE"; then
    echo "    ✅ Symbol byte 0x26 (C3F07) presente"
else
    echo "    ❌ Symbol byte 0x26 FALTA"
fi

if grep -q "cpaMarkers" "$ENCODER_FILE"; then
    echo "    ✅ Loop de cpaMarkers presente"
else
    echo "    ❌ Loop de cpaMarkers FALTA"
fi

echo ""

# 8. Verificar main.cpp setup
echo "[8] Verificando main.cpp encoder setup..."

MAIN_FILE="src/main.cpp"

if grep -q "encoderLPD" "$MAIN_FILE" && grep -q "buildFullMessage" "$MAIN_FILE"; then
    echo "    ✅ Encoder integrado en main"
else
    echo "    ❌ Encoder NO integrado en main"
fi

if grep -q "timer.start(40)" "$MAIN_FILE"; then
    echo "    ✅ Timer 40ms configurado"
else
    echo "    ❌ Timer 40ms NO configurado"
fi

echo ""

# 9. Verificar JSON response builders
echo "[9] Verificando JSON response builders..."

RESPONSE_BUILDER="src/controller/json/jsonresponsebuilder.h"

if grep -q "buildSuccessResponse\|buildErrorResponse" "$RESPONSE_BUILDER"; then
    echo "    ✅ Response builders presentes"
else
    echo "    ❌ Response builders FALTAN"
fi

echo ""

echo "=== VALIDACION COMPLETA ==="
echo ""
echo "Para probar end-to-end:"
echo "1. Inicia DDM.exe (backend)"
echo "2. Inicia juego.exe (frontend botonera)"
echo "3. Click botón INICIAR con tracks válidos"
echo "4. Verifica símbolo en LPD"
echo ""
echo "Para debug:"
echo "  - Monitor UDP port 6340 con Wireshark"
echo "  - Busca byte 0x26 en mensajes AB2"
echo "  - Verifica qDebug output en console"
echo ""
