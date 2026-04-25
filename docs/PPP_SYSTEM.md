# PPP System (31/03)

## 1. Objetivo y alcance

Este documento describe la implementacion actual del sistema PPP/CPA en backend DDM.

Alcance de esta etapa:

- Definir una unica cuenta matematica PPP/CPA reutilizable.
- Persistir PPP como atributo del `Track` para el caso SITREP.
- Calcular PPP de SITREP contra OwnShip en eventos puntuales (sin recálculo continuo).

Fuera de alcance de esta etapa:

- Recalculo periodico acoplado al movimiento continuo de tracks.
- Rediseño de la herramienta visual PPP de GUI.

---

## 2. Definiciones funcionales

PPP en este contexto se implementa como CPA (Closest Point of Approach), con salida:

- AZ: azimut del punto de aproximacion.
- DT: distancia del punto de aproximacion.
- T: tiempo al punto de aproximacion.

Para salida de SITREP, el tiempo se presenta en formato `HH:MM`.

---

## 3. Diferencia de casos de uso

### 3.1 PPP de SITREP

- Pareja de calculo: `Track vs OwnShip`.
- Persistencia: se guarda en atributos PPP dentro de cada `Track`.
- Consumo: serializacion JSON de tracks y visualizacion en SITREP.

### 3.2 PPP de GUI (herramienta operativa)

- Pareja de calculo: `Track vs Track`.
- No depende de OwnShip.
- Es conceptualmente una herramienta distinta, pero usa la misma matematica base.

Resumen:

- La cuenta PPP/CPA es una sola.
- Cambia el caso de uso y la pareja de entidades.

---

## 4. Arquitectura tecnica

## 4.1 Motor matematico generico

Archivo:

- `src/model/pppcalculator.h`
- `src/model/pppcalculator.cpp`

Clase:

- `PppCalculator`

Responsabilidad:

- Recibir dos estados cinemáticos.
- Resolver una unica cuenta PPP/CPA sin acoplamiento a SITREP ni OwnShip.
- Devolver resultado con estado (valido o no calculable) y motivo.

## 4.2 Integracion de caso de uso SITREP

Archivo:

- `src/controller/services/trackpppservice.h`
- `src/controller/services/trackpppservice.cpp`

Clase:

- `TrackPppService`

Responsabilidad:

- Tomar `Track` y `OwnShipState`.
- Construir entradas cinemáticas para `PppCalculator`.
- Persistir resultado PPP dentro del `Track`.
- Ejecutar recálculo masivo cuando aplica.

## 4.3 Modelo de datos

Archivo:

- `src/model/entities/track.h`
- `src/model/entities/track.cpp`

`Track` persiste campos PPP de SITREP con:

- azimut
- distancia
- tiempo (interno)
- estado/motivo
- formateo de salida `HH:MM`

---

## 5. Flujo de calculo actual

## 5.1 Al actualizar OwnShip

Puntos de entrada:

- CLI: `ownship set <course_deg> <speed_knots> [source]`
- JSON: `ownship_update`

Integracion:

- `src/controller/services/ownshipservice.cpp`

Comportamiento:

1. Persiste estado OwnShip en `CommandContext::ownShip`.
2. Si OwnShip es valido, ejecuta recálculo one-shot para tracks existentes.
3. Si no es valido, deja PPP como no calculable.

## 5.2 Al crear un track

Puntos de alta cubiertos:

- CLI/JSON: `TrackService` (`src/controller/services/trackservice.cpp`)
- QEK/botonera: `QEK` (`src/model/qek.h`)

Comportamiento:

1. Se crea/inserta track en contenedor.
2. Si OwnShip ya es valido, se calcula PPP inmediatamente para ese track.
3. Si OwnShip no es valido, no se calcula PPP en ese momento.

Con esto no se requiere re-ejecutar `ownship set` para nuevos tracks cuando OwnShip ya existe.

---

## 6. Integracion de salida y serializacion

Archivo:

- `src/controller/services/trackservice.cpp`

Campos JSON en `tracks`:

- `ppp_az`
- `ppp_dt`
- `ppp_t_hhmm`
- `ppp_status`
- `ppp_reason`

CLI SITREP:

- `src/controller/commands/sitrepcommand.cpp`
- Muestra PPP por track si estado es calculable.

Frontend:

- El parser de tracks consume esos campos y alimenta la tabla SITREP.

---

## 7. Casos borde y reglas

## 7.1 OwnShip no valido

- No se calcula PPP.
- El resultado queda en estado no calculable, con motivo trazable.

## 7.2 OwnShip con speed = 0

- Se acepta como dato valido (no rompe el sistema).
- Se trata como caso especial/degenerado segun cinemática relativa.
- Se informa estado/motivo en resultado PPP cuando corresponda.

## 7.3 Tracks estaticos

- En esta etapa se consideran estaticos para PPP operativo de SITREP.
- Se calcula una vez en disparadores de negocio (ownship set, alta de track con ownship valido).

---

## 8. Decisiones de diseño

## 8.1 PPP persistido en Track

Razon:

- Requisito funcional de SITREP por fila de track.
- Evita recalculo ad-hoc en capa de presentacion.
- Permite serializacion uniforme por `list_tracks`.

## 8.2 Motor unico reutilizable

Razon:

- Evita duplicacion matematica entre SITREP y GUI.
- Reduce divergencias de resultados entre casos de uso.

## 8.3 Sin recálculo continuo por ahora

Razon:

- Requisito de etapa actual.
- Tracks considerados estaticos en operacion PPP actual.

---

## 9. Evolucion futura (tracks dinamicos)

Si los tracks pasan a dinamicos:

1. Integrar recálculo PPP en el flujo cinemático (`updateTracks()` o punto equivalente).
2. Mantener `PppCalculator` como motor puro de dominio.
3. Mantener `TrackPppService` como capa de integracion, evitando logica de presentacion en servicios de dominio.

Esto preserva separacion de responsabilidades y consistencia entre SITREP y GUI.

---

## 10. Trazabilidad de archivos

Motor y dominio:

- `src/model/pppcalculator.h`
- `src/model/pppcalculator.cpp`
- `src/model/cpa.cpp` (reuso del motor para Track vs Track)- `src/model/entities/track.h`

Integracion aplicacion:

- `src/controller/services/trackpppservice.h`
- `src/controller/services/trackpppservice.cpp`
- `src/controller/services/ownshipservice.cpp`
- `src/controller/services/trackservice.cpp`
- `src/model/qek.h`

Exposicion:

- `src/controller/commands/sitrepcommand.cpp`
- `src/controller/services/trackservice.cpp` (serializacion JSON)
