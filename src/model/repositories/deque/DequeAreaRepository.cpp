#include "repositories/deque/DequeAreaRepository.h"

DequeAreaRepository::DequeAreaRepository(std::deque<AreaEntity>& areas)
    : m_areas(areas) {}

AreaEntity* DequeAreaRepository::findById(int id) {
    for (auto& area : m_areas) {
        if (area.getId() == id) {
            return &area;
        }
    }

    return nullptr;
}

bool DequeAreaRepository::eraseById(int id) {
    for (auto it = m_areas.begin(); it != m_areas.end(); ++it) {
        if (it->getId() == id) {
            m_areas.erase(it);
            return true;
        }
    }

    return false;
}
