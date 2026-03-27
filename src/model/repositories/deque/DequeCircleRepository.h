#pragma once

#include <deque>

#include "entities/circleEntity.h"
#include "repositories/interfaces/ICircleRepository.h"

class DequeCircleRepository final : public ICircleRepository {
public:
    explicit DequeCircleRepository(std::deque<CircleEntity>& circles);
    CircleEntity* findById(int id) override;
    bool eraseById(int id) override;

private:
    std::deque<CircleEntity>& m_circles;
};
