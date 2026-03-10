#include "trackservice.h"

#include "commandContext.h"
#include "entities/track.h"
#include <QJsonObject>

TrackService::TrackService(CommandContext* context)
    : m_context(context)
{
    Q_ASSERT(m_context);
}

TrackOperationResult TrackService::createTrack(const TrackCreateRequest& request)
{
    if (request.x < -256.0 || request.x > 256.0 || request.y < -256.0 || request.y > 256.0) {
        return {false, "INVALID_COORDINATES", "Coordenadas fuera de rango. Deben estar entre -256 y 256.", -1};
    }

    if (request.speedDmPerHour.has_value()) {
        if (request.speedDmPerHour.value() < 0.0 || request.speedDmPerHour.value() > 99.9) {
            return {false, "INVALID_SPEED", "Velocidad DM/h fuera de rango (0..99.9)", -1};
        }
    }

    if (request.courseDeg.has_value()) {
        if (request.courseDeg.value() < 0 || request.courseDeg.value() > 359) {
            return {false, "INVALID_COURSE", "Curso fuera de rango (0..359)", -1};
        }
    }

    if (request.fc.has_value()) {
        if (request.fc.value() < 1 || request.fc.value() > 6) {
            return {false, "INVALID_FC", "Asignacion FC invalida (1..6)", -1};
        }
    }

    const int id = m_context->nextTrackId++;

    Track& track = m_context->emplaceTrackFront(
        id,
        request.type,
        request.identity,
        request.mode,
        static_cast<float>(request.x),
        static_cast<float>(request.y),
        request.ctorSpeedKnots,
        request.ctorCourseDeg
    );

    if (request.speedDmPerHour.has_value()) track.setVelocidadDmPerHour(request.speedDmPerHour.value());
    if (request.courseDeg.has_value()) track.setCursoInt(request.courseDeg.value());
    if (request.fc.has_value()) track.setAsignacionFc(request.fc.value());
    if (request.asgc.has_value()) track.setCodigoAsignacion(request.asgc.value());
    if (request.linkY.has_value()) track.setEstadoLinkY(request.linkY.value());
    if (request.link14.has_value()) track.setEstadoLink14(request.link14.value());
    if (request.info.has_value()) {
        track.setInformacionAmpliatoria(request.info.value());
        m_context->setSitrepInfo(id, request.info.value());
    }
    if (request.priv.has_value()) track.setCodigoPrivado(request.priv.value());

    return {true, QString(), QString(), id};
}

TrackOperationResult TrackService::deleteTrackById(int trackId)
{
    if (trackId < 0) {
        return {false, "INVALID_ID", "El id del track debe ser no negativo", -1};
    }

    if (!m_context->eraseTrackById(trackId)) {
        return {false, "NOT_FOUND", "Track no encontrado", trackId};
    }

    return {true, QString(), QString(), trackId};
}

QJsonArray TrackService::serializeTracks() const
{
    auto identityToString = [](const Track& t) -> QString {
        return TrackData::toQString(t.getIdentity());
    };

    QJsonArray arr;
    for (const Track& tr : m_context->getTracks()) {
        QJsonObject trackObj;
        trackObj["id"] = tr.getId();
        trackObj["type"] = TrackData::toQString(tr.getType());
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
    
    return arr;
}
