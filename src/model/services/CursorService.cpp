#include "services/CursorService.h"

#include <utility>

CursorService::CursorService(ICursorRepository& cursors)
    : m_cursors(cursors) {}

CursorEntity& CursorService::addFront(const CursorEntity& cursor) {
    return m_cursors.addFront(cursor);
}

CursorEntity& CursorService::addFront(CursorEntity&& cursor) {
    return m_cursors.addFront(std::move(cursor));
}

bool CursorService::eraseById(int id) {
    return m_cursors.eraseById(id);
}
