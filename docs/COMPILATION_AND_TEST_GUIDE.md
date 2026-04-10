# INSTRUCCIONES DE COMPILACIÓN Y PRUEBA END-TO-END

## 🔵 Paso 1: Compilar el Backend (DesformatConcentrator)

```powershell
cd "C:\Users\jruma\OneDrive\Documentos\SIAG\DesformatConcentrator"

# Regenerar Makefile con qmake
qmake DDM.pro

# Compilar (usar mingw32-make en Windows con MinGW)
mingw32-make clean
mingw32-make

# Resultado esperado:
# Executable: build/Desktop_Qt_6_7_3_MinGW_64_bit-Debug/debug/DDM.exe
```

### Archivos Modificados en Backend:
- ✅ `src/controller/json/jsoncommandhandler.cpp` (+50 líneas en handlers)
- ✅ `src/controller/json/jsoncommandhandler.h` (agregó CPAService, CPATrackRef fwd decl)
- ✅ `src/controller/services/cpaservice.cpp` (reescrita completamente)
- ✅ `src/controller/services/cpaservice.h` (ampliada con nuevas structs)
- ✅ `src/model/commandContext.h` (agregó CpaMarkerState y methods)
- ✅ `src/model/decoders/lpdEncoder.cpp` (agregó loop de cpaMarkers)
- ✅ `src/model/decoders/lpdEncoder.h` (forward decl appendCpaMarkerMessage)

---

## 🔵 Paso 2: Compilar el Frontend (botonera)

```powershell
cd "C:\Users\jruma\OneDrive\Documentos\SIAG\tdc-botonera\botonera"

# Regenerar Makefile
qmake juego.pro

# Compilar
mingw32-make clean
mingw32-make

# Resultado esperado:
# Executable: build/Desktop_Qt_6_7_3_MinGW_64_bit-Debug/debug/juego.exe
```

### Nota:
El frontend **NO requiere cambios** en esta tanda. Los builders JSON ya existían:
- `buildPppStartCommand()` → ✅ Ya presente
- `buildPppGraphCommand()` → ✅ Ya presente
- `buildPppFinishCommand()` → ✅ Ya presente
- `buildPppClearTrackCommand()` → ✅ Ya presente

---

## 🧪 Paso 3: Test Backend Unit (Sin Frontend)

**Terminal 1: Ejecutar Backend**
```powershell
cd "C:\Users\jruma\OneDrive\Documentos\SIAG\DesformatConcentrator\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug"

.\DDM.exe
# Esperado: Backend escuchando en UDP port 6340
```

**Terminal 2: Enviar Comando JSON**
```powershell
# Primero, crear un track para que exista en el backend
# En la consola del DDM.exe, ejecutar:
# > create_track 42 1000 2000 100 45
# Esto crea track ID 42 en posición (1000,2000) con velocidad 100 knots a curso 45°

# Después, desde PowerShell, enviar comando JSON:
$json = '{"command":"cpa_start","args":{"index":1,"track_a":42,"track_b":7}}'
$bytes = [System.Text.Encoding]::UTF8.GetBytes($json)

$client = New-Object System.Net.Sockets.UdpClient
$client.Send($bytes, $bytes.Length, "127.0.0.1", 6340) | Out-Null
$client.Close()

Write-Host "Enviado: $json"
```

**Validar Respuesta**: En consola del DDM.exe debe aparecer:
```
[JsonCommandHandler] Comando cpa_start recibido
Backend debería logear los cálculos de CPA
```

---

## 🎮 Paso 4: Test Frontend Integration

**Prerequisites**:
1. Backend (DDM.exe) corriendo
2. Frontend (botonera.exe/juego.exe) compilado y listo

**Procedimiento**:

1. **Iniciar Backend**
   ```powershell
   .\DDM.exe
   # Esperar "Backend listening on UDP 6340"
   ```

2. **Crear Tracks en Backend** (por consola del backend)
   ```
   > create_track 42 1000 2000 100 45
   > create_track 7 1500 2500 80 90
   > list_tracks
   # Verificar que aparecen los 2 tracks
   ```

3. **Iniciar Frontend**
   ```powershell
   .\juego.exe
   # Debe abrir ventana de botonera con panels
   ```

4. **Navegar a Panel PPP/CPA**
   - En botonera, seleccionar "Punto Próximo" (PuntoProximoWorkspace)
   - O ir a menu donde ves los buttons

5. **Seleccionar Tracks**
   - Track A: 42 (o seleccionar "own_ship" si está configurado)
   - Track B: 7
   - Calc Index: 0 (primer slot)

6. **Click "INICIAR"** (cpa_start)
   - **Esperado**: 
     - UI muestra respuesta JSON con id:"CPA_SLOT_0", tcpa_sec, dcpa_dm, etc.
     - Backend log muestra cálculos CPA
     - LPD panel actualiza (si está visible)

7. **Click "GRAFICAR"** (ppp_graph)
   - **Esperado**:
     - 🔴 **AQUÍ DEBE APARECER SÍMBOLO EN LPD** ← PUNTO CRÍTICO
     - Un pequeño símbolo/marca en el punto de CPA calculado
     - Symbol tipo C3F07

8. **Click "FINALIZAR"** (ppp_finish)
   - **Esperado**: Símbolo desaparece de LPD

9. **Click "LIMPIAR"** (ppp_clear_track)
   - **Esperado**: Todas las sesiones del track se limpian

---

## 🔍 Paso 5: Debug & Monitoring

### Monitor UDP Traffic
```powershell
# Instalar Wireshark si no lo tienes
# Aplicar filter: udp.dstport == 6340

# Buscar byte 0x26 en los payloads AB2 (CPA marker symbol)
# Debe aparecer cuando das click en GRAFICAR
```

### Habilitar Debug Logging en Backend
Agregar a `src/main.cpp` antes del timer loop:
```cpp
// Debug: Log CPA markers cada tick
QObject::connect(&timer, &QTimer::timeout, [ctx]() {
    if (!ctx->cpaMarkers.empty()) {
        qDebug() << "[CPA] Found" << ctx->cpaMarkers.size() << "markers:";
        for (const auto& m : ctx->cpaMarkers) {
            qDebug() << "  - Session:" << m.sessionId 
                     << "@ (" << m.xDm << "," << m.yDm << ")";
        }
    }
});
```

### Verificar CommandContext Estado
```cpp
// En jsoncommandhandler.cpp, agregar en handleCpaStart():
qDebug() << "[CPA] Starting CPA for:";
qDebug() << "  Track A:" << (trackA.isOwnShip ? "OwnShip" : QString::number(trackA.trackId));
qDebug() << "  Track B:" << QString::number(trackB.trackId);
qDebug() << "  Result valid:" << result.valid;
qDebug() << "  SessionId:" << sessionId;
```

---

## 🎯 CHECKLIST DE VALIDACIÓN FINAL

### ✅ Compilación
- [ ] `qmake DDM.pro && mingw32-make` sin errores
- [ ] Ejecutable `DDM.exe` generado
- [ ] `qmake juego.pro && mingw32-make` sin errores (frontend)
- [ ] Ejecutable `juego.exe` generado

### ✅ Backend Unit Test
- [ ] Backend inicia sin crashes
- [ ] Crea tracks correctamente
- [ ] JSON cpa_start genera respuesta válida
- [ ] SessionId es "CPA_SLOT_N" (donde N = index)

### ✅ LPD Marker Storage
- [ ] CommandContext::cpaMarkers no vacío después de graphCPA
- [ ] Marcador tiene coordenadas válidas (no NaN, rango 0-3000 DM típico)
- [ ] sessionId almacenado correctamente

### ✅ LPD Encoding
- [ ] Byte 0x26 presente en payload UDP
- [ ] Coordenadas X,Y empaquetadas correctamente antes de 0x26
- [ ] Mensaje enviado cada 40ms

### ✅ Frontend Integration
- [ ] Botón INICIAR genera JSON correcto
- [ ] Tabla de ppp_results muestra respuesta
- [ ] Símbolo aparece en LPD en coordenadas calculadas
- [ ] Símbolo desaparece al FINALIZAR
- [ ] LIMPIAR elimina todas las marcas

---

## 🚨 TROUBLESHOOTING

### Problema: "Session not found" en ppp_graph
- **Causa**: sessionId mismatch entre frontend y backend
- **Fix**: Ambos deben usar `"CPA_SLOT_" + calcIndex`
- **Verificar**: qDebug output de sessionId en ambos lados

### Problema: Símbolo no aparece en LPD
- **Causa 1**: cpaMarkers vacío (graphCPA no fue llamado)
- **Causa 2**: Byte 0x26 no llega al frontend
- **Causa 3**: ContactRenderer no tiene implementado drawSymbolC3F07
- **Fix**: Verificar en este orden: qDebug markers → Wireshark → LPD code

### Problema: "Track not found"
- **Causa**: Backend no tiene tracks 42 y 7
- **Fix**: `create_track 42 X Y SPEED COURSE` antes de iniciar CPA

### Problema: Compilación falla
- **Causa**: Qt includes path no configurado
- **Fix**: Verificar DDM.pro tiene `QT += core gui network opengl`

---

## 📊 RESULTADO ESPERADO

Cuando todo funciona correctamente, el flujo es:

1. **Click Botón INICIAR** → Símbolo aparece en LPD
2. **Click Botón GRAFICAR** → Símbolo actualiza (re-graficado)
3. **Click Botón FINALIZAR** → Símbolo desaparece
4. **Click Botón LIMPIAR** → Todas las sesiones del track se eliminan

---

## 📝 ARCHIVOS DE REFERENCIA

- ✅ [CPA_PPP_END_TO_END_VERIFICATION.md](./CPA_PPP_END_TO_END_VERIFICATION.md) - Flujo detallado
- ✅ [VERIFICATION_STATUS.md](./VERIFICATION_STATUS.md) - Estado actual
- ✅ [test_end_to_end.sh](./test_end_to_end.sh) - Script de validación
- ✅ [README_BACKEND_ARCHITECTURE.md](./README_BACKEND_ARCHITECTURE.md) - Arquitectura completa
