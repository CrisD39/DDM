#pragma once

namespace RadarMath {

// Calcula la distancia en Data Miles (DM)
// desde el origen (0,0) hasta el punto (x, y).
double distanceDm(double x, double y);

double azimuthDeg(double x, double y);

} // namespace RadarMath
