#pragma once

#include "enums.h"     // TrackData::Type / Identity / TrackMode
#include <QString>

class Track {
    // Alias para escribir menos
    using Type      = TrackData::Type;
    using Identity  = TrackData::Identity;
    using TrackMode = TrackData::TrackMode;

public:
    Track() = default;

    // Constructor
    Track(int id, Type type, Identity identity, TrackMode mode, float x, float y)
        : m_id(id)
        , m_type(static_cast<uint8_t>(type))
        , m_identity(static_cast<uint8_t>(identity))
        , m_mode(static_cast<uint8_t>(mode))
        , m_x(x)
        , m_y(y)
    {}

    // --- Getters ---
    int getId() const              { return m_id; }
    Type getType() const           { return static_cast<Type>(m_type); }
    Identity getIdentity() const   { return static_cast<Identity>(m_identity); }
    TrackMode getTrackMode() const { return static_cast<TrackMode>(m_mode); }

    float getX() const             { return m_x; }   // DM
    float getY() const             { return m_y; }   // DM

    // --- Derivados (todo en DM) ---
    // Azimut desde (0,0) hacia (x,y) en grados [0,360)
    double getAzimuthDeg() const;

    // Distancia desde (0,0) hacia (x,y) en Data Miles (DM)
    double getDistanceDm() const;

    // --- Setters ---
    void setId(int id)                 { m_id = id; }
    void setType(Type t)               { m_type = static_cast<uint8_t>(t); }
    void setIdentity(Identity i)       { m_identity = static_cast<uint8_t>(i); }
    void setTrackMode(TrackMode m)     { m_mode = static_cast<uint8_t>(m); }
    void setX(float x)                 { m_x = x; }
    void setY(float y)                 { m_y = y; }

    QString toString() const;

private:
    int32_t m_id{0};
    uint8_t m_type{0};
    uint8_t m_identity{0};
    uint8_t m_mode{0};
    uint8_t _padding{0};

    float m_x{0.0f}; // DM
    float m_y{0.0f}; // DM
};
