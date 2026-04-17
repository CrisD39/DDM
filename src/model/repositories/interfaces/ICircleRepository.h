#pragma once

#include "entities/circleEntity.h"

class ICircleRepository {
public:
    virtual ~ICircleRepository() = default;
    virtual CircleEntity* findById(int id) = 0;
    virtual bool eraseById(int id) = 0;
};
