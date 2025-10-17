// Track.cpp
#include "Track.h"

// Ctor completo
Track::Track(int id, Track type, const QString& identity, double x, double y)
    : m_id(id), m_type(type), m_identity(identity), m_x(x), m_y(y) {}

// Getters
int Track::id() const { return m_id; }
Track Track::type() const { return m_type; }
const QString& Track::identity() const { return m_identity; }
double Track::x() const { return m_x; }
double Track::y() const { return m_y; }

// Setters
void Track::setId(int id) { m_id = id; }
void Track::setType(Track type) { m_type = type; }
void Track::setIdentity(const QString& identity) { m_identity = identity; }
void Track::setIdentity(QString&& identity) { m_identity = std::move(identity); }
void Track::setX(double x) { m_x = x; }
void Track::setY(double y) { m_y = y; }
