#include "maths.h"
#include <cmath>

/*
Usamos namespace y no class porque esto que estamos escribiendo es:
un conjunto de funciones matemáticas puras, sin estado
*/

namespace RadarMath {

static double normalizeDeg360(double deg)
{
    double x = std::fmod(deg, 360.0);
    if (x < 0.0) x += 360.0;
    // evita devolver 360 por errores numéricos
    if (x >= 360.0) x -= 360.0;
    return x;
}

double distanceDm(double x, double y)
{
    // Distancia euclídea en Data Miles
    return std::sqrt((x * x) + (y * y));
}

} // namespace RadarMath

double azimuthDeg(double x, double y) {
    // Si el punto es el origen, el azimut es indefinido.
    // Por convención devolvemos 0°.
    if (x == 0.0 && y == 0.0)
        return 0.0;

    // OJO: usamos atan2(x, y) (invertido a propósito)
    // para que 0° sea Norte (+Y) y crezca en sentido horario.
    const double rad = std::atan2(x, y);
    const double deg = rad * (180.0 / M_PI);

    return RadarMath::normalizeDeg360(deg);
}
