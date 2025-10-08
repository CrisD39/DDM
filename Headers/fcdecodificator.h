#ifndef FCDECODIFICATOR_H
#define FCDECODIFICATOR_H

#include "IDecodificator.h"
#include <QByteArray>
#include <QBitArray>
#include <QJsonObject>
#include <QVector>
#include <QQueue>
#include <QString>

struct RollingSteps {
    QByteArray first;
    QByteArray second;
};



class FCDecodificator : public IDecodificator
{
public:
    FCDecodificator();
    void decode(const QByteArray &message) override;

private:
    // Métodos de decodificación por palabra
    void decomsg1();
    void decomsg2();
    void decomsg3();
    void decomsg4();
    void decomsg5();
    void decomsg6();
    void decomsg7();
    void decomsg8();

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
    QString overlayMaster;
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
