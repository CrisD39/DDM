#include "figuresDispatcher.h"
#include <QtMath>

FiguresDispatcher::FiguresDispatcher(CommandContext *ctx, QObject *parent)
    : QObject(parent),
    m_ctx(ctx)
{
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

        // ⚠️ IMPORTANTE: acá tenés que llamar al método REAL de tu CommandContext.
        // Ejemplos posibles (vos elegís el correcto según tu código):
        //   m_ctx->emplaceTrackFront(track);
        //   m_ctx->addTrack(track);
        //   m_ctx->pushTrack(track);
        //
        // De momento lo dejo comentado para que no rompa compilación:
        // m_ctx->addTrack(track);
        m_ctx->emplaceTrackFront(track);
    }
}

void FiguresDispatcher::createRectangle()
{
    // TODO: implementar si lo necesitás
}

void FiguresDispatcher::createArea()
{
    // TODO: implementar si lo necesitás
}
