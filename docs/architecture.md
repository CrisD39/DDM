# Architecture Overview

## Class Diagram

```mermaid
classDiagr
am
    %% Interfaces & Base Classes
    class ITransport {
        <<interface>>
        +send(QByteArray) bool
        +start()
        +stop()
    }

    class IDecodificator {
        <<interface>>
        +decode(QByteArray)
    }

    class IOBMHandler {
        <<interface>>
        +updatePosition(QPair)
        +getPosition() QPair
        +updateRange(int)
    }

    class ICommand {
        <<interface>>
        +execute(CommandInvocation, CommandContext) CommandResult
    }

    %% Core Controllers
    class DclConcController {
        -ITransport* m_link
        -ConcDecoder* m_decoder
        +onDatagram(QByteArray)
    }

    class MessageRouter {
        -DclConcController* m_dclController
        -JsonCommandHandler* m_jsonHandler
        +onMessageReceived(QByteArray)
    }

    class CommandDispatcher {
        -CommandRegistry* m_registry
        -CommandParser* m_parser
        -CommandContext& m_ctx
        +onLine(QString)
    }

    %% Network Implementation
    class LocalIpcClient {
        +send(QByteArray)
    }

    class UdpClientAdapter {
        +send(QByteArray)
    }

    class TransportFactory {
        +makeTransport(TransportKind, TransportOpts) unique_ptr~ITransport~
    }

    %% Decoders & Encoders
    class ConcDecoder {
        +decode(QByteArray)
    }

    %% Handlers & Models
    class OBMHandler {
        +updatePosition(QPair)
    }

    class OverlayHandler {
        -OBMHandler* obmHandler
        -CommandContext* ctx
        +onNewOverlay(QString)
    }

    class OwnCurs {
        -OBMHandler* obm
        -CommandContext* ctx
        +updateHandwheel(QPair)
    }

    class CommandContext {
        +std::deque~Track~ tracks
        +std::deque~CursorEntity~ cursors
        +updateTracks(double)
    }

    %% Relationships
    ITransport <|-- LocalIpcClient
    ITransport <|-- UdpClientAdapter
    TransportFactory ..> ITransport : creates

    IDecodificator <|-- ConcDecoder
    IOBMHandler <|-- OBMHandler

    DclConcController --> ITransport
    DclConcController --> ConcDecoder

    MessageRouter --> DclConcController
    MessageRouter --> JsonCommandHandler

    CommandDispatcher --> CommandContext
    CommandDispatcher --> CommandRegistry

    OverlayHandler --> OBMHandler
    OverlayHandler --> CommandContext

    OwnCurs --> OBMHandler
    OwnCurs --> CommandContext

    ConcDecoder --|> IDecodificator
```

## Key Components

- **Transport Layer**: Abstracts UDP and Local IPC communication via `ITransport`.
- **Decoding**: `ConcDecoder` processes incoming byte streams.
- **Controllers**: 
    - `DclConcController` manages the concentrator logic.
    - `MessageRouter` dispatches messages to the appropriate controller.
- **Command System**: 
    - `CommandDispatcher` parses and executes console commands.
    - `CommandContext` maintains the application state (tracks, cursors).
- **Overlay/OBM**: Handles display overlays and "Own Own Bearing Marker" (OBM) logic.

---

## Cambios recientes (Feb 2026)

- `DDMController` formatea los `tracks` antes de exponerlos a QML y añade campos numéricos auxiliares (`azimutNum`, `distanciaNum`, `rumboNum`, `velocidadNum`).
- Se añadió `deleteTrack(int)` invocable en `DDMController` para que la interfaz envíe `{ "command": "delete_track", "args": { "id": <id> } }` al backend, evitando la manipulación local directa del modelo.
- `SitrepWorkspace.qml` incluye filtros en cliente (TODOS, AMIGOS, DESC., HOSTILES, TX, RX) y búsqueda; el filtrado es únicamente visual.
- Normalización de `identity` en backend con `TrackData::toQString(...)` para alinear nomenclatura entre CLI y GUI.
- Correcciones de compilación: añadidos `QJsonDocument`, `QJsonObject`, `QVariant` en `ddmcontroller` para resolver tipos incompletos.
