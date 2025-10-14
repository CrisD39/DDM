#include "OBMHandler.h"
#include "qdebug.h"
#include <cmath>

OBMHandler::OBMHandler() {
    obmPosition = {0.0f, 0.0f};
    range = 4;
}

void OBMHandler::updatePosition(QPair<float, float> newPosition)
{
    float valorX = newPosition.first / 64.0f;
    float valorY = newPosition.second / 64.0f;

    // Mantener el signo en ambos ejes
    float deltaPositionX = std::copysign(std::pow((std::abs(valorX)),2), valorX) * range;
    float deltaPositionY = std::copysign(std::pow((std::abs(valorY)),2), valorY) * range;

    obmPosition.xPosition += deltaPositionX;
    obmPosition.yPosition += deltaPositionY;

}

QPair<float, float> OBMHandler::getPosition()
{
    return { obmPosition.xPosition, obmPosition.yPosition };
}

void OBMHandler::updateRange(int newRange)
{
    if(newRange != range){
        range = newRange;
        qDebug() << "new Range for the boys" << range;
    }
}
