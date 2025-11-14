#pragma once
#include <QtMath>
#include <QPointF>

namespace Geometry {

// --- Constantes ---
constexpr double PI = 3.14159265358979323846;

// --- Conversiones de Ángulos ---
inline double degToRad(double deg) {
    return deg * PI / 180.0;
}

inline double radToDeg(double rad) {
    return rad * 180.0 / PI;
}

// Normaliza un ángulo a 0..359.99 grados
inline double normalizeDeg(double deg) {
    double d = fmod(deg, 360.0);
    return (d < 0) ? (d + 360.0) : d;
}

// --- Estructura de Vector 2D ---
// (Usaremos esto para los cálculos de posición)
struct Vector2D {
    double x{0.0};
    double y{0.0};

    Vector2D() = default;
    Vector2D(double x, double y) : x(x), y(y) {}

    // --- Operadores ---
    Vector2D operator+(const Vector2D& other) const { return {x + other.x, y + other.y}; }
    Vector2D operator-(const Vector2D& other) const { return {x - other.x, y - other.y}; }
    Vector2D operator*(double scalar) const { return {x * scalar, y * scalar}; }
    Vector2D operator/(double scalar) const { return {x / scalar, y / scalar}; }

    // --- Métodos ---
    double magnitude() const { return qSqrt(x * x + y * y); }
    double magnitudeSq() const { return x * x + y * y; }

    // --- Conversiones Polares (Convención Náutica: 0=Norte, 90=Este) ---
    // (Usamos x=Este, y=Norte)
    static Vector2D fromPolar(double courseDeg, double speed) {
        double angleRad = degToRad(normalizeDeg(courseDeg));
        return {speed * qSin(angleRad), speed * qCos(angleRad)};
    }

    double toPolarCourse() const {
        if (qAbs(x) < 1e-9 && qAbs(y) < 1e-9) return 0.0;
        return normalizeDeg(radToDeg(qAtan2(x, y))); // Atan2(x, y) da 0=Norte
    }

    double toPolarSpeed() const {
        return magnitude();
    }

    // --- Funciones Estáticas ---
    // (Calcula el vector de A hacia B)
    static Vector2D displacement(const Vector2D& posA, const Vector2D& posB) {
        return posB - posA;
    }

    // (Calcula un punto final desde un origen, rumbo y distancia)
    static Vector2D destination(const Vector2D& origin, double courseDeg, double distance) {
        return origin + fromPolar(courseDeg, distance);
    }
};

} // namespace Geometry
