#ifndef CURSORENTITY_H
#define CURSORENTITY_H

#include <QObject>    // ← necesario: hereda de QObject
#include <QPair>      // ← usa QPair
#include <qfloat16.h> // ← si tu Qt no trae qfloat16, cambia a float/qreal
#include <QString>
#include <QMetaType>

class CursorEntity : public QObject {
    Q_OBJECT

public:
    // Constructores (QObject no es copiable: borrar copias)
    explicit CursorEntity(QObject* parent = nullptr)
        : QObject(parent), coordinates{}, cursorAngle(0), cursorLength(0), lineType(0), cursorId(0) {}

    CursorEntity(QPair<qfloat16, qfloat16> coords,
                 qfloat16 angle,
                 qfloat16 length,
                 int lineType,
                 QObject* parent = nullptr)
        : QObject(parent), coordinates(coords), cursorAngle(angle),
        cursorLength(length), lineType(lineType), cursorId(0) {}

    CursorEntity(const CursorEntity&)            = delete; // QObject no permite copia
    CursorEntity& operator=(const CursorEntity&) = delete;
    CursorEntity(CursorEntity&&)                 = delete;
    CursorEntity& operator=(CursorEntity&&)      = delete;

    // Getters
    QPair<qfloat16, qfloat16> getCoordinates() const { return coordinates; }
    qfloat16 getCursorAngle()  const { return cursorAngle; }
    qfloat16 getCursorLength() const { return cursorLength; }
    int      getLineType()     const { return lineType; }
    int      getCursorId()     const { return cursorId; }

    // Setters
    void setCursorId(int id)                                   { cursorId = id; }
    void setCoordinates(const QPair<qfloat16, qfloat16>& c)    { coordinates = c; }
    void setCursorAngle(qfloat16 angle)                        { cursorAngle = angle; }
    void setCursorLength(qfloat16 length)                      { cursorLength = length; }
    void setLineType(int lt)                                   { lineType = lt; }

    // Utilidad
    QString toString() const; // mejor devolver QString

    bool operator==(const CursorEntity& other) const {
        return cursorId == other.cursorId
               && lineType == other.lineType
               && coordinates == other.coordinates
               && cursorAngle == other.cursorAngle
               && cursorLength == other.cursorLength;
    }

private:
    QPair<qfloat16, qfloat16> coordinates{};
    qfloat16 cursorAngle{};
    qfloat16 cursorLength{};
    qint8      lineType{};
    int      cursorId{};
};

// Registrar el *puntero* (los QObject no se registran por valor)
Q_DECLARE_METATYPE(CursorEntity*)

#endif // CURSORENTITY_H
