#pragma once
#include <QPair>
#include <QString>
#include <QMetaType>
#include <qfloat16.h>

class CursorEntity {
    Q_GADGET
public:
    CursorEntity() = default;

    CursorEntity(QPair<qfloat16, qfloat16> coords,
                 qfloat16 angle,
                 qfloat16 length,
                 int      lineType,
                 int      id = 0,
                 bool     value = false)
        : coordinates(coords),
        cursorAngle(angle),
        cursorLength(length),
        cursorId(id),
        lineType(lineType),
        state(value) {}

    // --- Copia / Move habilitados ---
    CursorEntity(const CursorEntity&)            = default;
    CursorEntity& operator=(const CursorEntity&) = default;
    CursorEntity(CursorEntity&&)                 = default;
    CursorEntity& operator=(CursorEntity&&)      = default;

    // --- Getters (nombres y tipos originales) ---
    QPair<qfloat16, qfloat16> getCoordinates() const { return coordinates; }
    qfloat16 getCursorAngle()  const { return cursorAngle; }
    qfloat16 getCursorLength() const { return cursorLength; }
    int      getLineType()     const { return lineType; }
    int      getCursorId()     const { return cursorId; }

    // --- Setters (nombres y firmas originales) ---
    void setCursorId(int id)                                         { cursorId = id; }
    void setCoordinates(const QPair<qfloat16, qfloat16> c)           { coordinates = c; }
    void setCursorAngle(qfloat16 angle)                              { cursorAngle = angle; }
    void setCursorLength(qfloat16 length)                            { cursorLength = length; }
    void setLineType(int lt)                                         { lineType= lt; }
    void setActive(bool value)                                       { state = value;}
    void switchActive()                                              { state = !state; }
    bool isActive() const                                            { return state;}


    QString toString() const {
        return QString("Cursor{id=%1, lt=%2, pos=(%3,%4), ang=%5, len=%6}")
        .arg(cursorId).arg(lineType)
            .arg(double(coordinates.first)).arg(double(coordinates.second))
            .arg(double(cursorAngle)).arg(double(cursorLength));
    }


private:
    // Bloque 1: 8 bytes (Geometría compacta)
    QPair<qfloat16, qfloat16> coordinates;
    qfloat16 cursorAngle;
    qfloat16 cursorLength;

    // Bloque 2: 8 bytes (Datos administrativos)
    int      cursorId;      // 4 bytes
    uint8_t  lineType;      // 1 byte (suficiente para 0-255)
    bool     state;         // 1 byte
    uint8_t  _padding[2];   // 2 bytes explícitos o implícitos para cerrar en 8.

};

Q_DECLARE_METATYPE(CursorEntity)
