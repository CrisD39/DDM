#ifndef FCDECODIFICATOR_H
#define FCDECODIFICATOR_H

#include <IDecodificator.h>
#include <QByteArray>
#include <QBitArray>
#include <QQueue>
#include <QPair>
#include <QJsonObject>
#include <QJsonArray>

class FCDecodificator : public IDecodificator
{
public:
    FCDecodificator();
    void decode(QByteArray message) override;

private:
    // --- helpers de conversión bit <-> byte ---
    QBitArray  byteArrayToBitArray(const QByteArray &bytes);
    QByteArray bitArrayToByteArray(const QBitArray &bits);

    // --- estado que usa el .cpp ---
    using RollingSteps = QPair<QByteArray, QByteArray>;
    QQueue<RollingSteps> rollingBallMaster;

    // buffer de bits de la palabra entrante
    QScopedPointer<QBitArray> inComingMessage { new QBitArray() };
    int currentBit = 0;

    // si usás JSON para decodificar etiquetas / display mode:
    QJsonObject displayJson;
    QJsonObject threatJson;
    QJsonObject overlayJson;
    QJsonObject displayModeJson;
    QJsonArray  jsonArray;   // si lo usás en configuration
};

#endif // FCDECODIFICATOR_H
