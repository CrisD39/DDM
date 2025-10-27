#include "owncurs.h"

OwnCurs::OwnCurs(CommandContext* context, OBMHandler* newObm, QObject* parent)
    : QObject(parent),
    cursor(new CursorEntity(this)),   // CursorEntity hereda de QObject ⇒ dale parent
    ctx(context),
    obm(newObm)
{
    // Si querés valores iniciales específicos, podés setearlos acá.
    // p.ej.: cursor->setCoordinates(QPair<qfloat16,qfloat16>(0,0));
}

void OwnCurs::cuOrOffCent()
{
    if (!cursor || !obm) return;
    // Asumo que OBMHandler::getPosition() devuelve QPair<qfloat16,qfloat16>
    cursor->setCoordinates(obm->getPosition());
}

void OwnCurs::updateHandwheel(const QPair<qfloat16, qfloat16>& update)
{
    if (!cursor) return;
    cursor->setCursorAngle(update.first);   // ángulo
    cursor->setCursorLength(update.second); // largo
}
