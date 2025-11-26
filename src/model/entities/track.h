// Track.h
#pragma once
#include <QString>
#include "enums/enums.h"
#include <QTimer>

#define INTERVAL 5000
class Track {

    using Type      = TrackData::Type;
    using Identity  = TrackData::Identity;
    using TrackMode = TrackData::TrackMode;

public:
    Track() = default;
    Track(int id, Type type, Identity identity, TrackMode, double x, double y);

    // Getters
    int getId() const;
    Type getType() const;
    Identity getIdentity() const;
    TrackMode getTrackMode()const;
    double getX() const;
    double getY() const;
    double getCourse() const;
    double getVelocity() const;

    // Setters (sin noexcept)
    void setId(int id);
    void setType(Type type);
    void setIdentity(Identity identity);
    void setTrackMode(TrackMode mode);
    void setX(double x);
    void setY(double y);
    double setCourse(double c);
    double setVelocity(double v);

    void actualizarPos();

    QString toString() const;
private:
    int m_id{0};
    Identity m_identity{Identity::Pending};     // default como en tu struct original
    Type m_type;                      // vacío = “no informado”
    TrackMode m_mode;

    double m_course;
    double m_velocity;
    QTimer *m_timer;

    double m_x{0.0};
    double m_y{0.0};

};
