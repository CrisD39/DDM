#pragma once
#include <QString>
#include "enums/enumsTrack.h"

class Track {

    using Type      = TrackData::Type;
    using Identity  = TrackData::Identity;
    using TrackMode = TrackData::TrackMode;

public:
    Track() = default;

    Track(int id,
          Type type,
          Identity identity,
          TrackMode mode,
          double x,
          double y,
          double rumbo = 0.0,
          double velocidad = 0.0);

    // Getters
    int getId() const;
    Type getType() const;
    Identity getIdentity() const;
    TrackMode getTrackMode() const;
    double getX() const;
    double getY() const;
    double getRumbo() const;        // NUEVO
    double getVelocidad() const;    // NUEVO

    // Setters
    void setId(int id);
    void setType(Type type);
    void setIdentity(Identity identity);
    void setTrackMode(TrackMode mode);
    void setX(double x);
    void setY(double y);
    void setRumbo(double r);        // NUEVO
    void setVelocidad(double v);    // NUEVO

    QString toString() const;

private:
    int m_id{0};
    Identity m_identity{Identity::Pending};
    Type m_type;
    TrackMode m_mode;

    double m_x{0.0};
    double m_y{0.0};

    double m_rumbo{0.0};      // grados 0–360
    double m_velocidad{0.0};  // nudos
};
