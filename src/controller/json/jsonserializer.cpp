#include "jsonserializer.h"
#include "entities/cursorEntity.h"

QJsonObject JsonSerializer::serializeLine(const CursorEntity& cursor)
{
    QJsonObject lineObj;
    lineObj["id"] = QString("LINE_%1").arg(cursor.getCursorId());
    lineObj["x"] = static_cast<double>(cursor.getCoordinates().first);
    lineObj["y"] = static_cast<double>(cursor.getCoordinates().second);
    lineObj["azimut"] = static_cast<double>(cursor.getCursorAngle());
    lineObj["length"] = static_cast<double>(cursor.getCursorLength());
    lineObj["type"] = cursor.getLineType();
    lineObj["active"] = cursor.isActive();
    
    return lineObj;
}
