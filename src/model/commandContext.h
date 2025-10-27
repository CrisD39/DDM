#pragma once
#include <QTextStream>
#include <QStringConverter>
#include <QString>
#include <QPointF>
#include <deque>        // ← usamos deque para push_front O(1)
#include "entities/track.h"

struct CommandContext {
    CommandContext() : out(stdout), err(stderr) {
        out.setEncoding(QStringConverter::Utf8);
        err.setEncoding(QStringConverter::Utf8);
    }

    QTextStream out;
    QTextStream err;
    QString     lastCommandLine;
    quint64     lastCommandHash = 0;
    int         commandCounter  = 1;

    std::deque<Track> tracks;
    int               nextTrackId = 1;

    double centerX = 0.0;
    double centerY = 0.0;

    inline std::deque<Track>& getTracks() { return tracks; }
    inline const std::deque<Track>& getTracks() const { return tracks; }

    inline Track& addTrackFront(const Track& t) {
        tracks.push_front(t);              // O(1)
        return tracks.front();
    }

    template<typename... Args>
    inline Track& emplaceTrackFront(Args&&... args) {
        tracks.emplace_front(std::forward<Args>(args)...); // O(1)
        return tracks.front();
    }

    inline QPointF center() const { return QPointF(centerX, centerY); }

    inline void setCenter(QPair<float,float> c){
        centerX = c.first;
        centerY = c.second;
    }

    inline void resetCenter(){ setCenter({0.0,0.0});}

    inline Track* findTrackById(int id) {
        for (Track& t : tracks) if (t.getId() == id) return &t;
        return nullptr;
    }
    inline const Track* findTrackById(int id) const {
        for (const Track& t : tracks) if (t.getId() == id) return &t;
        return nullptr;
    }

    inline bool eraseTrackById(int id) {
        for (auto it = tracks.begin(); it != tracks.end(); ++it) {
            if (it->getId() == id) { tracks.erase(it); return true; }
        }
        return false;
    }

    inline Track* getNextTrackById(int currentId) {
        if (tracks.empty()) return nullptr;

        // buscar índice del id
        std::size_t i = 0;
        for (; i < tracks.size(); ++i) {
            if (tracks[i].getId() == currentId) break;
        }
        if (i == tracks.size()) return nullptr;  // id no encontrado

        const std::size_t j = (i + 1) % tracks.size();  // circular
        return &tracks[j];
    }


};
