#pragma once
#include <QTextStream>
#include <QStringConverter>
#include <QString>
#include <QPointF>
#include <QPair>
#include <deque>
#include <utility>
#include <unordered_map>

#include "entities/track.h"
#include "entities/cursorEntity.h"
#include "network/iTransport.h"
#include "entities/areaEntity.h"
#include "entities/circleEntity.h"
#include "entities/polygonoentity.h"
#include "repositories/deque/DequeAreaRepository.h"
#include "repositories/deque/DequeCircleRepository.h"
#include "repositories/deque/DequeCursorRepository.h"
#include "repositories/deque/DequeTrackRepository.h"
#include "services/cursorrepositoryservice.h"
#include "services/DomainCleanupService.h"
#include "services/trackrepositoryservice.h"

struct CommandContext {
private:
    struct Dependencies {
        Dependencies(std::deque<CursorEntity>& cursorsRef,
                     std::deque<Track>& tracksRef,
                     std::deque<AreaEntity>& areasRef,
                     std::deque<CircleEntity>& circlesRef)
            : cursorRepository(cursorsRef)
            , trackRepository(tracksRef)
            , areaRepository(areasRef)
            , circleRepository(circlesRef)
            , cursorService(cursorRepository)
            , trackService(trackRepository)
            , cleanupService(areaRepository, circleRepository, cursorRepository) {}

        DequeCursorRepository cursorRepository;
        DequeTrackRepository trackRepository;
        DequeAreaRepository areaRepository;
        DequeCircleRepository circleRepository;

        CursorRepositoryService cursorService;
        TrackRepositoryService trackService;
        DomainCleanupService cleanupService;
    };

public:

    CommandContext()
        : out(stdout)
        , err(stderr)
        , dependencies(cursors, tracks, areas, circles) {
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

private:
    std::deque<CursorEntity> cursors;
    std::deque<Track> tracks;
    std::deque<AreaEntity> areas;
    std::deque<CircleEntity> circles;
    std::deque<PolygonoEntity> polygons;
    std::deque<CpaMarkerState> cpaMarkers;

    Dependencies dependencies;

public:

    double centerX = 0.0;
    double centerY = 0.0;

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
        CursorEntity& created = dependencies.cursorService.addFront(c);
        qDebug() << "termine de agregar";
        return created;
    }

    template <typename... Args>
    inline CursorEntity& emplaceCursorFront(Args&&... args) {
        qDebug() << "agregando cursor (emplace)";
        CursorEntity cursor(std::forward<Args>(args)...);
        CursorEntity& created = dependencies.cursorService.addFront(std::move(cursor));
        qDebug() << "termine de agregar (emplace)";
        qDebug() << "cursors size =" << cursors.size();
        return created;
    }

    inline Track& addTrackFront(const Track& t) {
        return dependencies.trackService.addFront(t);
    }

    template <typename... Args>
    inline Track& emplaceTrackFront(Args&&... args) {
        Track track(std::forward<Args>(args)...);
        return dependencies.trackService.addFront(std::move(track));
    }

    inline QPointF center() const { return QPointF(centerX, centerY); }

    inline void setCenter(QPair<float,float> c){
        centerX = c.first;
        centerY = c.second;
    }

    inline void resetCenter(){ setCenter({0.0f, 0.0f}); }

    inline Track* findTrackById(int id) {
        return dependencies.trackService.findById(id);
    }
    inline const Track* findTrackById(int id) const {
        return dependencies.trackService.findById(id);
    }

    inline bool eraseTrackById(int id) {
        return dependencies.trackService.eraseById(id);
    }

    inline bool eraseCursorById(int id) {
        return dependencies.cursorService.eraseById(id);
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
        return dependencies.trackService.getNextById(currentId);
    }

    inline void updateTracks(double deltaTime){
        dependencies.trackService.updateTracks(deltaTime);
    }

    inline bool deleteArea(int areaId) {
        return dependencies.cleanupService.deleteAreaWithCursors(areaId);
    }

    inline bool deleteCircle(int circleId) {
        return dependencies.cleanupService.deleteCircleWithCursors(circleId);
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


