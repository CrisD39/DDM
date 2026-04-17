#include "repositories/deque/DequeTrackRepository.h"

#include <utility>

DequeTrackRepository::DequeTrackRepository(std::deque<Track>& tracks)
    : m_tracks(tracks) {}

Track& DequeTrackRepository::addFront(const Track& track) {
    m_tracks.push_front(track);
    return m_tracks.front();
}

Track& DequeTrackRepository::addFront(Track&& track) {
    m_tracks.push_front(std::move(track));
    return m_tracks.front();
}

Track* DequeTrackRepository::findById(int id) {
    for (Track& track : m_tracks) {
        if (track.getId() == id) {
            return &track;
        }
    }

    return nullptr;
}

const Track* DequeTrackRepository::findById(int id) const {
    for (const Track& track : m_tracks) {
        if (track.getId() == id) {
            return &track;
        }
    }

    return nullptr;
}

bool DequeTrackRepository::eraseById(int id) {
    for (auto it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        if (it->getId() == id) {
            m_tracks.erase(it);
            return true;
        }
    }

    return false;
}

Track* DequeTrackRepository::getNextById(int currentId) {
    if (m_tracks.empty()) {
        return nullptr;
    }

    std::size_t index = 0;
    for (; index < m_tracks.size(); ++index) {
        if (m_tracks[index].getId() == currentId) {
            break;
        }
    }

    if (index == m_tracks.size()) {
        return nullptr;
    }

    const std::size_t nextIndex = (index + 1) % m_tracks.size();
    return &m_tracks[nextIndex];
}

void DequeTrackRepository::updateAll(double deltaTime) {
    for (Track& track : m_tracks) {
        track.updatePosition(deltaTime);
    }
}
