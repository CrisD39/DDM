#include "trackcommandhandler.h"
#include "../json/jsonresponsebuilder.h"
#include "../services/trackservice.h"
#include "commandContext.h"
#include "network/iTransport.h"
#include "entities/track.h"
#include "enums/enums.h"
#include <QJsonArray>
#include <QDebug>

TrackCommandHandler::TrackCommandHandler(CommandContext* context, ITransport* transport)
    : m_context(context), m_transport(transport), m_trackService(std::make_unique<TrackService>(context))
{
    Q_ASSERT(m_context);
    Q_ASSERT(m_transport);
}

QByteArray TrackCommandHandler::createTrack(const QJsonObject& args)
{
    // Validar coordenadas
    if (!args.contains("x") || !args.contains("y")) {
        return JsonResponseBuilder::buildValidationErrorResponse("create_track", "x/y", "", "required");
    }

    TrackCreateRequest request;
    request.x = args.value("x").toDouble();
    request.y = args.value("y").toDouble();
    request.type = static_cast<TrackData::Type>(args.value("type").toInt(static_cast<int>(0)));
    if (args.contains("info")) {
        request.info = args.value("info").toString();
    }

    TrackOperationResult result = m_trackService->createTrack(request);
    if (!result.success) {
        qWarning() << "[TrackCommandHandler] Error al crear track:" << result.message;
        return JsonResponseBuilder::buildErrorResponse("create_track", result.errorCode, result.message);
    }

    return buildCreateTrackResponse(result.trackId);
}

QByteArray TrackCommandHandler::deleteTrack(const QJsonObject& args)
{
    if (!args.contains("id")) {
        return JsonResponseBuilder::buildValidationErrorResponse("delete_track", "id", "", "required");
    }
    int id = args.value("id").toInt(-1);
    if (id < 0) return JsonResponseBuilder::buildValidationErrorResponse("delete_track", "id", QString::number(id), ">=0");

    TrackOperationResult result = m_trackService->deleteTrackById(id);
    if (!result.success) {
        return JsonResponseBuilder::buildErrorResponse("delete_track", result.errorCode, result.message);
    }

    return buildDeleteTrackResponse(id);
}

QByteArray TrackCommandHandler::listTracks(const QJsonObject& /*args*/)
{
    return buildListTracksResponse();
}

QByteArray TrackCommandHandler::buildCreateTrackResponse(int createdId)
{
    QJsonObject args;
    args["created_id"] = QString::number(createdId);
    args["tracks"] = m_trackService->serializeTracks();
    return JsonResponseBuilder::buildSuccessResponse("create_track", args);
}

QByteArray TrackCommandHandler::buildDeleteTrackResponse(int deletedId)
{
    QJsonObject args;
    args["deleted_id"] = deletedId;
    args["tracks"] = m_trackService->serializeTracks();
    return JsonResponseBuilder::buildSuccessResponse("delete_track", args);
}

QByteArray TrackCommandHandler::buildListTracksResponse()
{
    QJsonObject args;
    args["tracks"] = m_trackService->serializeTracks();
    return JsonResponseBuilder::buildSuccessResponse("list_tracks", args);
}