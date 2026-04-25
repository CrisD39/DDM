# Verificación End-to-End: Click → LPD Visualization

## 1️⃣ CLICK DEL BOTÓN (Frontend QML)

**Ubicación**: [botonera/DDM/PuntoProximoWorkspace.qml](../botonera/DDM/PuntoProximoWorkspace.qml#L324)

```qml
Button {
    text: "INICIAR"
    onClicked: ddmController.pppStart(selectedCalcIndex, trackAInput, trackBInput)
}
```

**Input**:
- `selectedCalcIndex`: 0-9 (int)
- `trackAInput`: 42 (track ID, puede ser "own_ship")
- `trackBInput`: 7 (track ID)

---

## 2️⃣ INVOCACIÓN DEL CONTROLADOR (Frontend C++)

**Ubicación**: [botonera/src/controller/protocol/ddmcontroller.cpp#L211](../botonera/src/controller/protocol/ddmcontroller.cpp#L211)

```cpp
void DDMController::pppStart(int calcIndex, int trackA, int trackB) {
    int slotNumber = calcIndex + 1;  // Convertir a 1-based
    QByteArray command = m_commandBuilder.buildPppStartCommand(slotNumber, trackA, trackB);
    emit sendBackendCommand(command);  // → Transport::send()
}
```

---

## 3️⃣ CONSTRUCTION DEL JSON (Frontend)

**Ubicación**: [botonera/src/controller/json/jsoncommandbuilder.cpp#L76](../botonera/src/controller/json/jsoncommandbuilder.cpp#L76)

```cpp
QByteArray JsonCommandBuilder::buildPppStartCommand(int calcIndex, int trackA, int trackB) {
    QJsonObject args;
    args["index"] = calcIndex;        // 1-based
    args["track_a"] = trackA;
    args["track_b"] = trackB;
    return buildCommand("cpa_start", args);
}
```

**JSON Enviado**:
```json
{
  "command": "cpa_start",
  "args": {
    "index": 1,
    "track_a": 42,
    "track_b": 7
  }
}
```

---

## 4️⃣ TRANSPORTE AL BACKEND

**Método**: LocalIPC (default) o UDP

**Puerto**: 6340 (UDP) | /siag_ddm (LocalIPC socket)

**Ruta**:
1. Frontend Transport → Backend Transport
2. Backend MessageRouter → detecta JSON
3. MessageRouter emite `jsonMessage(QByteArray)`
4. JsonCommandHandler slot conectado

---

## 5️⃣ PARSING EN EL BACKEND

**Ubicación**: [DesformatConcentrator/src/controller/json/jsoncommandhandler.cpp#L40](../src/controller/json/jsoncommandhandler.cpp#L40)
```cpp
void JsonCommandHandler::processJsonCommand(const QByteArray& jsonData) {
    QJsonObject obj;
    if (!parseJson(jsonData, obj)) return;
    
    const QString command = obj.value("command").toString();    // "cpa_start"
    const QJsonObject args = obj.value("args").toObject();      // {index:1, track_a:42, track_b:7}
    routeCommand(command, args);
}
```

---

## 6️⃣ ROUTING DEL COMANDO

**Ubicación**: [DesformatConcentrator/src/controller/json/jsoncommandhandler.cpp#L145](../src/controller/json/jsoncommandhandler.cpp#L145)
```cpp
void JsonCommandHandler::routeCommand(const QString& command, const QJsonObject& args) {
    auto it = m_commandMap.find("cpa_start");
    if (it != m_commandMap.end()) {
        sendResponse(it.value()(args));  // → handleCpaStart(args)
    }
}
```

---

## 7️⃣ HANDLER DE CPA_START

**Ubicación**: [DesformatConcentrator/src/controller/json/jsoncommandhandler.cpp#L268](../src/controller/json/jsoncommandhandler.cpp#L268)
```cpp
QByteArray JsonCommandHandler::handleCpaStart(const QJsonObject& args) {
    // Extrae: index=1, track_a=42, track_b=7
    const int calcIndex = args.value("index").toInt();  // 1
    
    CPATrackRef trackA, trackB;
    parseTrackPair(args, trackA, trackB, ...);  // Convierte 42→{isOwnShip:false, trackId:42}
    
    // Orquesta cálculo en servicio
    const CPAComputationResult result = m_cpaService->startCPA(trackA, trackB);
    
    // Devuelve respuesta con sessionId y resultados
    const QString sessionId = QStringLiteral("CPA_SLOT_%1").arg(calcIndex);  // "CPA_SLOT_1"
    // ... construye responseArgs
    return JsonResponseBuilder::buildSuccessResponse("cpa_start", responseArgs);
}
```

**Estructura de Respuesta**:
```json
{
  "status": "success",
  "command": "cpa_start",
  "args": {
    "index": 1,
    "id": "CPA_SLOT_1",
    "track_a": 42,
    "track_b": 7,
    "tcpa_sec": 120.5,
    "dcpa_dm": 0.5,
    "cpa_mid_x": 1000.0,
    "cpa_mid_y": 2000.0,
    "status": "active"
  }
}
```

---

## 8️⃣ SERVICIO CPA

**Ubicación**: [DesformatConcentrator/src/controller/services/cpaservice.cpp#L186](../src/controller/services/cpaservice.cpp#L186)
```cpp
CPAComputationResult CPAService::startCPA(const CPATrackRef& trackA, const CPATrackRef& trackB) {
    // 1. Valida que trackA y trackB sean válidos
    // 2. Resuelve de CommandContext: {xDm, yDm, speedDmPerHour, courseDeg}
    // 3. Calcula CPA con PppCalculator::compute()
    // 4. Almacena sesión: m_sessions["CPA_SLOT_1"] = { trackA, trackB, active:true }
    // 5. Retorna CPAComputationResult con {tcpaSeconds, dcpaDm, cpaMidX, cpaMidY}
}
```

---

## 9️⃣ ALMACENAMIENTO DE MARCADORES

**Ubicación**: [DesformatConcentrator/src/controller/json/jsoncommandhandler.cpp#L297](../src/controller/json/jsoncommandhandler.cpp#L297)
```cpp
QByteArray JsonCommandHandler::handlePppGraph(const QJsonObject& args) {
    const int calcIndex = args.value("calc_index").toInt();
    const QString sessionId = QStringLiteral("CPA_SLOT_%1").arg(calcIndex);
    
    const CPAComputationResult result = m_cpaService->graphCPA(sessionId);
    // graphCPA llama internamente a:
    //   computeCPA() → obtiene tcpa, dcpa, cpaMiddlePoint
    //   upsertMarker(sessionId, trackA, trackB, cpaMidX, cpaMidY)
    //     → m_context->upsertCpaMarker(marker)
}
```

**Marca Almacenada**:
```cpp
CommandContext::CpaMarkerState marker {
    sessionId = "CPA_SLOT_1",
    trackAId = 42,
    trackBId = 7,
    xDm = 1000.0f,      // Posición media X del CPA
    yDm = 2000.0f,      // Posición media Y del CPA
    visible = true
};
```

**Ubicación en memoria**: `CommandContext::cpaMarkers` (std::deque<CpaMarkerState>)

---

## 🔟 CODIFICACIÓN PARA LPD

**Ubicación**: [DesformatConcentrator/src/model/decoders/lpdEncoder.cpp#L109](../src/model/decoders/lpdEncoder.cpp#L109)
```cpp
QByteArray encoderLPD::buildFullMessage(const CommandContext &ctx) {
    // ... (procesa tracks, cursores, etc.)
    
    // CPA MARKERS (C3F07)
    for (const auto &marker : ctx.cpaMarkers) {
        if (marker.visible) {
            appendCpaMarkerMessage(bigBuffer, marker);
        }
    }
}

void encoderLPD::appendCpaMarkerMessage(QByteArray& dst, const CommandContext::CpaMarkerState& marker) {
    appendCoordinate(dst, marker.xDm, AB2_ID_X);      // X en DM
    appendCoordinate(dst, marker.yDm, AB2_ID_Y);      // Y en DM
    dst.append(static_cast<char>(0x26));              // ← SYMBOL BYTE CLAVE
    dst.append(static_cast<char>(0x00));              // Padding
    // ... resto de bytes AB2
    dst.append(static_cast<char>(EOMM));              // Fin del mensaje
}
```

**Estructura AB2 (LPD)**:
```
[Coordinate X] [Coordinate Y] [0x26] [0x00] [0x00] ... [EOMM]
                                ↑
                    "Éste es un símbolo C3F07"
```

---

## 1️⃣1️⃣ ENVÍO A FRONTEND (UDP/LocalIPC)

**Destinatario**: LPDWidget (y otros frontends)

**Frecuencia**: Cada 40ms (timer en main.cpp)

**Datos**: QByteArray con toda la escena LPD (incluida marca CPA)

---

## 1️⃣2️⃣ RECEPCIÓN Y DECODIFICACIÓN EN LPD

**Ubicación**: [botonera/src/view/lpdWidget.cpp](../botonera/src/view/lpdWidget.cpp)

```cpp
// LPDWidget recibe QByteArray del backend
void LPDWidget::paintGL() {
    // Procesa el mensaje AB2
    // Detecta byte 0x26 → "Esto es un símbolo C3F07"
    // Extrae coordenadas y dirige a ContactRenderer::drawSymbolC3F07()
}
```

---

## 1️⃣3️⃣ RENDERIZADO DEL SÍMBOLO

**Ubicación**: [botonera/src/model/entities/contactRenderer.cpp](../botonera/src/model/entities/contactRenderer.cpp)

```cpp
void ContactRenderer::drawSymbolC3F07(float xScreen, float yScreen, TacticalSymbol symbol) {
    // Renderiza el símbolo CPA/PPP:
    //  - Pequeño círculo o marca especial
    //  - En coordenadas (xScreen, yScreen) convertidas del DM
    //  - Visible en tiempo real en la pantalla LPD
}
```

---

## 📊 FLUJO RESUMIDO

```
[CLICK BOTÓN INICIAR]
        ↓
    pppStart(0, 42, 7)
        ↓
buildPppStartCommand()
        ↓
JSON: {"command":"cpa_start","args":{"index":1,"track_a":42,"track_b":7}}
        ↓
Transport (UDP/LocalIPC)
        ↓
Backend ProcessJsonCommand
        ↓
routeCommand("cpa_start", args)
        ↓
handleCpaStart(args)
        ↓
m_cpaService->startCPA(trackRef42, trackRef7)
        ↓
PppCalculator::compute() → TCPA, DCPA, midpoint
        ↓
m_context->upsertCpaMarker({sessionId:"CPA_SLOT_1", xDm:1000, yDm:2000})
        ↓
encoderLPD::buildFullMessage() → incluye marker con byte 0x26
        ↓
Transport → Frontend
        ↓
LPDWidget::paintGL()
        ↓
ContactRenderer::drawSymbolC3F07(xScreen, yScreen)
        ↓
[SÍMBOLO VISIBLE EN LPD] ✅
```

---

## ✅ CHECKLIST DE VALIDACIÓN

| Paso | Componente | Validación |
|------|-----------|-----------|
| 1 | QML Button | Click ejecuta `pppStart()` |
| 2 | DDMController | Construye JSON con index, track_a, track_b |
| 3 | JsonCommandBuilder | JSON tiene estructura correcta |
| 4 | Transport | Mensaje llega al backend |
| 5 | JsonCommandHandler | parseJson() extrae "cpa_start" |
| 6 | routeCommand() | Encuentra y ejecuta handleCpaStart() |
| 7 | handleCpaStart() | Parsea args, valida trackPair |
| 8 | CPAService | startCPA() calcula y almacena sesión |
| 9 | CommandContext | upsertCpaMarker() agrega marca a cpaMarkers |
| 10 | LPDEncoder | appendCpaMarkerMessage() emite byte 0x26 |
| 11 | Transport | Mensaje con marca enviado al frontend |
| 12 | LPDWidget | drawSymbolC3F07() renderiza marca |

---

## 🔴 PUNTOS CRÍTICOS A MONITOREAR

1. **Sesiones consistentes**: `calcIndex` → `sessionId` debe ser 1:1 en frontend y backend
2. **Track lookup**: CommandContext debe contener los tracks 42 y 7
3. **Coordenadas**: CPA mid-point debe convertirse correctamente de DM a pantalla
4. **Símbolo byte**: Debe ser exactamente `0x26` para llegar a drawSymbolC3F07
5. **Visibilidad**: CommandContext::cpaMarkers debe estar en el loop de LPDEncoder

---

## 🎯 PRUEBAS RECOMENDADAS

1. **Backend Unit Test**:
   ```bash
   echo '{"command":"cpa_start","args":{"index":1,"track_a":42,"track_b":7}}' | nc -u localhost 6340
   ```
   Esperado: respuesta JSON con "status":"success" y sessionId "CPA_SLOT_1"

2. **Frontend Integration Test**:
   - Click botón INICIAR con tracks 42 y 7
   - Verificar que LPD muestra un símbolo en el punto CPA calculado

3. **Protocol Trace**:
   - Monitor UDP port 6340
   - Buscar byte 0x26 en los mensajes AB2 enviados

---

## 📝 DOCUMENTOS RELACIONADOS

- [botonera/docs/CPA_PPP_FLOW_MAPPING.md](../botonera/docs/CPA_PPP_FLOW_MAPPING.md)
- [DesformatConcentrator/docs/README_BACKEND_ARCHITECTURE.md](./README_BACKEND_ARCHITECTURE.md)
- [botonera/docs/CPA_PPP_FRONTEND_SUMMARY.md](../botonera/docs/CPA_PPP_FRONTEND_SUMMARY.md)
