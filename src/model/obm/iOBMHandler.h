#ifndef IOBMHANDLER_H
#define IOBMHANDLER_H

#include <QPair>
#include <QObject>

// Estructura para representar la posición del OBM
struct OBMPosition {
    float xPosition = 0.0f;
    float yPosition = 0.0f;
};

// Interfaz base para manejar la posición del OBM
class IOBMHandler : public QObject
{
    Q_OBJECT

public:
    virtual void updatePosition(QPair<float, float> newPosition) = 0;
    virtual QPair<float, float> getPosition() = 0;
    virtual void updateRange(int newRange) = 0;

protected:
    OBMPosition obmPosition;
    int range = 1;
};

#endif // IOBMHANDLER_H
