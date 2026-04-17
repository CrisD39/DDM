#include "services/cursorrepositoryservice.h"

#include <utility>

CursorRepositoryService::CursorRepositoryService(ICursorRepository& cursors)
    : m_cursors(cursors) {}

CursorEntity& CursorRepositoryService::addFront(const CursorEntity& cursor) {
    return m_cursors.addFront(cursor);
}

CursorEntity& CursorRepositoryService::addFront(CursorEntity&& cursor) {
    return m_cursors.addFront(std::move(cursor));
}

bool CursorRepositoryService::eraseById(int id) {
    return m_cursors.eraseById(id);
}