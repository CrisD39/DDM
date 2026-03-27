#pragma once

#include "repositories/interfaces/IAreaRepository.h"
#include "repositories/interfaces/ICircleRepository.h"
#include "repositories/interfaces/ICursorRepository.h"

class DomainCleanupService {
public:
    DomainCleanupService(IAreaRepository& areas,
                         ICircleRepository& circles,
                         ICursorRepository& cursors);

    bool deleteAreaWithCursors(int areaId);
    bool deleteCircleWithCursors(int circleId);

private:
    IAreaRepository& m_areas;
    ICircleRepository& m_circles;
    ICursorRepository& m_cursors;
};
