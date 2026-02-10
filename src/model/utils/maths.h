#pragma once

#include <QtGlobal>
#include <QPointF>
#include <QDateTime>

namespace RadarMath {

// ---------------------------
// Constantes y utilitarios
// ---------------------------

constexpr double kEpsilon = 1e-9;

double normalizeDeg360(double deg);   // -> [0,360)
double normalizeDeg180(double deg);   // -> (-180,180]
double clamp(double v, double lo, double hi);

// ---------------------------
// Conversión de unidades
// ---------------------------
// nmPerDm = cuántas millas náuticas representa 1 Data Mile (DM) en pantalla.
// Ej: si 1 DM = 0.25 NM => nmPerDm = 0.25
double dmToNm(double dm, double nmPerDm);
double nmToDm(double nm, double nmPerDm);

// ---------------------------
// Geometría en plano radar
// ---------------------------
// Convención asumida:
// - Centro radar: (0,0)
// - +X hacia la derecha (Este), +Y hacia arriba (Norte)
// - Azimut/Bearing: 0° = Norte (+Y), 90° = Este (+X), sentido horario
double distanceDm(const QPointF& a, const QPointF& b = QPointF(0.0, 0.0));
double distanceNm(const QPointF& a, const QPointF& b, double nmPerDm);

double bearingDeg(const QPointF& from, const QPointF& to); // [0..360)
double relativeBearingDeg(double ownHeadingDeg, double targetBearingDeg); // [-180..180]

// ---------------------------
// Cinemática de track
// ---------------------------

struct Kinematics {
    double courseDeg = 0.0;   // rumbo (COG) [0..360)
    double speedKnots = 0.0;  // knots
    bool valid = false;
};

// Calcula rumbo+velocidad a partir de dos mediciones.
// t0/t1 en ms desde epoch (QDateTime::toMSecsSinceEpoch()).
Kinematics kinematicsFromSamples(
    const QPointF& p0_dm, qint64 t0_ms,
    const QPointF& p1_dm, qint64 t1_ms,
    double nmPerDm
    );

} // namespace RadarMath
