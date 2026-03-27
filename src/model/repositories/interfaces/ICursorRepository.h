#pragma once

#include "entities/cursorEntity.h"

class ICursorRepository {
public:
    virtual ~ICursorRepository() = default;
    virtual CursorEntity& addFront(const CursorEntity& cursor) = 0;
    virtual CursorEntity& addFront(CursorEntity&& cursor) = 0;
    virtual bool eraseById(int id) = 0;
};
