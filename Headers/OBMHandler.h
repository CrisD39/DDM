#ifndef OBMHANDLER_H
#define OBMHANDLER_H

#include "Commands/IOBMHandler.h"
#include <cmath>

class OBMHandler : public IOBMHandler {
    Q_OBJECT

public:
    OBMHandler();

    void updatePosition(QPair<float, float> newPosition) override;
    QPair<float, float> getPosition() override;
    void updateRange(int newRange) override;
signals:
    void newOBMPosition();
};

#endif // OBMHANDLER_H
