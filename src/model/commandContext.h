#pragma once
#include <QTextStream>
#include <QStringConverter>
#include <QString>
#include <QPointF>
#include <deque>        // usamos deque para push_front O(1)
#include <set>          // pool de IDs libres ordenado (menor primero)
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

    // Datos de tracks
    std::deque<Track> tracks;   // almacenamiento principal
    int               nextTrackId = 1;
    std::set<int>     freeIds;


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

    // Toma el menor ID libre si existe si no, usa nextTrackId y lo incrementa
    inline int acquireId() {
        if (!freeIds.empty()) {
            int id = *freeIds.begin();
            freeIds.erase(freeIds.begin());
            return id;
        }
        return nextTrackId++;
    }

    inline void releaseId(int id) {
        if (id == nextTrackId - 1) {            nextTrackId--;
            while (!freeIds.empty()) {
                auto it = freeIds.find(nextTrackId - 1);
                if (it == freeIds.end()) break;
                freeIds.erase(it);
                nextTrackId--;
            }
        } else {
            freeIds.insert(id);
        }
    }
};
