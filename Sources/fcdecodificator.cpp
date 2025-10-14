#include "fcdecodificator.h"
#include <QDebug>
#include <QBitArray>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#define WORD_SIZE 24
#define WORD_COUNT 9

FCDecodificator::FCDecodificator() {}

void FCDecodificator::decode(const QByteArray &message)
{
    currentBit = 0;
    readJson();
    inComingMessage = new QBitArray();
    *inComingMessage = byteArrayToBitArray(message);

    decomsg1();
    decomsg2();
    decomsg4();
    decomsg5();
}


// ====================== MENSAJE 1 ======================
void FCDecodificator::decomsg1()
{
    //qDebug()<<"decomsg1";
    // --- RANGE SCALE (bits 0-2) ---
    int word1 = WORD_SIZE * 0;
    currentBit = word1;

    QString rangeBits;
    for (int i = 0; i <= 2; ++i) {
        rangeBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    QJsonObject rangeJson = jsonFile["RANGE_SCALE_DECODE"].toObject();
    if (rangeJson.contains(rangeBits)) {
        QString decodedRange = rangeJson[rangeBits].toString();
        this->range = decodedRange.toInt();
        //qDebug() << "[Decodificación] RANGE:" << rangeBits << ":" << decodedRange;
    }

    // --- DISPLAY SELECTION (bits 3-11) ---
    QVector<bool> decodedDisplaySelection;
    QJsonObject displayJson = jsonFile["DISPLAY_SELECTION_DECODE"].toObject();
    for (int i = 0; i < 8; i++) {
        if (inComingMessage->testBit(currentBit)) {
            QString key = QString::number(currentBit);
            if (displayJson.contains(key)) {
                QString decoded = displayJson[key].toString();
                //qDebug() << "[Decodificación] DISPLAY SELECTION bit" << currentBit << ":" << decoded;
            }
            decodedDisplaySelection.append(true); //aca tengo que ver que pongo
        } else {
            decodedDisplaySelection.append(false);
        }
        currentBit++;
    }

    this->displaySelection = decodedDisplaySelection;

    // --- THREAT ASSESSMENT (bits 12-16) ---
    QVector<bool> decodedThreatAssessment;
    QJsonObject threatJson = jsonFile["THREAT_ASSESMENT_DECODE"].toObject();
    for (int i = 0; i < 5; ++i) {
        if (inComingMessage->testBit(currentBit)) {
            QString key = QString::number(currentBit);
            if (threatJson.contains(key)) {
                QString decoded = threatJson[key].toString();
                //qDebug() << "[Decodificación] THREAT ASSESSMENT bit" << currentBit << ":" << decoded;
            }
            decodedThreatAssessment.append(true);
        } else {
            decodedThreatAssessment.append(false);
        }
        currentBit++;
    }

    this->threatAssessment = decodedThreatAssessment;
}


// ====================== MENSAJE 2 ======================
void FCDecodificator::decomsg2()
{
    //qDebug() << "DECOMSG 2 << ";
    int word2 = WORD_SIZE * 1;
    currentBit = word2;

    QVector<bool> decodedDisplayMode(16, false);
    QJsonObject displayModeJson = jsonFile["DISPLAY_MODE_MASTER_DECODE"].toObject();

    // Display Mode Izquierda
    for (int i = 0; i <= 7; i++) {
        // qDebug()<<"estoy en el bit %i de la palabra 2\n"<<currentBit;
        bool bitVal = inComingMessage->testBit(currentBit);
        decodedDisplayMode[i] = bitVal;
        if (bitVal) {
            // qDebug()<< "bit valido";
            QString key = QString::number(currentBit);
            if (displayModeJson.contains(key)) {
                QString decoded = displayModeJson[key].toString();
                //qDebug() << "[Decodificación] DISPLAY MODE IZQ bit" << i << "→" << decoded;
            }
        }
        currentBit++;
    }

    // Display Mode Derecha
    for (int i = 0; i <= 7; i++) {
        bool bitVal = inComingMessage->testBit(currentBit);
        decodedDisplayMode[8 + i] = bitVal;
        if (bitVal) {
            QString key = QString::number(currentBit);
            if (displayModeJson.contains(key)) {
                QString decoded = displayModeJson[key].toString();
                //qDebug() << "[Decodificación] DISPLAY MODE DER bit" << i << "→" << decoded;
            }
        }
        currentBit++;
    }

    this->displayMode = decodedDisplayMode;
}

// ====================== MENSAJE 4 ======================
void FCDecodificator::decomsg4()
{
    int word4 = WORD_SIZE * 3;
    currentBit = word4;

    // QEK Master
    QString qekMasterBits;
    for (int i = 0; i < 8; i++) {
        qekMasterBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    QJsonObject qekJson = jsonFile["QEK_DECODE"].toObject();
    if (qekJson.contains(qekMasterBits)) {
        QString decodedQEK = qekJson[qekMasterBits].toString();
        quickEntryKeyboardMaster.clear();
        for (QChar bit : qekMasterBits) {
            quickEntryKeyboardMaster.append(bit == '1');
        }
        //qDebug() << "[Decodificación] QEK Master:" << qekMasterBits << "→" << decodedQEK;
    } else {
        //qDebug() << "[Decodificación] QEK Master desconocido:" << qekMasterBits;
    }

    qDebug()<< "Ultimo bit de master: "<< currentBit;
    // QEK Slave
    QString qekSlaveBits;
    for (int i = 0; i < 8; i++) {
        //qDebug()<<"El bit actual es: "<< currentBit;
        //qDebug()<< "Tiene valor: "<< inComingMessage->testBit(currentBit);
        qekSlaveBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    qDebug()<< "qekSlavebits es" <<qekSlaveBits;
    if (qekJson.contains(qekSlaveBits)) {
        QString decodedQEK = qekJson[qekSlaveBits].toString();
        quickEntryKeyboardSlave.clear();
        //qDebug() << "[Decodificación] QEK Slave:" << qekSlaveBits << "→" << decodedQEK;
    } else {
        //qDebug() << "[Decodificación] QEK Slave desconocido:" << qekSlaveBits;
    }
}


void FCDecodificator::decomsg5()
{
    int word5 = WORD_SIZE * 4;
    currentBit = word5;

    // --- ICM Izquierdo (bits 0–2 de la palabra 5) ---
    QString icmMasterBits;
    for (int i = 0; i <= 2; i++) {
        icmMasterBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    QJsonObject icmJson = jsonFile["ICM_DECODE"].toObject();

    if (icmJson.contains(icmMasterBits)) {
        QString decodedICM = icmJson[icmMasterBits].toString();
        this->icmMaster.clear();
        for (QChar bit : icmMasterBits) {
            this->icmMaster.append(bit == '1');
        }
        //qDebug() << "[Decodificación] ICM Izquierdo:" << icmMasterBits << "→" << decodedICM;
    } else {
        qWarning() << "[Decodificación] ICM Izquierdo desconocido:" << icmMasterBits;
    }

    currentBit++; // hay un 0 en el bit 3

    // --- Overlay Izquierdo (bits 4–7 palabra 5) ---
    QString overlayMasterBits;
    for (int i = 0; i <= 3; i++) {
        overlayMasterBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    QJsonObject overlayJson = jsonFile["OVERLAY_DECODE"].toObject();
    if (overlayJson.contains(overlayMasterBits)) {
        QString decodedOverlay = overlayJson[overlayMasterBits].toString();
        this->overlayMaster = decodedOverlay;
        //qDebug() << "[Decodificación] Overlay Izquierdo:" << overlayMasterBits << "→" << decodedOverlay;
    } else {
        qWarning() << "[Decodificación] Overlay Izquierdo desconocido:" << overlayMasterBits;
    }

    // --- ICM Derecho (bits 8–10 de la palabra 5) ---
    QString icmSlaveBits;
    for (int i = 0; i <= 2; i++) {
        icmSlaveBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    if (icmJson.contains(icmSlaveBits)) {
        QString decodedICM = icmJson[icmSlaveBits].toString();
        this->icmSlave = decodedICM;
        //qDebug() << "[Decodificación] ICM Derecho:" << icmSlaveBits << "→" << decodedICM;
    } else {
        qWarning() << "[Decodificación] ICM Derecho desconocido:" << icmSlaveBits;
    }

    currentBit++; // salto bit 11

    // --- Overlay Derecho (bits 12–15 palabra 5) ---
    QString overlaySlaveBits;
    for (int i = 0; i <= 3; i++) {
        overlaySlaveBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
        currentBit++;
    }

    if (overlayJson.contains(overlaySlaveBits)) {
        QString decodedOverlay = overlayJson[overlaySlaveBits].toString();
        this->overlaySlave = decodedOverlay;
        //qDebug() << "[Decodificación] Overlay Derecho:" << overlaySlaveBits << "→" << decodedOverlay;
    } else {
        qWarning() << "[Decodificación] Overlay Derecho desconocido:" << overlaySlaveBits;
    }
}

void FCDecodificator::decomsg6()
{
    // La palabra 6 empieza en el bit 24 * 5 = 120
    int word6 = WORD_SIZE * 5;  // Si las palabras van de 0–8, mantener 5; si van de 1–9, cambiar a 6
    currentBit = word6;

    // --- Handwheel ΔΦ (bits 0–7 palabra 6) ---
    float phiValue = 0.0f;
    for (int i = 0; i < 8; ++i) {
        if (currentBit >= inComingMessage->size()) {
            qWarning() << "[Decodificación] Error: acceso fuera de rango en ΔΦ (bit" << currentBit << ")";
            break;
        }
        bool bit = inComingMessage->testBit(currentBit);
        phiValue = (phiValue < 1) | (bit ? 1 : 0);
        currentBit++;
    }

    // Conversión de complemento a dos (signed 8-bit)
    if (phiValue && 0x80) {
        phiValue -= 256;
    }

    // --- Handwheel Δρ (bits 8–15 palabra 6) ---
    int rhoValue = 0.0f;
    for (int i = 0; i < 8; ++i) {
        if (currentBit >= inComingMessage->size()) {
            qWarning() << "[Decodificación] Error: acceso fuera de rango en Δρ (bit" << currentBit << ")";
            break;
        }
        bool bit = inComingMessage->testBit(currentBit);
        rhoValue = (rhoValue << 1) | (bit ? 1 : 0);
        currentBit++;
    }

    if (rhoValue & 0x80) {
        rhoValue -= 256;
    }

    // Guardamos en los estados internos
    this->handWheelPhiQueue = phiValue;
    this->handWheelRhoQueue = rhoValue;

    QPair<float,float> deltasHandwhell;
    deltasHandwhell.first = phiValue;
    deltasHandwhell.second = rhoValue;

    //qDebug() << "[Decodificación] Handwheel ΔΦ:" << phiValue
    //         << " Δρ:" << rhoValue;

    emit signalOBM(deltasHandwhell);
}


void FCDecodificator::decomsg7()
{
    // Palabra 7 → desplazamiento base 24 * 6 = 144
    int word7 = WORD_SIZE * 6;
    currentBit = word7;

    // --- Rolling Ball ΔX (bits 0–7 palabra 7) ---
    int deltaX = 0;
    for (int i = 0; i < 8; ++i) {
        if (currentBit >= inComingMessage->size()) {
            qWarning() << "[Decodificación] Error: acceso fuera de rango en ΔX (bit" << currentBit << ")";
            break;
        }
        bool bit = inComingMessage->testBit(currentBit);
        deltaX = (deltaX << 1) | (bit ? 1 : 0);
        currentBit++;
    }

    // --- Rolling Ball ΔY (bits 8–15 palabra 7) ---
    int deltaY = 0;
    for (int i = 0; i < 8; ++i) {
        if (currentBit >= inComingMessage->size()) {
            qWarning() << "[Decodificación] Error: acceso fuera de rango en ΔY (bit" << currentBit << ")";
            break;
        }
        bool bit = inComingMessage->testBit(currentBit);
        deltaY = (deltaY << 1) | (bit ? 1 : 0);
        currentBit++;
    }

    // Guardamos los desplazamientos en la cola RollingBall
    RollingSteps step;
    step.first = QByteArray(1, static_cast<char>(deltaX));
    step.second = QByteArray(1, static_cast<char>(deltaY));
    // this->rollingBallMaster.enqueue(step);

    //qDebug() << "[Decodificación] Rolling Ball Master ΔX:" << deltaX
    //         << " ΔY:" << deltaY;
}

void FCDecodificator::decomsg8()
{
    // Palabra 8 → desplazamiento base 24 * 7 = 168
    int word8 = WORD_SIZE * 7;
    currentBit = word8;

    // --- Rolling Ball ΔX (bits 0–7 palabra 8) ---
    int deltaX = 0;
    for (int i = 0; i < 8; ++i) {
        if (currentBit >= inComingMessage->size()) {
            qWarning() << "[Decodificación] Error: acceso fuera de rango en ΔX (bit" << currentBit << ")";
            break;
        }
        bool bit = inComingMessage->testBit(currentBit);
        deltaX = (deltaX << 1) | (bit ? 1 : 0);
        currentBit++;
    }

    // --- Rolling Ball ΔY (bits 8–15 palabra 8) ---
    int deltaY = 0;
    for (int i = 0; i < 8; ++i) {
        if (currentBit >= inComingMessage->size()) {
            qWarning() << "[Decodificación] Error: acceso fuera de rango en ΔY (bit" << currentBit << ")";
            break;
        }
        bool bit = inComingMessage->testBit(currentBit);
        deltaY = (deltaY << 1) | (bit ? 1 : 0);
        currentBit++;
    }

    // Guardamos en la cola de Rolling Ball Slave
    RollingSteps step;
    step.first = QByteArray(1, static_cast<char>(deltaX));
    step.second = QByteArray(1, static_cast<char>(deltaY));
    // this->rollingBallSlave.enqueue(step);

    //qDebug() << "[Decodificación] Rolling Ball Slave ΔX:" << deltaX
    //         << " ΔY:" << deltaY;
}



 // ====================== UTILIDADES ======================
 QBitArray FCDecodificator::byteArrayToBitArray(const QByteArray &byteArray)
 {
     QBitArray bits(byteArray.size() * 8);
     for (int i = 0; i < byteArray.size(); ++i) {
         for (int bit = 0; bit < 8; ++bit) {
             bool value = (byteArray[i] >> (7 - bit)) & 1;
             bits.setBit(i * 8 + bit, value);
         }
     }
     return bits;
 }

 QByteArray FCDecodificator::bitArrayToByteArray(const QBitArray &bitArray)
 {
     QByteArray byteArray((bitArray.size() + 7) / 8, 0);
     for (int i = 0; i < bitArray.size(); ++i) {
         byteArray[i / 8] |= (bitArray.testBit(i) ? 1 : 0) << (7 - (i % 8));
     }
     return byteArray;
 }

 void FCDecodificator::readJson()
 {
     QString jsonFilePath = ":/jsons/decodificado.json";
     QFile file(jsonFilePath);
     if (file.open(QIODevice::ReadOnly)) {
         QByteArray bytes = file.readAll();
         file.close();
         QJsonParseError jsonError;
         QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
         if (jsonError.error == QJsonParseError::NoError && document.isObject()) {
             jsonFile = document.object();
         } else {
             qWarning() << "Error al leer JSON:" << jsonError.errorString();
         }
     } else {
         qWarning() << "No se pudo abrir el archivo JSON:" << jsonFilePath;
     }
 }
