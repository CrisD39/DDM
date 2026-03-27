#pragma once

#include "entities/track.h"

class ITrackRepository {
public:
    virtual ~ITrackRepository() = default;
    virtual Track& addFront(const Track& track) = 0;
    virtual Track& addFront(Track&& track) = 0;
    virtual Track* findById(int id) = 0;
    virtual const Track* findById(int id) const = 0;
    virtual bool eraseById(int id) = 0;
    virtual Track* getNextById(int currentId) = 0;
    virtual void updateAll(double deltaTime) = 0;
};
