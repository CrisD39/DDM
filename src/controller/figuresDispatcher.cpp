#include "figuresDispatcher.h"
#include <QtMath>

FiguresDispatcher::FiguresDispatcher(CommandContext *ctx, OwnShip* ownshipper,QObject *parent)
    : QObject(parent),
    m_ctx(ctx)
{
    ownship = ownshipper;
}

void FiguresDispatcher::createCircle(const Circle &circle)
{
    if (!m_ctx) {
        qDebug()<<"no pude crear el circulo";
        return;
    }
    qDebug()<<"Hola en figures";
    const double radius = circle.radio;
    const double cx = static_cast<double>(circle.center.first);
    const double cy = static_cast<double>(circle.center.second);
    const auto type = circle.type;
    Q_UNUSED(circle.color);   // por ahora no lo usamos

    constexpr int kNumTracks = 12;
    const double angleStep = 2.0 * M_PI / static_cast<double>(kNumTracks);

    int nextId = 0;   // ajustalo si tu CommandContext ya maneja IDs de otra forma

    for (int i = 0; i < kNumTracks; ++i) {
        const double angle = i * angleStep;

        const double x = cx + radius * qCos(angle);
        const double y = cy + radius * qSin(angle);

        // Constructor de Track: Track(int id, Type type, Identity identity, TrackMode, double x, double y)
        Track track(
            nextId++,
            type,
            TrackData::Identity::ConfHostile,
            TrackData::TrackMode::FC1,
            x,
            y
            );

        m_ctx->emplaceTrackFront(track);
    }
}

void FiguresDispatcher::createArea(const Rectangle &rect)
{
    if (!m_ctx) {
        qDebug() << "no pude crear el rectangulo: CommandContext no válido";
        return;
    }

    qDebug() << "Creando rectángulo...";

    // 1. Obtener la posición geográfica del Own Ship (Ejemplo: asumir acceso)
    // ⚠️ Tendrías que implementar una forma de obtener la Lat/Lon del Own Ship.
    // Por ejemplo, si CommandContext tiene un getter para OwnShip:
    // const OwnShip& os = m_ctx->getOwnShip();
    // const QString osLat = os.getLat();
    // const QString osLon = os.getLon();

    // ⚠️ PLACHOLDER para OwnShip Lat/Lon:
    const QString osLat = "40-00.0N";
    const QString osLon = "070-00.0W";

    // 2. Definir los puntos a convertir y el tipo
    QList<QPair<QString, QString>> geoPoints;
    geoPoints << rect.pointA << rect.pointB << rect.pointC << rect.pointD;

    int nextId = 100; // ID de ejemplo para los tracks del rectángulo

    for (const auto& geo : geoPoints) {
        const QString lat = geo.first;
        const QString lon = geo.second;

        // 3. Convertir Lat/Lon a coordenadas relativas (X, Y)
        QPair<double, double> relativeCoords = geoToRelative(lat, lon);
        const double x = relativeCoords.first;
        const double y = relativeCoords.second;

        // 4. Crear un Track en las coordenadas relativas
        // Solo crearemos los tracks de las esquinas del rectángulo.
        Track track(
            nextId++,
            rect.type,
            TrackData::Identity::ConfHostile, // O la identidad que desees
            TrackData::TrackMode::FC1,       // O el modo que desees
            x,
            y
            );

        // 5. Agregar el track al CommandContext
        m_ctx->emplaceTrackFront(track);
        qDebug() << "Track creado en relativo (X,Y):" << x << "," << y;
    }
}


QPair<double, double> FiguresDispatcher::geoToRelative(const QString& latStr,
                                                       const QString& lonStr) const
{
    // Verificar si el OwnShip es accesible y válido
    if (!ownship) {
        qDebug() << "Error: OwnShip no inicializado en FiguresDispatcher.";
        return qMakePair(0.0, 0.0);
    }

    // 1. Obtener la posición del Own Ship
    // ⚠️ Debes implementar getLat() y getLon() en tu clase OwnShip
    const QString osLatStr = ownship->getLat();
    const QString osLonStr = ownship->getLon();


    // 2. Convertir todas las coordenadas a grados decimales (double)
    // ⚠️ Asumiendo que existe una función geoToDecimal(QString)
    const double ownLatRad = qDegreesToRadians(geoToDecimal(osLatStr)); // OwnShip Lat
    const double ownLonRad = qDegreesToRadians(geoToDecimal(osLonStr)); // OwnShip Lon
    const double pointLatRad = qDegreesToRadians(geoToDecimal(latStr)); // Punto Lat
    const double pointLonRad = qDegreesToRadians(geoToDecimal(lonStr)); // Punto Lon

    // --- CÁLCULO DE DISTANCIA Y RUMBO (FÓRMULA DEL CÍRCULO MÁXIMO) ---

    // 3. Diferencias
    const double dLon = pointLonRad - ownLonRad;
    const double dLat = pointLatRad - ownLatRad;

    // 4. Calcular Distancia (Distancia a través de la Fórmula de Haversine)
    const double a = qSin(dLat/2.0) * qSin(dLat/2.0) +
                     qCos(ownLatRad) * qCos(pointLatRad) *
                         qSin(dLon/2.0) * qSin(dLon/2.0);
    const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));

    // Radio promedio de la Tierra en metros (por ejemplo, 6371000m)
    constexpr double kEarthRadiusMeters = 6371000.0;
    const double distanceMeters = kEarthRadiusMeters * c; // Distancia en metros

    // 5. Calcular Rumbo (para obtener la dirección)
    const double y = qSin(dLon) * qCos(pointLatRad);
    const double x = qCos(ownLatRad) * qSin(pointLatRad) -
                     qSin(ownLatRad) * qCos(pointLatRad) * qCos(dLon);

    // El 'bearing' es el ángulo desde el Norte (0 a 360 grados)
    const double bearingRad = qAtan2(y, x);

    // 6. Convertir Distancia y Rumbo a Coordenadas Relativas (X, Y)
    // En navegación, Y es Norte/Sur (vertical) y X es Este/Oeste (horizontal)
    // Coordenadas relativas (X, Y) en metros:
    const double relativeX = distanceMeters * qSin(bearingRad); // Proyección Este/Oeste
    const double relativeY = distanceMeters * qCos(bearingRad); // Proyección Norte/Sur

    qDebug() << "Convirtiendo a relativo (X,Y) en metros:" << relativeX << "," << relativeY;

    return qMakePair(relativeX, relativeY);
}

// ⚠️ NECESITARÍAS ESTA FUNCIÓN DE UTILIDAD EN ALGÚN LADO
// (La ponemos aquí a modo de ejemplo, pero podría estar en un archivo de utilidades)
/**
 * @brief Convierte una cadena de coordenadas geográficas (Lat/Lon) a grados decimales.
 * @param geoStr La cadena en formato "DD-MM.M{N|S}" o "DDD-MM.M{E|W}".
 * @return El valor en grados decimales (double).
 */
double FiguresDispatcher::geoToDecimal(const QString& geoStr)
{
    // Verifica si la cadena está vacía
    if (geoStr.isEmpty()) {
        qDebug() << "Error en geoToDecimal: Cadena vacía.";
        return 0.0;
    }

    // El último carácter indica la dirección (N, S, E, W)
    const QChar direction = geoStr.right(1).toUpper().at(0);

    // La parte numérica de la coordenada (ej: "40-00.0" o "070-00.0")
    QString numericPart = geoStr;
    numericPart.chop(1); // Elimina el carácter de dirección

    // Busca la posición del guion que separa Grados y Minutos
    const int dashIndex = numericPart.indexOf('-');

    if (dashIndex == -1) {
        qDebug() << "Error en geoToDecimal: Formato inválido. No se encontró el guion separador.";
        return 0.0;
    }

    // 1. Extraer Grados (DD o DDD)
    const QString degreesStr = numericPart.left(dashIndex);
    bool okDegrees;
    const double degrees = degreesStr.toDouble(&okDegrees);

    // 2. Extraer Minutos (MM.M)
    const QString minutesStr = numericPart.mid(dashIndex + 1);
    bool okMinutes;
    const double minutes = minutesStr(okMinutes);

    if (!okDegrees || !okMinutes) {
        qDebug() << "Error en geoToDecimal: Falló la conversión de grados o minutos a número.";
        return 0.0;
    }

    // 3. Calcular el valor decimal: Grados + (Minutos / 60)
    double decimalValue = degrees + (minutes / 60.0);

    // 4. Aplicar el signo basado en la dirección
    // Suroeste (S, W) son negativas. Noreste (N, E) son positivas.
    if (direction == 'S' || direction == 'W') {
        decimalValue = -decimalValue;
    }

    // Si la dirección es N o E, ya es positiva.

    return decimalValue;
}
