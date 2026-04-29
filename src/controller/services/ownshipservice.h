#pragma once

#include <QJsonObject>
#include <QString>

class CommandContext;

struct OwnShipOperationResult {
    bool success = false;
    QString errorCode;
    QString message;
};

class OwnShipService
{
public:
    explicit OwnShipService(CommandContext* context);

    OwnShipOperationResult updateFromJson(const QJsonObject& args);
    OwnShipOperationResult setFromCli(double courseDeg,
                                      double speedKnots,
                                      const QString& source = QStringLiteral("CLI"));

    QJsonObject serializeOwnShip() const;
    QString formatOwnShip() const;

private:
    static double normalize360(double deg);
    static void splitCoordinate(double value,
                                int& degrees,
                                int& minutes,
                                int& seconds,
                                QChar& hemisphere,
                                QChar positiveHemisphere,
                                QChar negativeHemisphere);
    void refreshDerivedFields();
    void syncOwnShipVirtualTrack();

    CommandContext* m_context;
};
