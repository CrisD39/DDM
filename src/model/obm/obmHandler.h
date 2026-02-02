#ifndef OBMHANDLER_H
#define OBMHANDLER_H

#include "commandContext.h"
#include "entities/track.h"
#include "iOBMHandler.h"
#include <cmath>

class OBMHandler : public IOBMHandler {
    Q_OBJECT

public:
    OBMHandler();

    void updatePosition(QPair<float, float> newPosition) override;
    QPair<float, float> getPosition() override;
    void updateRange(int newRange) override;
    double getDistanceFromTrack(const Track& t) const;
    Track* OBMAssociationProcess(CommandContext* ctx);
    void setPosition(QPair<float, float> newPosition);
signals:
    void newOBMPosition();
};

#endif // OBMHANDLER_H
