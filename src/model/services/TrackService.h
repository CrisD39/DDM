#pragma once

#include "repositories/interfaces/ITrackRepository.h"

class TrackService {
public:
    explicit TrackService(ITrackRepository& tracks);

    Track& addFront(const Track& track);
    Track& addFront(Track&& track);
    Track* findById(int id);
    const Track* findById(int id) const;
    bool eraseById(int id);
    Track* getNextById(int currentId);
    void updateTracks(double deltaTime);

private:
    ITrackRepository& m_tracks;
};
