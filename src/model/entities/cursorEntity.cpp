#include "cursorEntity.h"
#include <QString>

QString CursorEntity::toString() const {
    return QString("Cursor{id=%1, lineType=%2, coords=(%3, %4), angle=%5, length=%6}")
    .arg(cursorId)
        .arg(lineType)
        .arg(static_cast<double>(coordinates.first))
        .arg(static_cast<double>(coordinates.second))
        .arg(static_cast<double>(cursorAngle))
        .arg(static_cast<double>(cursorLength));
}
