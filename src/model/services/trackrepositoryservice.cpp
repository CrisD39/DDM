#include "services/trackrepositoryservice.h"

#include <utility>

TrackRepositoryService::TrackRepositoryService(ITrackRepository& tracks)
    : m_tracks(tracks) {}

Track& TrackRepositoryService::addFront(const Track& track) {
    return m_tracks.addFront(track);
}

Track& TrackRepositoryService::addFront(Track&& track) {
    return m_tracks.addFront(std::move(track));
}

Track* TrackRepositoryService::findById(int id) {
    return m_tracks.findById(id);
}

const Track* TrackRepositoryService::findById(int id) const {
    return m_tracks.findById(id);
}

bool TrackRepositoryService::eraseById(int id) {
    return m_tracks.eraseById(id);
}

Track* TrackRepositoryService::getNextById(int currentId) {
    return m_tracks.getNextById(currentId);
}

void TrackRepositoryService::updateTracks(double deltaTime) {
    m_tracks.updateAll(deltaTime);
}