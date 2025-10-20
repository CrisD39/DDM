// Track.h
#pragma once
#include <QString>
#include "enums/enums.h"

class Track {

    using Type      = TrackData::Type;
    using Identity  = TrackData::Identity;
    using TrackMode = TrackData::TrackMode;

public:
    Track() = default;
    Track(int id, Type type, Identity identity, double x, double y);

    // Getters
    int getId() const;
    Type getType() const;
    Identity getIdentity() const;
    TrackMode getTrackMode()const;
    double getX() const;
    double getY() const;

    // Setters (sin noexcept)
    void setId(int id);
    void setType(Type type);
    void setIdentity(Identity identity);
    void setX(double x);
    void setY(double y);

private:
    int m_id{0};
    Identity m_identity{Identity::Pending};     // default como en tu struct original
    Type m_type;                      // vacío = “no informado”

    double m_x{0.0};
    double m_y{0.0};
};
