#pragma once
#include <QTextStream>
#include <QStringConverter>
#include <QString>
#include <QPointF>
#include <QPair>
#include <QtMath>
#include <cmath>
#include <deque>
#include <utility>
#include <unordered_map>

#include "entities/track.h"
#include "entities/cursorEntity.h"
#include "network/iTransport.h"
#include "entities/areaEntity.h"
#include "entities/circleEntity.h"
#include "entities/polygonoentity.h"

struct CommandContext {
    enum MotionMode {
        RELATIVE = 0,
        TRUE_MOTION = 1
    };

    CommandContext() : out(stdout), err(stderr) {
        out.setEncoding(QStringConverter::Utf8);
        err.setEncoding(QStringConverter::Utf8);
    }

    // --- SITREP extra ---
    struct SitrepExtra { QString info; };
    std::unordered_map<int, SitrepExtra> sitrepExtra;

    inline QString sitrepInfo(int trackId) const {
        auto it = sitrepExtra.find(trackId);
        return (it != sitrepExtra.end()) ? it->second.info : QString("-");
    }

    inline void setSitrepInfo(int trackId, const QString& text) {
        sitrepExtra[trackId].info = text;
    }

    QTextStream out;
    QTextStream err;
    QString     lastCommandLine;
    quint64     lastCommandHash = 0;
    int         commandCounter  = 1;

    std::deque<CursorEntity> cursors;
    std::deque<Track> tracks;

    struct OwnShipState {
        double xDm = 0.0;
        double yDm = 0.0;
        double latitudeDeg = 0.0;
        double longitudeDeg = 0.0;
        double speedKnots = 0.0;
        double courseDeg = 0.0;
        QString timeUtc = QStringLiteral("00:00:00");
        QString dateUtc = QStringLiteral("1970-01-01");
        QString source = QStringLiteral("unknown");
        bool valid = false;
    };

    OwnShipState ownShip;

    struct CpaMarkerState {
        QString sessionId;
        int trackAId = -1;
        int trackBId = -1;
        float xDm = 0.0f;
        float yDm = 0.0f;
        bool visible = true;
    };

    int               nextTrackId = 1;

    int               nextCursorId = 2;

    std::deque<AreaEntity> areas;
    std::deque<CircleEntity> circles;
    std::deque<PolygonoEntity> polygons;
    std::deque<CpaMarkerState> cpaMarkers;

    double centerX = 0.0;
    double centerY = 0.0;
    MotionMode motionMode = RELATIVE;

    // Transport opcional: si está seteado, los comandos CLI/backend pueden
    // notificar eventos JSON al frontend via transport->send()
    ITransport* transport = nullptr;

    inline std::deque<Track>& getTracks() { return tracks; }
    inline const std::deque<Track>& getTracks() const { return tracks; }

    inline std::deque<CursorEntity>& getCursors() { return cursors; }
    inline const std::deque<CursorEntity>& getCursors() const { return cursors; }

    inline std::deque<AreaEntity>& getAreas() { return areas; }
    inline const std::deque<AreaEntity>& getAreas() const { return areas; }

    inline std::deque<CircleEntity>& getCircles() { return circles; }
    inline const std::deque<CircleEntity>& getCircles() const { return circles; }

    inline std::deque<PolygonoEntity>& getPolygons() { return polygons; }
    inline const std::deque<PolygonoEntity>& getPolygons() const { return polygons; }

    inline std::deque<CpaMarkerState>& getCpaMarkers() { return cpaMarkers; }
    inline const std::deque<CpaMarkerState>& getCpaMarkers() const { return cpaMarkers; }

    inline void addArea(const AreaEntity& area) {
        areas.push_back(area);
    }

    inline void addCircle(const CircleEntity& circle) {
        circles.push_back(circle);
    }

    inline void addPolygon(const PolygonoEntity& polygon) {
        polygons.push_back(polygon);
    }

    inline CursorEntity& addCursorFront(const CursorEntity& c) {
        qDebug() << "agregando cursor ID:" << c.getCursorId()
                 << " Angle:" << c.getCursorAngle()
                 << " Length:" << c.getCursorLength();
        cursors.push_front(c);
        qDebug() << "termine de agregar";
        return cursors.front();
    }

    template <typename... Args>
    inline CursorEntity& emplaceCursorFront(Args&&... args) {
        qDebug() << "agregando cursor (emplace)";
        cursors.emplace_front(std::forward<Args>(args)...);
        qDebug() << "termine de agregar (emplace)";
        qDebug() << "cursors size =" << cursors.size();
        return cursors.front();
    }

    inline Track& addTrackFront(const Track& t) {
        tracks.push_front(t);
        return tracks.front();
    }

    template <typename... Args>
    inline Track& emplaceTrackFront(Args&&... args) {
        tracks.emplace_front(std::forward<Args>(args)...);
        return tracks.front();
    }

    inline QPointF center() const { return QPointF(centerX, centerY); }

    inline void setCenter(QPair<float,float> c){
        centerX = c.first;
        centerY = c.second;
    }

    inline void resetCenter(){ setCenter({0.0f, 0.0f}); }

    inline void setMotionMode(MotionMode mode) { motionMode = mode; }
    inline MotionMode getMotionMode() const { return motionMode; }

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

    inline bool eraseCursorById(int id) {
        for (auto it = cursors.begin(); it != cursors.end(); ++it) {
            if (it->getCursorId() == id) { cursors.erase(it); return true; }
        }
        return false;
    }

    inline void upsertCpaMarker(const CpaMarkerState& marker) {
        for (CpaMarkerState& existing : cpaMarkers) {
            if (existing.sessionId == marker.sessionId) {
                existing = marker;
                return;
            }
        }
        cpaMarkers.push_front(marker);
    }

    inline bool eraseCpaMarkerBySessionId(const QString& sessionId) {
        for (auto it = cpaMarkers.begin(); it != cpaMarkers.end(); ++it) {
            if (it->sessionId == sessionId) {
                cpaMarkers.erase(it);
                return true;
            }
        }
        return false;
    }

    inline int eraseCpaMarkersByTrackId(int trackId) {
        int removed = 0;
        for (auto it = cpaMarkers.begin(); it != cpaMarkers.end();) {
            if (it->trackAId == trackId || it->trackBId == trackId) {
                it = cpaMarkers.erase(it);
                ++removed;
            } else {
                ++it;
            }
        }
        return removed;
    }
    // transport is declared above; do not redeclare here.
    inline Track* getNextTrackById(int currentId) {
        if (tracks.empty()) return nullptr;
        std::size_t i = 0;
        for (; i < tracks.size(); ++i) {
            if (tracks[i].getId() == currentId) break;
        }
        if (i == tracks.size()) return nullptr;
        const std::size_t j = (i + 1) % tracks.size();
        return &tracks[j];
    }

    inline void updateTracks(double deltaTime){
        if (deltaTime <= 0.0) return;

        const double dtHours = deltaTime / 3600.0;
        if (dtHours <= 0.0) return;

        auto displacementDm = [&](const Track& t, double& dx, double& dy) {
            const double speedDmPerHour = t.getVelocidadDmPerHour();
            if (speedDmPerHour <= 0.0) {
                dx = 0.0;
                dy = 0.0;
                return;
            }

            const double distanceDm = speedDmPerHour * dtHours;
            const double courseRad = qDegreesToRadians(t.getCourseDeg());
            dx = distanceDm * std::sin(courseRad);
            dy = distanceDm * std::cos(courseRad);
        };

        if (motionMode == RELATIVE) {
            Track* ownShipTrack = findTrackById(0);
            double ownDx = 0.0;
            double ownDy = 0.0;
            if (ownShipTrack) {
                displacementDm(*ownShipTrack, ownDx, ownDy);
            }

            for (Track& track : tracks) {
                if (track.getId() == 0) {
                    // OwnShip stays anchored at center in relative-motion mode.
                    track.setX(0.0f);
                    track.setY(0.0f);
                    continue;
                }

                double trkDx = 0.0;
                double trkDy = 0.0;
                displacementDm(track, trkDx, trkDy);

                const float newX = static_cast<float>(track.getX() + (trkDx - ownDx));
                const float newY = static_cast<float>(track.getY() + (trkDy - ownDy));
                track.setX(newX);
                track.setY(newY);
            }
            return;
        }

        for (Track& track : tracks) {
            double trkDx = 0.0;
            double trkDy = 0.0;
            displacementDm(track, trkDx, trkDy);

            const float newX = static_cast<float>(track.getX() + trkDx);
            const float newY = static_cast<float>(track.getY() + trkDy);
            track.setX(newX);
            track.setY(newY);
        }
    }

    inline bool deleteArea(int areaId) {
        for (auto it = areas.begin(); it != areas.end(); ++it) {
            if (it->getId() == areaId) {
                // Eliminar cursores asociados
                eraseCursorById(it->getCursorIdAB());
                eraseCursorById(it->getCursorIdBC());
                eraseCursorById(it->getCursorIdCD());
                eraseCursorById(it->getCursorIdDA());
                // Eliminar area de la lista
                areas.erase(it);
                return true;
            }
        }
        return false;
    }

    inline bool deleteCircle(int circleId) {
        for (auto it = circles.begin(); it != circles.end(); ++it) {
            if (it->getId() == circleId) {
                // Eliminar cursores asociados
                for(int cid : it->getCursorIds()) {
                    eraseCursorById(cid);
                }
                circles.erase(it);
                return true;
            }
        }
        return false;
    }

    inline bool deletePolygon(int polygonId) {
        for (auto it = polygons.begin(); it != polygons.end(); ++it) {
            if (it->getId() == polygonId) {
                for (int cid : it->getCursorIds()) {
                    eraseCursorById(cid);
                }
                polygons.erase(it);
                return true;
            }
        }
        return false;
    }
};


