#pragma once

#include <deque>
#include <QString>

class CommandContext;
class Track;

class SitrepService {
public:
    explicit SitrepService(CommandContext* context);

    std::deque<Track> snapshot() const;
    bool deleteTrackById(int id);
    Track* findTrackById(int id) const;
    void setSitrepInfo(int id, const QString& text);

private:
    CommandContext* m_context;
};
