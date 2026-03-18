#include "cpaservice.h"
#include "trackservice.h"
#include "commandContext.h"
#include "entities/track.h"
#include "model/cpa.h"

#include <cmath>
#include <QDebug>

CPAService::CPAService(CommandContext* context)
    : m_context(context)
{
    Q_ASSERT(m_context);
}

CPAResult CPAService::computeCPA(int trackId1, int trackId2) const
{
    TrackService trackService(m_context);
    
    Track* track1 = trackService.findTrackById(trackId1);
    Track* track2 = trackService.findTrackById(trackId2);

    if (!track1 || !track2) {
        return {0.0, 0.0, false};
    }

    CPA cpa;
    return cpa.computeCPA(*track1, *track2);
}
