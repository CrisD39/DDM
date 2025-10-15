#ifndef FCDECODIFICATOR_H
#define FCDECODIFICATOR_H

#include "IDecodificator.h"
#include <QByteArray>
#include <QBitArray>
#include <QJsonObject>
#include <QVector>
#include <QQueue>
#include <QString>
#include <QPair>

struct RollingSteps {
    QByteArray first;
    QByteArray second;
};

class FCDecodificator : public IDecodificator {
Q_OBJECT

public:
    explicit FCDecodificator();
    void decode(const QByteArray &message) override;

signals:
    void signalOBM(QPair<float,float> delta);
    void newRollingBall(QPair<float,float>);
    void newRange(int);
    void newQEK(QString);
    void newOverlay(QString);

private:
    // Métodos de decodificación por palabra
    void decodeWord1();
    void decodeWord2();
    void decodeWord3();
    void decodeWord4();
    void decodeWord5();
    void decodeWord6();
    void decodeWord7();
    void decodeWord8();

    // Utilidades internas
    void readJson();
    QBitArray byteArrayToBitArray(const QByteArray &byteArray);
    QByteArray bitArrayToByteArray(const QBitArray &bitArray);

    // Variables internas de trabajo
    QBitArray *inComingMessage;
    QBitArray *outComingMessage;
    int currentBit = 0;

    // Estados compartidos
    int range = 0;
    int handWheelPhiQueue = 0;
    int handWheelRhoQueue = 0;

    QVector<bool> threatAssessment;
    QVector<bool> displayMode;
    QVector<bool> displaySelection;

    // Estados master
    QVector<bool> labelSelectionMaster;
    QVector<bool> quickEntryKeyboardMaster;
    QVector<bool> centerMaster;
    QVector<bool> icmMaster;
    QString overlayMaster = "0";
    QString mikMaster;


    // Estados slave
    QString labelSelectionSlave;
    QString quickEntryKeyboardSlave;
    QString centerSlave;
    QString icmSlave;
    QString overlaySlave;
    QString mikSlave;

    QJsonObject jsonFile;
};



#endif // FCDECODIFICATOR_H
