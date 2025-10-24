// Track.cpp
#include "Track.h"

// Ctor completo
Track::Track(
    int id,
    Type type,
    Identity identity,
    TrackMode mode,
    double x,
    double y
    ): m_id(id), m_type(type), m_mode(mode), m_identity(identity), m_x(x), m_y(y) {}

// Getters
    int Track::getId() const { return m_id; }
    TrackData::Type Track::getType() const { return m_type;}
    TrackData::Identity Track::getIdentity() const { return m_identity; }
    TrackData::TrackMode Track::getTrackMode() const { return m_mode; }
    double Track::getX() const { return m_x; }
    double Track::getY() const { return m_y; }

// Setters
    void Track::setId(int id) { m_id = id; }
    void Track::setType(Type type) { m_type = type; }
    void Track::setIdentity(Identity identity) { m_identity = identity; }
    void Track::setTrackMode(TrackMode mode) { m_mode = mode; }
    void Track::setX(double x) { m_x = x; }
    void Track::setY(double y) { m_y = y; }
