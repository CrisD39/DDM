#pragma once
#include <QPair>
#include <QString>
#include <QMetaType>
#include <qfloat16.h>
#include <QtGlobal>   // qreal si lo necesitás
#include <utility>

class CursorEntity {
    Q_GADGET
public:
    // --- Ctors ---
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
        lineType(lineType),
        cursorId(id),
        state(value) {}

    // --- Copia / Move habilitados ---
    CursorEntity(const CursorEntity&)            = default;
    CursorEntity& operator=(const CursorEntity&) = default;
    CursorEntity(CursorEntity&&)                 = default;
    CursorEntity& operator=(CursorEntity&&)      = default;

    // --- Getters (nombres y tipos originales) ---
    QPair<qfloat16, qfloat16> getCoordinates() const { return coordinates; }  // :contentReference[oaicite:2]{index=2}
    qfloat16 getCursorAngle()  const { return cursorAngle; }                  // :contentReference[oaicite:3]{index=3}
    qfloat16 getCursorLength() const { return cursorLength; }                 // :contentReference[oaicite:4]{index=4}
    int      getLineType()     const { return lineType; }                    // :contentReference[oaicite:5]{index=5}
    int      getCursorId()     const { return cursorId; }                    // :contentReference[oaicite:6]{index=6}

    // --- Setters (nombres y firmas originales) ---
    void setCursorId(int id)                                         { cursorId = id; }                          // :contentReference[oaicite:7]{index=7}
    void setCoordinates(const QPair<qfloat16, qfloat16>& c)          { coordinates = c; }                         // :contentReference[oaicite:8]{index=8}
    void setCursorAngle(qfloat16 angle)                              { cursorAngle = angle; }                     // :contentReference[oaicite:9]{index=9}
    void setCursorLength(qfloat16 length)                            { cursorLength = length; }                   // :contentReference[oaicite:10]{index=10}
    void setLineType(int lt)                                         { lineType= lt; }    // :contentReference[oaicite:11]{index=11}
    void setActive(bool value)                                       { state = value;}
    void switchActive()                                              { state = !state; }
    bool isActive() const                                            { return state;}
    // --- Utilidad (igual que antes) ---
    QString toString() const {
        return QString("Cursor{id=%1, lt=%2, pos=(%3,%4), ang=%5, len=%6}")
        .arg(cursorId).arg(lineType)
            .arg(double(coordinates.first)).arg(double(coordinates.second))
            .arg(double(cursorAngle)).arg(double(cursorLength));
    }


private:
    QPair<qfloat16, qfloat16> coordinates{};
    qfloat16 cursorAngle{};
    qfloat16 cursorLength{};
    int      lineType{};   // mismo nombre lógico, sin choque con macro
    int      cursorId{};
    bool     state{};
    // Nota: renombré el miembro a lineType_ / cursorId_ arriba para no chocar
    // con los getters y mantener claridad.
};

Q_DECLARE_METATYPE(CursorEntity)
