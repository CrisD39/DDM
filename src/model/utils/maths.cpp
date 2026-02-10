#include "maths.h"
#include <QtGlobal>
#include <QPointF>
#include <QDateTime>

namespace RadarMath {

double normalizeDeg360(double deg)
{
    double x = std::fmod(deg, 360.0);
    if (x < 0.0) x += 360.0;
    // Evitar 360.0 exacto por errores numéricos
    if (x >= 360.0) x -= 360.0;
    return x;
}

double normalizeDeg180(double deg)
{
    double x = normalizeDeg360(deg);
    if (x > 180.0) x -= 360.0;
    return x;
}

double clamp(double v, double lo, double hi)
{
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

// ---------------------------
// Unidades
// ---------------------------
double dmToNm(double dm, double nmPerDm)
{
    return dm * nmPerDm;
}

double nmToDm(double nm, double nmPerDm)
{
    if (qAbs(nmPerDm) < kEpsilon) return 0.0;
    return nm / nmPerDm;
}

// ---------------------------
// Geometría
// ---------------------------
double distanceDm(const QPointF& a, const QPointF& b)
{
    const double dx = a.x() - b.x();
    const double dy = a.y() - b.y();
    return std::hypot(dx, dy);
}

double distanceNm(const QPointF& a, const QPointF& b, double nmPerDm)
{
    return dmToNm(distanceDm(a, b), nmPerDm);
}

double bearingDeg(const QPointF& from, const QPointF& to)
{
    // Bearing clásico: atan2(Easting, Northing)
    // dx = Este (x), dy = Norte (y)
    const double dx = to.x() - from.x();
    const double dy = to.y() - from.y();

    // Si es el mismo punto, por convención 0°
    if (qAbs(dx) < kEpsilon && qAbs(dy) < kEpsilon)
        return 0.0;

    const double rad = std::atan2(dx, dy); // <-- ojo: (x,y) invertidos a propósito
    const double deg = qRadiansToDegrees(rad);
    return normalizeDeg360(deg);
}

double relativeBearingDeg(double ownHeadingDeg, double targetBearingDeg)
{
    // Relativo = (bearing - heading) normalizado a [-180,180]
    return normalizeDeg180(targetBearingDeg - ownHeadingDeg);
}

// ---------------------------
// Cinemática
// ---------------------------
Kinematics kinematicsFromSamples(
    const QPointF& p0_dm, qint64 t0_ms,
    const QPointF& p1_dm, qint64 t1_ms,
    double nmPerDm
    ){
    Kinematics out;

    const qint64 dt_ms = t1_ms - t0_ms;
    if (dt_ms <= 0) {
        out.valid = false;
        return out;
    }

    const double dt_h = static_cast<double>(dt_ms) / 3600000.0; // ms -> horas
    if (dt_h < kEpsilon) {
        out.valid = false;
        return out;
    }

    const double dist_nm = distanceNm(p0_dm, p1_dm, nmPerDm);
    const double cog_deg = bearingDeg(p0_dm, p1_dm);
    const double spd_kn = dist_nm / dt_h;

    out.courseDeg = cog_deg;
    out.speedKnots = spd_kn;
    out.valid = true;
    return out;
}

} // namespace RadarMath
