#ifndef FIGURESDISPATCHER_H
#define FIGURESDISPATCHER_H

#include <QObject>
#include <QPair>
#include <QtGlobal>

#include "commandContext.h"
#include "model/entities/track.h"
#include "enumsTrack.h"   // mismo include que usa Track.h
#include "ownship/ownship.h"
#include <QtMath>

class FiguresDispatcher : public QObject
{
    Q_OBJECT

public:
    struct Circle {
        double radio;                               // radio del círculo
        QPair<qfloat16, qfloat16> center;           // centro (x, y)
        TrackData::Type type;                       // tipo de track
        int color;                                  // color (por ahora no lo usamos acá)
    };

    struct Rectangle {
        // Puntos geográficos (Lat, Lon)
        QPair<QString, QString> pointA; // {Lat_A, Lon_A}
        QPair<QString, QString> pointB; // {Lat_B, Lon_B}
        QPair<QString, QString> pointC; // {Lat_C, Lon_C}
        QPair<QString, QString> pointD; // {Lat_D, Lon_D}
        TrackData::Type type;           // Tipo de track
        int color;                      // Color
    };


    explicit FiguresDispatcher(CommandContext *ctx, OwnShip *ownshipper,
                               QObject *parent = nullptr);

    // Creación de figuras
    void createCircle(const Circle &circle);
    void createArea(const Rectangle &rect);


    QPair<double, double> geoToRelative(const QString &lat, const QString &lon) const;
private:
    OwnShip *ownship;
    CommandContext *m_ctx{nullptr};
    double geoToDecimal(const QString& geoStr);
};

#endif // FIGURESDISPATCHER_H
