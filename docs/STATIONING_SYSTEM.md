# Sistema Estacionamiento (EST)

## 1. Objetivo y alcance

El componente Estacionamiento calcula la maniobra cinemática entre dos unidades para que TRACK-A alcance una posicion relativa respecto de TRACK-B.

Entradas funcionales requeridas:

- TRACK-A (ID de track)
- TRACK-B (ID de track)
- Azimut relativo (AZ)
- Distancia (D)
- Modalidad mutuamente excluyente:
  - Velocidad de TRACK-A (VD)
  - Tiempo de maniobra (DU)

Restriccion de esta etapa:

- Backend DDM only (sin UI de Botonera)
- Logica pura + interfaz CLI

---

## 2. Contrato CLI (fase actual)

Comando:

```text
estacionamiento [--track-a=<ID|0000>] --track-b=<ID_EXTERNO> --az=<deg> --d=<dm> \
  (--vd=<knots> | --du=<hours>)
```

Regla de default:

- Si se omite `--track-a`, se asume `0000` (Buque Propio).
- `--track-b` es obligatorio y no puede ser `0000`.

ID reservado:

- `0000` (o `0`) representa Buque Propio.
- Para `0000` no se consulta la lista de tracks regulares.
- Se toma estado desde `CommandContext::ownShip`.
- Coordenadas para `0000`: `xDm = 0.0`, `yDm = 0.0`.
- `ownship set` (y `ownship_update`) sincroniza un track virtual con `id=0000` dentro del estado backend.

Validaciones obligatorias:

- `vd` y `du` son excluyentes (exactamente uno debe estar presente).
- Solo se aceptan argumentos: `track-a`, `track-b`, `az`, `d`, `vd`, `du`.
- Todos los campos numericos deben ser finitos.
- `track-a` y `track-b` deben ser IDs enteros `>= 0` (`0` reservado para OwnShip).
- `track-b` debe ser un track externo (`track-b != 0`).
- `d > 0`.
- `vd > 0` cuando aplica.
- `du > 0` cuando aplica.
- Si se usa `0000` y `ownShip` no esta inicializado, se devuelve error indicando ejecutar `ownship set`.

Fuente de datos cinemáticos:

- El operador no ingresa posicion/rumbo/velocidad de las unidades.
- El backend obtiene esa informacion desde `CommandContext` a partir de los IDs.

Salida CLI esperada:

```text
RUMBO: XXX / TIEMPO: H 00:00:00
```

---

## 3. Modelo cinematico implementado

Se asume movimiento rectilineo uniforme para ambas unidades durante la maniobra.

### 3.1 Sistema de referencia

- Convencion angular: `0 deg = Norte (+Y)`, `90 deg = Este (+X)`, sentido horario.
- `AZ` se interpreta como azimut relativo al rumbo actual de TRACK-B.
- Azimut absoluto de estacionamiento:

$$
AZ_{abs} = normalize(course_B + AZ)
$$

Vector offset deseado (desde TRACK-B hacia posicion de estacionamiento):

$$
r_{off} = D \cdot (\sin(AZ_{abs}), \cos(AZ_{abs}))
$$

### 3.2 Punto objetivo dinamico

Con $A_0$ y $B_0$ posiciones iniciales, y $V_B$ velocidad de TRACK-B:

$$
P_{obj}(t) = B_0 + V_B t + r_{off}
$$

TRACK-A debe cumplir:

$$
A_0 + V_A t = P_{obj}(t)
$$

### 3.3 Modalidad VD (velocidad conocida)

Si $|V_A| = VD$, el tiempo surge de:

$$
|R + V_B t| = VD \cdot t,
\quad R = (B_0 + r_{off} - A_0)
$$

que lleva a una cuadratica:

$$
(|V_B|^2 - VD^2)t^2 + 2(R\cdot V_B)t + |R|^2 = 0
$$

Se elige la menor raiz positiva valida.

### 3.4 Modalidad DU (tiempo conocido)

Con $t = DU$:

$$
V_A = \frac{R + V_B t}{t}
$$

de donde se obtiene el rumbo de TRACK-A.

### 3.5 Salidas de calculo

- `rumboDeg` de TRACK-A.
- `timeHours` de maniobra.
- Estados de error: entrada invalida o solucion no real/positiva.

---

## 4. Arquitectura de implementacion en DDM

Separacion SRP aplicada:

- `src/model/estacionamientocalculator.*`:
  - Motor matematico puro (sin CLI, sin JSON, sin acceso a estado global).
- `src/controller/services/estacionamientoservice.*`:
  - Parseo/validacion de entrada CLI, lookup por ID y adaptacion de unidades.
- `src/controller/commands/estacionamientocommand.*`:
  - Comando CLI (`ICommand`) y presentacion de salida.

Integracion:

- Registro en `main.cpp` para estar disponible en consola junto a comandos existentes.

---

## 5. Casos limite contemplados

- `vd` y `du` juntos -> error de validacion.
- Ninguno de `vd`/`du` -> error de validacion.
- Sin raiz positiva en modo VD -> no hay solucion cinemática valida.
- Tiempo cero/negativo en modo DU -> invalido.
- Magnitudes no finitas -> invalido.

---

## 6. Evolucion prevista

- Reusar el mismo motor para futura API JSON (sin duplicar matematica).
- Resolver automaticamente datos de TRACK-A/B desde `CommandContext` cuando haya identificadores operativos estables para la unidad guia y el buque propio.
- Incorporar restricciones tacticas adicionales (ej. limites de rumbo/velocidad por plataforma).