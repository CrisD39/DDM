#include "services/TrackService.h"

#include <utility>

TrackService::TrackService(ITrackRepository& tracks)
    : m_tracks(tracks) {}

Track& TrackService::addFront(const Track& track) {
    return m_tracks.addFront(track);
}

Track& TrackService::addFront(Track&& track) {
    return m_tracks.addFront(std::move(track));
}

Track* TrackService::findById(int id) {
    return m_tracks.findById(id);
}

const Track* TrackService::findById(int id) const {
    return m_tracks.findById(id);
}

bool TrackService::eraseById(int id) {
    return m_tracks.eraseById(id);
}

Track* TrackService::getNextById(int currentId) {
    return m_tracks.getNextById(currentId);
}

void TrackService::updateTracks(double deltaTime) {
    m_tracks.updateAll(deltaTime);
}
