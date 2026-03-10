# Architecture Overview

## Class Diagram

```mermaid
classDiagram
    %% Interfaces & Base Classes
    class ITransport {
        <<interface>>
        +send(QByteArray) bool
        +start()
        +stop()
    }

    class ICommand {
        <<interface>>
        +execute(CommandInvocation, CommandContext) CommandResult
    }

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

    class JsonCommandHandler {
        -CursorCommandHandler* m_cursorHandler
        -TrackCommandHandler* m_trackHandler
        -GeometryCommandHandler* m_geometryHandler
        +processJsonCommand(QByteArray)
    }

    class CursorCommandHandler {
        -CursorService m_service
        +createLine(QJsonObject) QByteArray
        +deleteLine(QJsonObject) QByteArray
    }

    class TrackCommandHandler {
        -TrackService m_service
        +createTrack(QJsonObject) QByteArray
        +deleteTrack(QJsonObject) QByteArray
        +listTracks(QJsonObject) QByteArray
    }

    class GeometryCommandHandler {
        -GeometryService m_service
        +createArea(QJsonObject) QByteArray
        +deleteArea(QJsonObject) QByteArray
        +createCircle(QJsonObject) QByteArray
        +deleteCircle(QJsonObject) QByteArray
        +createPolygon(QJsonObject) QByteArray
        +deletePolygon(QJsonObject) QByteArray
        +listShapes(QJsonObject) QByteArray
    }

    class CursorService {
        +createCursor(CursorCreateRequest) CursorOperationResult
        +deleteCursorById(int) CursorOperationResult
    }

    class TrackService {
        +createTrack(TrackCreateRequest) TrackOperationResult
        +deleteTrackById(int) TrackOperationResult
        +serializeTracks() QJsonArray
    }

    class GeometryService {
        +createArea(AreaCreateRequest) ShapeOperationResult
        +deleteAreaById(int) ShapeOperationResult
        +createCircle(CircleCreateRequest) ShapeOperationResult
        +deleteCircleById(int) ShapeOperationResult
        +createPolygon(PolygonCreateRequest) ShapeOperationResult
        +deletePolygonById(int) ShapeOperationResult
        +listShapes() QJsonObject
    }

    class CommandDispatcher {
        -CommandRegistry* m_registry
        -CommandParser* m_parser
        -CommandContext& m_ctx
        +onLine(QString)
    }

    class CommandContext {
        +std::deque~Track~ tracks
        +std::deque~CursorEntity~ cursors
        +std::deque~AreaEntity~ areas
        +std::deque~CircleEntity~ circles
        +std::deque~PolygonoEntity~ polygons
        +double centerX
        +double centerY
    }

    class ConcDecoder {
        +decode(QByteArray)
    }

    class LocalIpcClient {
        +send(QByteArray)
    }

    class UdpClientAdapter {
        +send(QByteArray)
    }

    %% Relationships
    ITransport <|-- LocalIpcClient
    ITransport <|-- UdpClientAdapter

    MessageRouter --> DclConcController
    MessageRouter --> JsonCommandHandler

    JsonCommandHandler --> CursorCommandHandler
    JsonCommandHandler --> TrackCommandHandler
    JsonCommandHandler --> GeometryCommandHandler

    CursorCommandHandler --> CursorService
    TrackCommandHandler --> TrackService
    GeometryCommandHandler --> GeometryService

    CursorService --> CommandContext
    TrackService --> CommandContext
    GeometryService --> CommandContext

    DclConcController --> ITransport
    DclConcController --> ConcDecoder

    CommandDispatcher --> CommandContext
```

## Runtime Flow

```mermaid
flowchart TD
    A[Frontend QML] -->|JSON| B[MessageRouter]
    C[Consola STDIN] --> D[CommandDispatcher]
    E[DCL Concentrator] -->|Binario| B

    B --> F[JsonCommandHandler]
    B --> G[DclConcController]

    F --> H[Cursor/Track/Geometry Handlers]
    H --> I[Services Layer]
    D --> I

    I --> J[CommandContext]
    J --> K[JsonResponseBuilder]
    K -->|JSON| A

    G --> L[ConcDecoder]
    J --> M[lpdEncoder]
```

## Key Components

- Transport Layer: abstrae UDP y Local IPC mediante ITransport.
- MessageRouter: punto de entrada de red, decide JSON vs binario DCL.
- JsonCommandHandler: despacha comandos JSON a handlers específicos.
- Handlers: adaptadores de protocolo (parseo/validación/respuesta).
- Services: lógica de negocio compartida entre JSON y CLI.
- CommandContext: estado único del backend (tracks, cursores y figuras).
- CommandDispatcher: ejecuta comandos de consola sobre el mismo estado.

---

## Cambios recientes (Mar 2026)

- Se eliminó duplicación entre CLI y JSON moviendo reglas de negocio a CursorService, TrackService y GeometryService.
- Se añadieron comandos JSON de geometría faltantes: delete_polygon y list_shapes.
- CommandContext incorporó colecciones de areas, circles y polygons además de tracks/cursors.
- Se centralizó normalización angular en RadarMath::normalizeAngle360.
- Se resolvieron errores de compilación por tipos incompletos en headers de handlers/services y firmas inconsistentes en entidades.
- DDMController mantiene formateo de tracks para QML y la eliminación de track vía comando JSON delete_track.
