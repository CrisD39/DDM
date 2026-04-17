#pragma once

#include "entities/areaEntity.h"

class IAreaRepository {
public:
    virtual ~IAreaRepository() = default;
    virtual AreaEntity* findById(int id) = 0;
    virtual bool eraseById(int id) = 0;
};
