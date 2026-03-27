#pragma once

#include <deque>

#include "entities/track.h"
#include "repositories/interfaces/ITrackRepository.h"

class DequeTrackRepository final : public ITrackRepository {
public:
    explicit DequeTrackRepository(std::deque<Track>& tracks);

    Track& addFront(const Track& track) override;
    Track& addFront(Track&& track) override;
    Track* findById(int id) override;
    const Track* findById(int id) const override;
    bool eraseById(int id) override;
    Track* getNextById(int currentId) override;
    void updateAll(double deltaTime) override;

private:
    std::deque<Track>& m_tracks;
};
