// Track.h
#pragma once
#include <QString>
#include "enums/enums.h"

class Track {
public:
    Track() = default;
    Track(int id, Type type, const QString& identity, double x, double y);

    // Getters
    int id() const;
    Track type() const;
    const QString& identity() const;
    double x() const;
    double y() const;

    // Setters (sin noexcept)
    void setId(int id);
    void setType(Track type);
    void setIdentity(const QString& identity);
    void setIdentity(QString&& identity); // overload move
    void setX(double x);
    void setY(double y);

private:
    int m_id{0};
    Identity m_identity{Identity::Pending};     // default como en tu struct original
    Type m_type;                      // vacío = “no informado”
    double m_x{0.0};
    double m_y{0.0};
};
