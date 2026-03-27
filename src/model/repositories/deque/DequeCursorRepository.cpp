#include "repositories/deque/DequeCursorRepository.h"

#include <utility>

DequeCursorRepository::DequeCursorRepository(std::deque<CursorEntity>& cursors)
    : m_cursors(cursors) {}

CursorEntity& DequeCursorRepository::addFront(const CursorEntity& cursor) {
    m_cursors.push_front(cursor);
    return m_cursors.front();
}

CursorEntity& DequeCursorRepository::addFront(CursorEntity&& cursor) {
    m_cursors.push_front(std::move(cursor));
    return m_cursors.front();
}

bool DequeCursorRepository::eraseById(int id) {
    for (auto it = m_cursors.begin(); it != m_cursors.end(); ++it) {
        if (it->getCursorId() == id) {
            m_cursors.erase(it);
            return true;
        }
    }

    return false;
}
