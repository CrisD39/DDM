# Módulo: OwnShip

## Descripción general

Este módulo concentra el estado del buque propio dentro del backend DDM. Su objetivo es mantener una única fuente de verdad para los parámetros cinemáticos y geográficos de ownship, y exponerlos al resto del sistema para cálculos de PPP/SITREP, estacionamiento, serialización JSON y representación CLI.

En la práctica, ownship puede llegar desde dos orígenes:

- actualización manual por consola o JSON (`ownship set` / `ownship_update`)
- actualización automática desde `botonera`, cuando el GPS externo publica el snapshot consolidado

La regla de negocio actual es simple: el propio buque siempre se modela con posición relativa `(0,0)` en DM para los cálculos internos, pero conserva latitud/longitud absolutas para poder georreferenciar tracks y alimentar reportes.

## Responsabilidad funcional

- Mantener el snapshot operativo de ownship.
- Sincronizar el track virtual `id=0` usado por PPP/SITREP/estacionamiento.
- Exponer el estado en formato JSON y textual para CLI.
- Servir como referencia geográfica para convertir coordenadas relativas de tracks a latitud/longitud.

## Contrato de datos

### Campos base

| Campo | Tipo | Descripción |
|---|---|---|
| `xDm` | `double` | Posición relativa en eje X del sistema interno. Para ownship se mantiene en `0.0`. |
| `yDm` | `double` | Posición relativa en eje Y del sistema interno. Para ownship se mantiene en `0.0`. |
| `latitudeDeg` | `double` | Latitud geográfica decimal. |
| `longitudeDeg` | `double` | Longitud geográfica decimal. |
| `speedKnots` | `double` | Velocidad propia en nudos. |
| `courseDeg` | `double` | Rumbo propio en grados. |
| `timeUtc` | `QString` | Hora UTC en formato ISO `hh:mm:ss`. |
| `dateUtc` | `QString` | Fecha UTC en formato ISO `yyyy-MM-dd`. |
| `source` | `QString` | Origen de la actualización (`GPS`, `MANUAL`, `CLI`, `AR-TDC`, etc.). |
| `valid` | `bool` | Marca de validez operativa del snapshot. |

### Campos derivados

| Campo | Tipo | Descripción |
|---|---|---|
| `latitudeDegrees` | `int` | Grados enteros de latitud. |
| `latitudeMinutes` | `int` | Minutos enteros de latitud. |
| `latitudeSeconds` | `int` | Segundos enteros de latitud. |
| `longitudeDegrees` | `int` | Grados enteros de longitud. |
| `longitudeMinutes` | `int` | Minutos enteros de longitud. |
| `longitudeSeconds` | `int` | Segundos enteros de longitud. |
| `nsIndicator` | `QChar` | Indicador hemisférico N/S. |
| `ewIndicator` | `QChar` | Indicador hemisférico E/W. |
| `day` | `QString` | Día de la fecha UTC. |
| `month` | `QString` | Mes de la fecha UTC en abreviatura (`JAN`, `FEB`, ...). |
| `year` | `QString` | Año de la fecha UTC. |

## Clases principales

### OwnShipState

- **Rol**: snapshot persistido de ownship dentro de `CommandContext`.
- **Uso**: lo consumen `OwnShipService`, `TrackPppService`, `EstacionamientoService` y los serializadores de track/listado.

### OwnShipService

- **Rol**: punto de acceso de negocio para ownship.
- **Métodos clave**:

| Método | Firma | Descripción |
|---|---|---|
| Actualizar JSON | `OwnShipOperationResult updateFromJson(const QJsonObject& args)` | Valida payload `ownship_update` y actualiza `CommandContext::ownShip`. |
| Actualizar CLI | `OwnShipOperationResult setFromCli(double courseDeg, double speedKnots, const QString& source = QStringLiteral("CLI"))` | Actualiza ownship manual desde consola. |
| Serializar | `QJsonObject serializeOwnShip() const` | Devuelve snapshot completo de ownship. |
| Formatear | `QString formatOwnShip() const` | Texto resumido para CLI. |

- **Dependencias**:
- `CommandContext`
- `TrackPppService`

## Flujo de datos

### Entrada manual

1. El usuario ejecuta `ownship set` por consola o `ownship_update` por JSON.
2. `OwnShipService` valida curso, velocidad y coordenadas.
3. `CommandContext::ownShip` se actualiza.
4. Se sincroniza el track virtual `id=0`.
5. Se recalculan sesiones PPP/SITREP dependientes.

### Entrada automática desde botonera

1. `botonera` recibe GPS por `GPSSocket` y consolida el snapshot en `OwnShipData`.
2. El snapshot se publica hacia DDM como `ownship_update`.
3. DDM lo procesa por `JsonCommandHandler -> OwnShipCommandHandler -> OwnShipService`.
4. Se actualiza el mismo `CommandContext::ownShip` que usa el resto del backend.

### Uso de ownship como referencia geográfica

Cuando `ownShip` es válido, los tracks conservan sus coordenadas relativas internas (`xDm`, `yDm`), pero al serializarse se pueden convertir a latitud/longitud geográfica usando la referencia absoluta del propio buque. Esto permite:

- mantener los cálculos internos en DM
- mostrar una lat/lon consistente en listas/SITREP
- evitar duplicación de lógica cinemática

## Manejo de errores

- Se rechaza el payload si faltan campos requeridos o si los valores numéricos están fuera de rango.
- Se considera inválido el ownship si no puede establecerse velocidad, curso o georreferencia mínima.
- En caso de datos manuales o GPS, el contrato acepta una fuente explícita en `source` para auditar el origen.

## Relación con otros módulos

- `docs/modules/command-context.md`: define el estado persistido y el track virtual `id=0`.
- `docs/modules/services.md`: documenta la lógica de negocio que consume ownship.
- `docs/modules/handlers.md`: entrada JSON `ownship_update`.
- `docs/modules/commands.md`: comando CLI `ownship` para pruebas.
- `docs/protocols/json-command-api.md`: contrato del mensaje `ownship_update`.

## Notas de integración

- El propio buque sigue anclado en `(0,0)` DM para los cálculos tácticos internos.
- La latitud/longitud geográfica existe para referencia, persistencia, exportación y cálculo de posición de tracks en presentaciones y reportes.
- La prioridad operativa prevista para el enlace entre proyectos es `GPS > MANUAL`.