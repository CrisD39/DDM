#include "services/DomainCleanupService.h"

DomainCleanupService::DomainCleanupService(IAreaRepository& areas,
                                           ICircleRepository& circles,
                                           ICursorRepository& cursors)
    : m_areas(areas), m_circles(circles), m_cursors(cursors) {}

bool DomainCleanupService::deleteAreaWithCursors(int areaId) {
    AreaEntity* area = m_areas.findById(areaId);
    if (!area) {
        return false;
    }

    m_cursors.eraseById(area->getCursorIdAB());
    m_cursors.eraseById(area->getCursorIdBC());
    m_cursors.eraseById(area->getCursorIdCD());
    m_cursors.eraseById(area->getCursorIdDA());

    return m_areas.eraseById(areaId);
}

bool DomainCleanupService::deleteCircleWithCursors(int circleId) {
    CircleEntity* circle = m_circles.findById(circleId);
    if (!circle) {
        return false;
    }

    for (int cursorId : circle->getCursorIds()) {
        m_cursors.eraseById(cursorId);
    }

    return m_circles.eraseById(circleId);
}
