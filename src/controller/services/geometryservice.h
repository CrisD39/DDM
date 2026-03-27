#pragma once

#include <QPointF>
#include <QString>
#include <QJsonObject>
#include <vector>

class CommandContext;

struct GeometryResult {
    bool success = false;
    QString errorCode;
    QString message;
    int id = -1;
};

class GeometryService {
public:
    explicit GeometryService(CommandContext* context);

    GeometryResult createArea(const std::vector<QPointF>& points, int areaType, const QString& areaColor);
    GeometryResult deleteArea(int areaId);

    GeometryResult createCircle(const QPointF& center, double radius, int type, const QString& color);
    GeometryResult deleteCircle(int circleId);

    GeometryResult createPolygon(const std::vector<QPointF>& points, int polyType, const QString& polyColor);
    GeometryResult deletePolygon(int polygonId);

    QJsonObject listShapes() const;

private:
    CommandContext* m_context;
};
