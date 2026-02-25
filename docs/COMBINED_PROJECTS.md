# DesformatConcentrator + tdc-botonera — Combined Project Documentation

Este documento reúne la información esencial de los dos proyectos del workspace: el backend C++/Qt (DesformatConcentrator / DDM) y la interfaz QML (tdc-botonera / Botonera). Está pensado como referencia rápida para desarrolladores que trabajan en ambas partes.

---

## Resumen ejecutivo
- Backend: "DesformatConcentrator" / DDM — Qt6 C++17, procesa datos binarios del concentrador (DCL), expone comandos JSON y una consola (stdin) con comandos tipo `sitrep`, `add`, `delete`.
- Frontend: "tdc-botonera" — QML UI (Botonera / DDM) que envía/recibe JSON vía `ITransport` y muestra el SITREP en pantalla.

El flujo principal: Mensajes entrantes → `MessageRouter` (backend) → Handlers (JSON/binary) → `CommandContext` (estado) → encoder (salida hardware) y JSON responses → frontend.

---

## Estructura de carpetas (resumen)
- DesformatConcentrator/
  - DDM.pro
  - docs/
    - architecture.md
    - README_BACKEND_ARCHITECTURE.md
  - src/
    - main.cpp
    - controller/
      - messagerouter.*
      - commands/
      - handlers/
    - model/
      - commandContext.h
      - decoders/
      - entities/
      - enums/
- tdc-botonera/
  - botonera/
    - DDM/ (QML UI)
      - SitrepWorkspace.qml
    - src/
      - controller/
        - protocol/
          - ddmcontroller.h
          - ddmcontroller.cpp

(Usa la vista del repo para navegar a los archivos mencionados.)

---

## Arquitectura general

### Backend (DDM)
- MessageRouter (`src/controller/messagerouter.cpp`) decide si el mensaje es JSON (frontend) o binario (DCL) y lo enruta.
- `JsonCommandHandler` mantiene un `m_commandMap` con comandos como `create_line`, `delete_track`, `list_tracks`.
- `CommandContext` (header-only) es el estado único compartido: listas de `CursorEntity` y `Track`, centro X/Y, etc.
- Decoders/Encoders:
  - `concDecoder` decodifica mensajes binarios del hardware.
  - `lpdEncoder` genera paquetes periódicos hacia hardware (cada 40 ms por timer).
- CLI: hay `sitrep` y otras utilidades en `src/controller/commands/` que imprimen en formato humano.

Referencias clave:
- [src/main.cpp](DesformatConcentrator/src/main.cpp)
- [src/controller/messagerouter.cpp](DesformatConcentrator/src/controller/messagerouter.cpp)
- [src/model/commandContext.h](DesformatConcentrator/src/model/commandContext.h)

### Frontend (Botonera / QML)
- `DDMController` (C++ QObject) parsea respuestas JSON y expone `tracksList` (QVariantList) a QML.
- `SitrepWorkspace.qml` consume `ddmController.tracksList` y renderiza la tabla SITREP.
- Transport layer: Local IPC o UDP según `Configuration`.

Referencias:
- [tdc-botonera/botonera/src/controller/protocol/ddmcontroller.h](tdc-botonera/botonera/src/controller/protocol/ddmcontroller.h)
- [tdc-botonera/botonera/DDM/SitrepWorkspace.qml](tdc-botonera/botonera/DDM/SitrepWorkspace.qml)

---

## Formato de mensajes JSON (resumen)
Comunicación estándar JSON entre backend y frontend:

Ejemplo base de respuesta:
```json
{
  "status": "success",
  "command": "list_tracks",
  "args": { "tracks": [ /* objetos track */ ] }
}
```

Campos importantes de un `track` en JSON (backend → frontend):
- `id`: entero / id único
- `identity`: string (Identidad: AMIGO / HOSTIL / Pending / etc.)
- `azimut`: número (grados)
- `distancia`: número (Nm o unidades internas)
- `rumbo`: entero
- `velocidad`: número
- `link`: string (`TX`, `RX`, `--`)
- `info`: string (información ampliatoria — se envía desde `addCommand`)

Archivos relevantes:
- [src/controller/handlers/trackcommandhandler.cpp](DesformatConcentrator/src/controller/handlers/trackcommandhandler.cpp)
- [src/controller/commands/addCommand.cpp](DesformatConcentrator/src/controller/commands/addCommand.cpp)

---

## SITREP — detalles de UI y formato
- La CLI (`sitrepCommand`) formatea texto humano (redondeos, placeholders `--` para lat/long, identidades bien legibles).
- La GUI originalmente mostraba valores brutos; se sincronizó con la CLI mediante formateo en `DDMController` (QVariantMap) para que la UI muestre exactamente lo mismo.
- Filtrado visual: se añadieron filtros en `SitrepWorkspace.qml` (`TODOS`, `AMIGOS`, `DESC.`, `HOSTILES`, `TX`, `RX`) y búsqueda. Estos filtros son puramente visuales (no afectan backend).

Cambios clave (recientes):
- `tdc-botonera/botonera/src/controller/protocol/ddmcontroller.cpp` — ahora formatea campos (`azimut`, `distancia`, `rumbo`, `velocidad`, placeholders de `lat/lon`), mantiene valores numéricos en `azimutNum`, `distanciaNum`, etc.
- `tdc-botonera/botonera/DDM/SitrepWorkspace.qml` — usa `filteredTracks()` y control de filtros + búsqueda; el botón borrar llama `ddmController.deleteTrack(id)` en lugar de mutar localmente el modelo.

---

## Build & Run

### Backend (DesformatConcentrator / DDM)
Requisitos: Qt6, qmake, MinGW (en Windows) o equivalente.
Comandos:
```bash
cd DesformatConcentrator
qmake DDM.pro
mingw32-make
# Ejecutar:
./DDM/Debug/DDM.exe  (o binario generado)
```

Notas:
- Si cambias `.pro`, vuelve a correr `qmake` antes de `make`.
- Revisa `Configuration::instance()` en `src/model/utils/configuration.h` para puertos y `useLocalIpc`.

### Frontend (tdc-botonera)
Requisitos: QtQuick/QML runtime, Qt Creator sugiere ejecutar el .qml desde el proyecto `botonera`.
Comandos (compilación de C++ parte):
```bash
cd tdc-botonera/botonera
qmake tdc-botonera.pro
mingw32-make
```

Para desarrollo QML puro, puedes abrir `DDM/` en QtCreator y ejecutar la interfaz en modo debug con el backend corriendo (o usando transporte LocalIPC).

---

## Cambios realizados durante la sesión (resumen técnico)
- Solucionado error de compilación en `ddmcontroller.cpp` añadiendo `#include <QJsonDocument>` y en `ddmcontroller.h` añadiendo `#include <QJsonObject>` y `#include <QVariant>`.
- GUI:
  - Añadido formateo de campos en `ddmcontroller.cpp` (azimut/distancia/rumbo/velocidad, lat/lon placeholders, `info`).
  - Implementado `deleteTrack(int)` invocable que construye `{ "command": "delete_track", "args": {"id": <id>} }` y emite `sendBackendCommand`.
  - `SitrepWorkspace.qml` actualizado para llamar `ddmController.deleteTrack(tid)` y para usar `filteredTracks()` como modelo.
  - Añadido filtrado visual y búsqueda en `SitrepWorkspace.qml`.
- Backend:
  - Normalizado el string de identidad usando `TrackData::toQString(...)` en puntos donde se construye JSON para evitar discrepancias.

Archivos modificados (localizados):
- `tdc-botonera/botonera/src/controller/protocol/ddmcontroller.cpp`
- `tdc-botonera/botonera/src/controller/protocol/ddmcontroller.h`
- `tdc-botonera/botonera/DDM/SitrepWorkspace.qml`
- `DesformatConcentrator/src/controller/handlers/trackcommandhandler.cpp` (ajuste de identity)
- `DesformatConcentrator/src/controller/commands/addCommand.cpp` (ajuste de identity)

---

## Notas sobre extensión y próximos pasos sugeridos
1. Lat/Lon: Actualmente la GUI muestra `--` como placeholder; si quieres conversión real desde coordenadas de pantalla/DM a lat/lon hay que implementar la fórmula/mapa en `CommandContext` o en `DDMController` y añadir los campos `latNum`/`lonNum` al JSON.
2. Filters as properties: Si necesitas exponer el estado del filtro al backend o a otros componentes, exponer `filterSelected` en `DDMController` como `Q_PROPERTY`.
3. UX para delete: añadir deshabilitado/indicador mientras backend confirma eliminación.
4. Tests: añadir pruebas unitarias para `JsonResponseParser` y para la lógica de formateo en `DDMController`.

---

## Cómo contribuir rápido (checklist)
- Para cambios de backend: modificar `src/` y compilar `DesformatConcentrator` (qmake → make).
- Para cambios de UI: editar `DDM/*.qml`, reiniciar la app front-end (no es necesario rebuild si sólo QML cambia).
- Para depuración de mensajes: usar `nc -u` para simular paquetes JSON hacia el puerto del backend.

---

## Contactos / historial de cambios recientes
- Cambios y parches recientes fueron aplicados interactuando con el repositorio local (formatos SITREP, filtros QML, delete mapping). Si quieres, puedo crear un PR o commits con mensajes claros para cada cambio.

---

Si quieres que amplíe alguna sección (por ejemplo: diagrama de mensajes, tabla de endpoints JSON, o pasos precisos para convertir coordenadas a lat/lon) dime cuál y lo añado. También puedo mover este archivo al root del workspace o a `tdc-botonera/docs/` si prefieres.
