#pragma once

#include <QJsonArray>
#include <QString>
#include <optional>

#include "enums.h"
#include "entities/track.h"

class CommandContext;

struct TrackCreateRequest {
    TrackData::Type type = TrackData::Surface;
    TrackData::Identity identity = TrackData::Pending;
    TrackData::TrackMode mode = TrackData::Auto;
    double x = 0.0;
    double y = 0.0;

    // Legacy constructor fields.
    double ctorSpeedKnots = 0.0;
    double ctorCourseDeg = 0.0;

    // Optional model overrides.
    std::optional<double> speedDmPerHour;
    std::optional<int> courseDeg;
    std::optional<int> fc;
    std::optional<QString> asgc;
    std::optional<Track::LinkYStatus> linkY;
    std::optional<Track::Link14Status> link14;
    std::optional<QString> info;
    std::optional<QString> priv;
};

struct TrackOperationResult {
    bool success = false;
    QString errorCode;
    QString message;
    int trackId = -1;
};

class TrackService {
public:
    explicit TrackService(CommandContext* context);

    TrackOperationResult createTrack(const TrackCreateRequest& request);
    TrackOperationResult deleteTrackById(int trackId);

    QJsonArray serializeTracks() const;

private:
    CommandContext* m_context;
};
