#include "repositories/deque/DequeCircleRepository.h"

DequeCircleRepository::DequeCircleRepository(std::deque<CircleEntity>& circles)
    : m_circles(circles) {}

CircleEntity* DequeCircleRepository::findById(int id) {
    for (auto& circle : m_circles) {
        if (circle.getId() == id) {
            return &circle;
        }
    }

    return nullptr;
}

bool DequeCircleRepository::eraseById(int id) {
    for (auto it = m_circles.begin(); it != m_circles.end(); ++it) {
        if (it->getId() == id) {
            m_circles.erase(it);
            return true;
        }
    }

    return false;
}
