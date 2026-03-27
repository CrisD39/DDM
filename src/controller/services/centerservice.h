#pragma once

#include <QPair>

class CommandContext;

class CenterService {
public:
    explicit CenterService(CommandContext* context);

    void setCenter(double x, double y);
    QPair<float,float> getCenter() const;

private:
    CommandContext* m_context;
};
