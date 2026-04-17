#pragma once

#include <deque>

#include "entities/areaEntity.h"
#include "repositories/interfaces/IAreaRepository.h"

class DequeAreaRepository final : public IAreaRepository {
public:
    explicit DequeAreaRepository(std::deque<AreaEntity>& areas);
    AreaEntity* findById(int id) override;
    bool eraseById(int id) override;

private:
    std::deque<AreaEntity>& m_areas;
};
