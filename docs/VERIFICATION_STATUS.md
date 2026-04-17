# Resumen de la Verificación End-to-End: Click → LPD

## Estado Actual (Después de Refactor)

### ✅ COMPLETADO

#### Backend (DesformatConcentrator)

1. **JsonCommandHandler** (`jsoncommandhandler.cpp/h`)
   - ✅ `parseTrackRefValue()`: Convierte int/string → CPATrackRef
   - ✅ `parseTrackPair()`: Valida track_a y track_b
   - ✅ `handleCpaStart()`: Procesa `{"command":"cpa_start","args":{"index":N,"track_a":ID,"track_b":ID}}`
   - ✅ `handlePppGraph()`: Procesa `{"command":"ppp_graph","args":{"calc_index":N}}`
   - ✅ `handlePppFinish()`: Procesa `{"command":"ppp_finish","args":{"calc_index":N}}`
   - ✅ `handlePppClearTrack()`: Procesa `{"command":"ppp_clear_track","args":{"calc_index":N,"field":"track_a|track_b"}}`

2. **CPAService** (`cpaservice.cpp/h`)
   - ✅ `startCPA(trackA, trackB)`: Calcula CPA, almacena sesión, devuelve resultado
   - ✅ `graphCPA(sessionId)`: Grafica sesión existente, upserta marcador
   - ✅ `finishCPA(sessionId)`: Marca sesión como 
 inactiva, borra marcadores
   - ✅ `clearTrack(trackRef)`: Limpia todas las sesiones/marcadores del track
   - ✅ Almacenamiento de sesiones: `m_sessions["CPA_SLOT_N"]`

3. **CommandContext** (`commandContext.h`)
   - ✅ `CpaMarkerState` struct: sessionId, trackAId, trackBId, xDm, yDm, visible
   - ✅ `cpaMarkers` deque: Storage para marcadores
   - ✅ `upsertCpaMarker()`: Actualiza o inserta marcador
   - ✅ `eraseCpaMarkerBySessionId()`: Borra por sesión
   - ✅ `eraseCpaMarkersByTrackId()`: Borra todos los marcadores de un track

4. **LPDEncoder** (`lpdEncoder.cpp/h`)
   - ✅ `appendCpaMarkerMessage()`: Emite marker como AB2 con byte 0x26
   - ✅ Loop en `buildFullMessage()`: Procesa todos los cpaMarkers
   - ✅ Symbol byte `0x26` configurado para `ContactRenderer::drawSymbolC3F07`

5. **Main Loop** (`main.cpp`)
   - ✅ QTimer cada 40ms: Invoca `encoder->buildFullMessage(*ctx)`
   - ✅ Transport: Envía mensaje a frontend

#### Frontend (botonera)

1. **QML Buttons** (`DDM/PuntoProximoWorkspace.qml`)
   - ✅ Button INICIAR: `ddmController.pppStart(selectedCalcIndex, trackA, trackB)`
   - ✅ Button FINALIZAR: `ddmController.pppFinish(selectedCalcIndex)`
   - ✅ Button GRAFICAR: `ddmController.pppGraph(selectedCalcIndex, enabled)`
   - ✅ Button LIMPIAR: `ddmController.pppClearTrack(selectedCalcIndex, field)`

2. **DDMController** (`protocol/ddmcontroller.cpp`)
   - ✅ `pppStart()`: Construye JSON y emite `sendBackendCommand`
   - ✅ `pppGraph()`: Construye JSON y emite `sendBackendCommand`
   - ✅ `pppFinish()`: Construye JSON y emite `sendBackendCommand`
   - ✅ `pppClearTrack()`: Construye JSON y emite `sendBackendCommand`

3. **JsonCommandBuilder** (`json/jsoncommandbuilder.cpp`)
   - ✅ `buildPppStartCommand()`: `{index, track_a, track_b}`
   - ✅ `buildPppGraphCommand()`: `{calc_index, enabled}`
   - ✅ `buildPppFinishCommand()`: `{calc_index}`
   - ✅ `buildPppClearTrackCommand()`: `{calc_index, field}`

4. **Transport** (`injector.cpp`)
   - ✅ `sendBackendCommand` conectado a `ITransport::send()`
   - ✅ `jsonMessage` conectado a `DDMController::handleBackendResponse()`

### ⚠️ POR VALIDAR

| Punto | Descripción | Prioridad | Acción |
|-------|-----------|----------|--------|
| 1 | **Compilación**: Verificar que no hay errores de compilación | 🔴 CRÍTICA | Ejecutar `qmake` + `make` |
| 2 | **Tracks disponibles**: Backend debe tener tracks 42 y 7 en CommandContext | 🔴 CRÍTICA | Crear tracks antes de iniciar CPA |
| 3 | **OwnShip**: Si track_a es "own_ship", verificar que `CommandContext::ownShip.valid == true` | 🟡 IMPORTANTE | Validar ownship state |
| 4 | **SessionId mapping**: Verificar que `"CPA_SLOT_1"` es consistente frontend↔backend | 🟡 IMPORTANTE | Monitor de sesiones |
| 5 | **Symbol byte 0x26**: Verificar que llega al frontend correactamente | 🟡 IMPORTANTE | Wireshark capture UDP 6340 |
| 6 | **LPDWidget renderizado**: `drawSymbolC3F07()` debe renderizar la marca | 🟡 IMPORTANTE | Visual en LPD |
| 7 | **Response parsing**: Frontend parsea respuestas CPA correctamente | 🟡 IMPORTANTE | DDMController::handleBackendResponse |

### 🧪 PLAN DE PRUEBA

#### Test 1: Backend Unit (Sin LPD)

```bash
# Terminal 1: Backend
./DDM

# Terminal 2: Enviar comando JSON
echo '{"command":"cpa_start","args":{"index":1,"track_a":42,"track_b":7}}' \
  | nc -u localhost 6340

# Esperado: Respuesta JSON con status:"success" e id:"CPA_SLOT_1"
```

**Validar**:
- ✅ Backend parsea JSON correctamente
- ✅ Encuentra tracks 42 y 7
- ✅ Calcula CPA (tcpa_sec, dcpa_dm, cpa_mid_x, cpa_mid_y)
- ✅ Almacena sesión con sessionId "CPA_SLOT_1"
- ✅ Marcador en CommandContext::cpaMarkers

#### Test 2: LPD Encoder

```cpp
// En main.cpp, después de crear encoder, log:
qDebug() << ctx->cpaMarkers.size() << "CPA markers";

for (const auto& m : ctx->cpaMarkers) {
    qDebug() << "Marker:" << m.sessionId << "@" << m.xDm << m.yDm;
}
```

**Validar**:
- ✅ cpaMarkers no está vacío después de pppGraph
- ✅ Coordenadas son válidas (no NaN, rango razonable)

#### Test 3: LPD Packet Capture

```bash
# Ejecutar Wireshark
# Filter: udp.dstport == 6340
# Buscar byte 0x26 en los payloads AB2
```

**Validar**:
- ✅ Byte 0x26 presente en mensajes
- ✅ Coordenadas X,Y antes del 0x26

#### Test 4: Frontend Integration

1. Iniciar botonera.exe
2. Click INICIAR con tracks "42" y "7"
3. Visualizar respuesta en UI
4. Click GRAFICAR
5. Verificar símbolo en LPD

**Validar**:
- ✅ LPD muestra símbolo C3F07 en punto calculado
- ✅ Símbolo desaparece al FINALIZAR
- ✅ LIMPIAR borra todos los marcadores

---

## 🔴 POSIBLES PROBLEMAS

### Problema 1: Tracks no existen
**Síntoma**: Backend responde con error "track_not_found"
**Causa**: Tracks 42 y 7 no se crearon antes
**Solución**: Crear tracks con `create_track` antes de iniciar CPA

### Problema 2: OwnShip no válido
**Síntoma**: Error "own_ship_not_set" cuando track_a es "own_ship"
**Causa**: `CommandContext::ownShip.valid == false`
**Solución**: Ejecutar `ownship_update` con coordenadas válidas antes

### Problema 3: SessionId mismatch
**Síntoma**: `ppp_graph` responde con "session_not_found"
**Causa**: Frontend usa `calc_index`, backend espera diferente formato
**Solución**: Ambos deben construir sessionId = "CPA_SLOT_" + index

### Problema 4: Símbolo no aparece en LPD
**Síntoma**: LPD actualiza pero no muestra marca
**Causa**: Byte 0x26 no llega al frontend, o ContactRenderer no lo renderiza
**Solución**: Verificar Wireshark + logs en LPDWidget::paintGL

### Problema 5: Compilación falla
**Síntoma**: Error de linkage o undefined reference
**Causa**: qmake no regeneró Makefile después de cambios
**Solución**: `qmake DDM.pro && make clean && make`

---

## 📋 RESUMEN DE CAMBIOS

### Backend

| Archivo | Cambio | Líneas |
|---------|--------|---------|
| jsoncommandhandler.h | Forward decl CPAService, struct CPATrackRef | +2 |
| jsoncommandhandler.cpp | 4 handlers + 2 parse helpers | +200 |
| cpaservice.h | Ampliada con CPATrackRef, CPASession, etc. | +40 |
| cpaservice.cpp | Reescrita con session management | +250 |
| commandContext.h | CpaMarkerState + marker methods | +40 |
| lpdEncoder.h | appendCpaMarkerMessage() | +1 |
| lpdEncoder.cpp | Loop cpaMarkers + appendCpaMarkerMessage | +30 |

**Total Backend**: ~563 líneas de código nuevo/modificado

### Frontend

| Archivo | Cambio | Estado |
|---------|--------|--------|
| DDM/PuntoProximoWorkspace.qml | Botones CPA/PPP | ✅ Ya existía |
| ddmcontroller.cpp | pppStart/pppGraph/pppFinish/pppClearTrack | ✅ Ya existía |
| jsoncommandbuilder.cpp | Builders para 4 comandos | ✅ Ya existía |

**Total Frontend**: Todos los componentes ya estaban presentes

---

## ✅ PRÓXIMOS PASOS

1. **Compilar y linkear** DesformatConcentrator
2. **Test Backend**: Enviar JSON, verificar respuesta
3. **Test Encoder**: Verificar markers en CommandContext
4. **Test Transport**: Wireshark en UDP 6340
5. **Test Frontend**: Click botón → Visualización en LPD
6. **Debugging**: Agregar qDebug statements según sea necesario

---

## 📚 DOCUMENTACIÓN GENERADA

- ✅ `CPA_PPP_END_TO_END_VERIFICATION.md` - Flujo detallado paso a paso
- ✅ `test_end_to_end.sh` - Script de validación
- ✅ Este documento - Resumen de estado actual
