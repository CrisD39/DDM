#ifndef FIGURESDISPATCHER_H
#define FIGURESDISPATCHER_H

#include <QObject>
#include <QPair>
#include <QtGlobal>

#include "commandContext.h"
#include "model/entities/track.h"
#include "enums/enums.h"   // mismo include que usa Track.h

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

    explicit FiguresDispatcher(CommandContext *ctx,
                               QObject *parent = nullptr);

    // Creación de figuras
    void createCircle(const Circle &circle);
    void createRectangle();
    void createArea();

private:
    CommandContext *m_ctx{nullptr};
};

#endif // FIGURESDISPATCHER_H
