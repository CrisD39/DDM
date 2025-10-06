#ifndef FCDECODIFICATOR_H
#define FCDECODIFICATOR_H

#include <IDecodificator.h>
#include <QByteArray>

class FCDecodificator: public IDecodificator
{
public:
    FCDecodificator();
    void decode(QByteArray message) override;
    //bitArrayToByteArray(const QBitArray bitArray);

private:
    void decomsg1();
    void decomsg2();
    void decomsg3();
    void decomsg4();
    void decomsg5();
    void decomsg6();
    void decomsg7();
    void decomsg8();

    void readJson();

    QBitArray* inComingMessage;
    QBitArray* outComingMessage;

    QBitArray byteArrayToBitArray(const QByteArray bitArray);
    QByteArray bitArrayToByteArray(const QBitArray bitArray);

    // Estados compartidos
    int range;
    int handWheelPhiQueue;
    int handWheelRhoQueue;

    QVector<bool> threatAssessment;
    QVector<bool> displayMode;
    QVector<bool> displaySelection;

    // Estados master
    QVector<bool> labelSelectionMaster;
    QVector<bool> quickEntryKeyboardMaster;
    QVector<bool> centerMaster;
    QVector<bool> icmMaster;
    // QQueue<RollingSteps> rollingBallMaster;
    // QQueue<RollingSteps> rollingBallSlave;
    QString overlayMaster;
    QString mikMaster;

    // Estados slave
    QString labelSelectionSlave;
    QString quickEntryKeyboardSlave;
    QString centerSlave;
    QString icmSlave;

    QString overlaySlave;
    QString mikSlave;

    // QJsonObject jsonFile;

    int currentBit;
};

#endif // FCDECODIFICATOR_H
