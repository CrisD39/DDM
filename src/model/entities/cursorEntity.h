#ifndef CURSORENTITY_H
#define CURSORENTITY_H

#include <qpair.h>
#include <qfloat16.h>
#include <qstring.h>
#include <QMetaType>

class CursorEntity
{
private:
    QPair<qfloat16,qfloat16> coordinate;
    qfloat16 cursorAngle;
    qfloat16 cursorLenght;
    int lineType;
    int cursorId;

    public:
    CursorEntity(): lineType(0){}
    ~CursorEntity() = default;
    CursorEntity(const CursorEntity &) = default;

    // Constructor
    CursorEntity(
        QPair<qfloat16, qfloat16> coordinates,
        qfloat16 cursorAngle,
        qfloat16 cursorLength,
        int lineType
        );

    // Getters
    QPair<qfloat16, qfloat16> getCoordinates() const;
    qfloat16 getCursorAngle() const;
    qfloat16 getCursorLength() const;
    int getLineType() const;

    // Setters
    void setCursorId(int id);
    void setCoordinates(QPair<qfloat16, qfloat16> coordinates);
    void setCursorAngle(qfloat16 cursorAngle);
    void setCursorLength(qfloat16 cursorLength);
    void setLineType(int lineType);

    void toString();

    bool operator==(const CursorEntity &other) const;

};

Q_DECLARE_METATYPE(CursorMessage)
#endif // CURSORENTITY_H
