#pragma once

#include <deque>

#include "entities/cursorEntity.h"
#include "repositories/interfaces/ICursorRepository.h"

class DequeCursorRepository final : public ICursorRepository {
public:
    explicit DequeCursorRepository(std::deque<CursorEntity>& cursors);
    CursorEntity& addFront(const CursorEntity& cursor) override;
    CursorEntity& addFront(CursorEntity&& cursor) override;
    bool eraseById(int id) override;

private:
    std::deque<CursorEntity>& m_cursors;
};
