#pragma once

#include "repositories/interfaces/ICursorRepository.h"

class CursorRepositoryService {
public:
    explicit CursorRepositoryService(ICursorRepository& cursors);

    CursorEntity& addFront(const CursorEntity& cursor);
    CursorEntity& addFront(CursorEntity&& cursor);
    bool eraseById(int id);

private:
    ICursorRepository& m_cursors;
};