#include "cursorEntity.h"
#include "qdebug.h"

// Constructor
CursorEntity::CursorEntity(
    QPair<qfloat16, qfloat16> coordinates,
    qfloat16 cursorAngle,
    qfloat16 cursorLength,
    int lineType
    ): coordinates(coordinates),
    cursorAngle(cursorAngle),
    cursorLength(cursorLength),
    lineType(lineType)
{
}

// Getters
QPair<qfloat16, qfloat16> CursorEntity::getCoordinates() const {
    return coordinates;
}

qfloat16 CursorEntity::getCursorAngle() const {
    return cursorAngle;
}

qfloat16 CursorEntity::getCursorLength() const {
    return cursorLength;
}

int CursorEntity::getLineType() const {
    return lineType;
}

void CursorEntity::setCursorId(int id)
{
    cursorId = id;
}

// Setters
void CursorEntity::setCoordinates(QPair<qfloat16, qfloat16> newCoordinates) {
    coordinates = newCoordinates;
}

void CursorEntity::setCursorAngle(qfloat16 newCursorAngle) {
    cursorAngle = newCursorAngle;
}

void CursorEntity::setCursorLength(qfloat16 newCursorLength) {
    cursorLength = newCursorLength;
}

void CursorEntity::setLineType(int newLineType) {
    lineType = newLineType;
}

void CursorEntity::toString(){
    qDebug() << "--------------------------------";
    qDebug() << "COORDENADAS: " << coordinates.first << " " << coordinates.second;
    qDebug() << "ANGULO: " << cursorAngle;
    qDebug() << "LONGITUD: " << cursorLength;
    qDebug() << "TIPO DE LINEA: " << lineType;
}

bool CursorEntity::operator==(const CursorMessage &other) const{
    return
        qFuzzyCompare(coordinates.first, other.coordinates.first) &&
        qFuzzyCompare(coordinates.second, other.coordinates.second) &&
        qFuzzyCompare(cursorAngle, other.cursorAngle) &&
        qFuzzyCompare(cursorLength, other.cursorLength) &&
        lineType == other.lineType;
}
