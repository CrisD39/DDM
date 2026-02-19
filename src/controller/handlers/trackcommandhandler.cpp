#include "trackcommandhandler.h"
#include "../json/jsonresponsebuilder.h"
#include "commandContext.h"
#include "network/iTransport.h"
#include "entities/track.h"
#include "enums/enums.h"
#include <QJsonArray>
#include <QDebug>

TrackCommandHandler::TrackCommandHandler(CommandContext* context, ITransport* transport)
    : m_context(context), m_transport(transport)
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

    double x = args.value("x").toDouble();
    double y = args.value("y").toDouble();
    int type = args.value("type").toInt(static_cast<int>(0));

    // id
    const int id = m_context->nextTrackId++;
    try {
        Track& t = m_context->emplaceTrackFront(
            id,
            static_cast<TrackData::Type>(type),
            TrackData::Identity::Pending,
            TrackData::TrackMode::Auto,
            static_cast<float>(x),
            static_cast<float>(y)
        );

        // opcional: store sitrep info if provided
        if (args.contains("info")) {
            m_context->setSitrepInfo(t.getId(), args.value("info").toString());
        }

        // notificar transporte
        if (m_transport) {
            return buildCreateTrackResponse(t.getId());
        }

        return JsonResponseBuilder::buildSuccessResponse("create_track", QJsonObject{{"created_id", QString::number(t.getId())}});

    } catch (const std::exception& e) {
        qWarning() << "[TrackCommandHandler] Error al crear track:" << e.what();
        return JsonResponseBuilder::buildErrorResponse("create_track", "BACKEND_ERROR", e.what());
    }
}

QByteArray TrackCommandHandler::deleteTrack(const QJsonObject& args)
{
    if (!args.contains("id")) {
        return JsonResponseBuilder::buildValidationErrorResponse("delete_track", "id", "", "required");
    }
    int id = args.value("id").toInt(-1);
    if (id < 0) return JsonResponseBuilder::buildValidationErrorResponse("delete_track", "id", QString::number(id), ">=0");

    bool ok = m_context->eraseTrackById(id);
    if (!ok) {
        return JsonResponseBuilder::buildErrorResponse("delete_track", "NOT_FOUND", "Track not found");
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
    
    auto identityToString = [](const Track& t) -> QString {
        const char* names[] = { "Pending", "PossFriend", "Unknown", "ConfHostile",
                               "ConfFriend", "Unknown", "EvalUnknown", "Heli" };
        auto ident = t.getIdentity();
        if (ident >= 0 && ident <= 7) return QString::fromLatin1(names[ident]);
        return "Desconocida";
    };
    
    QJsonArray arr;
    for (const Track& tr : m_context->getTracks()) {
        QJsonObject trackObj;
        trackObj["id"] = tr.getId();
        trackObj["identity"] = identityToString(tr);
        trackObj["azimut"] = tr.getAzimuthDeg();
        trackObj["distancia"] = tr.getDistanceDm();
        trackObj["rumbo"] = tr.getCursoInt();
        trackObj["velocidad"] = tr.getVelocidadDmPerHour();
        trackObj["link"] = tr.getEstadoLinkY() == Track::LinkY_Invalid ? "--" :
                           QString(QChar("RCTS"[int(tr.getEstadoLinkY())])); // R,C,T,S en orden 0..3
        trackObj["lat"] = tr.getY();
        trackObj["lon"] = tr.getX();
        trackObj["info"] = tr.getInformacionAmpliatoria();
        arr.append(trackObj);
    }
    args["tracks"] = arr;
    return JsonResponseBuilder::buildSuccessResponse("create_track", args);
}

QByteArray TrackCommandHandler::buildDeleteTrackResponse(int deletedId)
{
    QJsonObject args;
    args["deleted_id"] = deletedId;
    
    auto identityToString = [](const Track& t) -> QString {
        const char* names[] = { "Pending", "PossFriend", "Unknown", "ConfHostile",
                               "ConfFriend", "Unknown", "EvalUnknown", "Heli" };
        auto ident = t.getIdentity();
        if (ident >= 0 && ident <= 7) return QString::fromLatin1(names[ident]);
        return "Desconocida";
    };
    
    QJsonArray arr;
    for (const Track& tr : m_context->getTracks()) {
        QJsonObject trackObj;
        trackObj["id"] = tr.getId();
        trackObj["identity"] = identityToString(tr);
        trackObj["azimut"] = tr.getAzimuthDeg();
        trackObj["distancia"] = tr.getDistanceDm();
        trackObj["rumbo"] = tr.getCursoInt();
        trackObj["velocidad"] = tr.getVelocidadDmPerHour();
        trackObj["link"] = tr.getEstadoLinkY() == Track::LinkY_Invalid ? "--" :
                           QString(QChar("RCTS"[int(tr.getEstadoLinkY())]));
        trackObj["lat"] = tr.getY();
        trackObj["lon"] = tr.getX();
        trackObj["info"] = tr.getInformacionAmpliatoria();
        arr.append(trackObj);
    }
    args["tracks"] = arr;
    return JsonResponseBuilder::buildSuccessResponse("delete_track", args);
}

QByteArray TrackCommandHandler::buildListTracksResponse()
{
    QJsonObject args;
    
    auto identityToString = [](const Track& t) -> QString {
        const char* names[] = { "Pending", "PossFriend", "Unknown", "ConfHostile",
                               "ConfFriend", "Unknown", "EvalUnknown", "Heli" };
        auto ident = t.getIdentity();
        if (ident >= 0 && ident <= 7) return QString::fromLatin1(names[ident]);
        return "Desconocida";
    };
    
    QJsonArray arr;
    for (const Track& tr : m_context->getTracks()) {
        QJsonObject trackObj;
        trackObj["id"] = tr.getId();
        trackObj["identity"] = identityToString(tr);
        trackObj["azimut"] = tr.getAzimuthDeg();
        trackObj["distancia"] = tr.getDistanceDm();
        trackObj["rumbo"] = tr.getCursoInt();
        trackObj["velocidad"] = tr.getVelocidadDmPerHour();
        trackObj["link"] = tr.getEstadoLinkY() == Track::LinkY_Invalid ? "--" :
                           QString(QChar("RCTS"[int(tr.getEstadoLinkY())]));
        trackObj["lat"] = tr.getY();
        trackObj["lon"] = tr.getX();
        trackObj["info"] = tr.getInformacionAmpliatoria();
        arr.append(trackObj);
    }
    args["tracks"] = arr;
    return JsonResponseBuilder::buildSuccessResponse("list_tracks", args);
}