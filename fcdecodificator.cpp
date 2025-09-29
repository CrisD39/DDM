#include "fcdecodificator.h"


#define WORD_SIZE 24
#define WORD_COUNT 9

FCDecodificator::FCDecodificator() {}

void FCDecodificator::decode(QByteArray message)
{
    currentBit = 0;
    *inComingMessage = byteArrayToBitArray(message);

    decomsg1();
}


void FCDecodificator::decomsg1()
    {
        // --- RANGE SCALE (bits 23-21) ---
        QString rangeBits;
        for (int i = 0; i <= 2; ++i) {
            rangeBits.append(inComingMessage->testBit(currentBit) ? '1' : '0');
            currentBit++;
        }

        QJsonObject rangeJson = jsonFile["RANGE_SCALE_DECODE"].toObject();
        if (rangeJson.contains(rangeBits)) {
            QString decodedRange = rangeJson[rangeBits].toString();
            // Guardamos como string o lo mapeamos a int/enum
            this->range = decodedRange.toInt();
            qDebug() << "[Decodificación] RANGE:" << rangeBits << "→" << decodedRange;
        }

        // --- DISPLAY SELECTION (bits 20-12) ---
        QVector<bool> decodedDisplaySelection;
        QJsonObject displayJson = jsonFile["DISPLAY_SELECTION_DECODE"].toObject();
    for (int i = 0; i <8; i++) {
        if (inComingMessage->testBit(i)) {
            QString key = QString::number(currentBit); // Según el JSON, 3=Air, 4=Surf...
            if (displayJson.contains(key)) {
            QString decoded = displayJson[key].toString();
            qDebug() << "[Decodificación] DISPLAY SELECTION bit" << i << "→" << decoded;
        }
            decodedDisplaySelection.append(true); // tendria que agregar los valores  ver como lo soluciono.
        } else {
            decodedDisplaySelection.append(false);
        }
         currentBit++;
    }

    this->displaySelection = decodedDisplaySelection;

    // --- THREAT ASSESSMENT (bits 11-8) ---
    QVector<bool> decodedThreatAssessment;
    QJsonObject threatJson = jsonFile["THREAT_ASSESMENT_DECODE"].toObject();
    for (int i = 0; i <= 5; ++i) {
        if (inComingMessage->testBit(i)) {
             QString key = QString::number(i); // JSON tiene "11", "12", "13", "14", "15"
            if (threatJson.contains(key)) {
                QString decoded = threatJson[key].toString();
                qDebug() << "[Decodificación] THREAT ASSESSMENT bit" << i << "→" << decoded;
            }
            decodedThreatAssessment.append(true);
            } else {
            decodedThreatAssessment.append(false);
        }
    }
    this->threatAssessment = decodedThreatAssessment;

    i += 8
}



 void FCDecodificator::decomsg2()
 {
     QVector<bool> decodedDisplayMode(16, false);
     QJsonObject displayModeJson = jsonFile["DISPLAY_MODE_DECODE"].toObject();

     // --- Display Mode Izquierda (bits 23–18) ---
     for (int i = 23; i >= 18; --i) {
         bool bitVal = inComingMessage->testBit(i);
         decodedDisplayMode[23 - i] = bitVal; // guardamos en el vector
         if (bitVal) {
             QString key = QString::number(i);
             if (displayModeJson.contains(key)) {
                 QString decoded = displayModeJson[key].toString();
                 qDebug() << "[Decodificación] DISPLAY MODE IZQ bit" << i << "→" << decoded;
             }
         }
     }

     // --- TM (bit 17) ---
     if (inComingMessage->testBit(17)) {
         QString key = QString::number(17);
         if (displayModeJson.contains(key)) {
             qDebug() << "[Decodificación] TM →" << displayModeJson[key].toString();
         }
     }

     // --- OC (bit 16) ---
     if (inComingMessage->testBit(16)) {
         QString key = QString::number(16);
         if (displayModeJson.contains(key)) {
             qDebug() << "[Decodificación] OC →" << displayModeJson[key].toString();
         }
     }

     // --- Display Mode Derecha (bits 15–10) ---
     for (int i = 15; i >= 10; --i) {
         bool bitVal = inComingMessage->testBit(i);
         decodedDisplayMode[15 - i + 6] = bitVal;
         if (bitVal) {
             QString key = QString::number(i);
             if (displayModeJson.contains(key)) {
                 QString decoded = displayModeJson[key].toString();
                 qDebug() << "[Decodificación] DISPLAY MODE DER bit" << i << "→" << decoded;
             }
         }
     }

     // --- SA (bit 9) ---
     if (inComingMessage->testBit(9)) {
         QString key = QString::number(9);
         if (displayModeJson.contains(key)) {
             qDebug() << "[Decodificación] SA →" << displayModeJson[key].toString();
         }
     }

     // --- RR (bit 8) ---
     if (inComingMessage->testBit(8)) {
         QString key = QString::number(8);
         if (displayModeJson.contains(key)) {
             qDebug() << "[Decodificación] RR →" << displayModeJson[key].toString();
         }
     }

     // Guardamos en la variable interna
     this->displayMode = decodedDisplayMode;
 }

 void FCDecodificator::decomsg4()
 {
     // --- QEK Master (bits 23–16) ---
     QString qekMasterBits;
     for (int i = 23; i >= 16; --i) {
         qekMasterBits.append(inComingMessage->testBit(i) ? '1' : '0');
     }

     QJsonObject qekJson = jsonFile["QEK_DECODE"].toObject();
     if (qekJson.contains(qekMasterBits)) {
         QString decodedQEK = qekJson[qekMasterBits].toString();
         this->quickEntryKeyboardMaster.clear();
         for (QChar bit : qekMasterBits) {
             this->quickEntryKeyboardMaster.append(bit == '1');
         }
         qDebug() << "[Decodificación] QEK Master:" << qekMasterBits << "→" << decodedQEK;
     } else {
         qDebug() << "[Decodificación] QEK Master desconocido:" << qekMasterBits;
     }

     // --- QEK Slave (bits 15–8) ---
     QString qekSlaveBits;
     for (int i = 15; i >= 8; --i) {
         qekSlaveBits.append(inComingMessage->testBit(i) ? '1' : '0');
     }

     if (qekJson.contains(qekSlaveBits)) {
         QString decodedQEK = qekJson[qekSlaveBits].toString();
         this->quickEntryKeyboardSlave.clear();
         for (QChar bit : qekSlaveBits) {
             this->quickEntryKeyboardSlave.append(bit == '1');
         }
         qDebug() << "[Decodificación] QEK Slave:" << qekSlaveBits << "→" << decodedQEK;
     } else {
         qDebug() << "[Decodificación] QEK Slave desconocido:" << qekSlaveBits;
     }
 }


 void FCDecodificator::decomsg5()
 {
     // --- ICM Izquierdo (bits 23–21) ---
     QString icmMasterBits;
     for (int i = 23; i >= 21; --i) {
         icmMasterBits.append(inComingMessage->testBit(i) ? '1' : '0');
     }

     QJsonObject icmJson = jsonFile["ICM_DECODE"].toObject();
     if (icmJson.contains(icmMasterBits)) {
         this->icmMaster.clear();
         for (QChar bit : icmMasterBits) {
             this->icmMaster.append(bit == '1');
         }
         qDebug() << "[Decodificación] ICM Master:" << icmMasterBits
                  << "→" << icmJson[icmMasterBits].toString();
     }

     // --- Overlay Izquierdo (bits 19–16) ---
     QString overlayMasterBits;
     for (int i = 19; i >= 16; --i) {
         overlayMasterBits.append(inComingMessage->testBit(i) ? '1' : '0');
     }
     this->overlayMaster = overlayMasterBits;
     qDebug() << "[Decodificación] Overlay Master:" << overlayMasterBits;

     // --- ICM Derecho (bits 15–13) ---
     QString icmSlaveBits;
     for (int i = 15; i >= 13; --i) {
         icmSlaveBits.append(inComingMessage->testBit(i) ? '1' : '0');
     }

     if (icmJson.contains(icmSlaveBits)) {
         this->icmSlave.clear();
         for (QChar bit : icmSlaveBits) {
             this->icmSlave.append(bit == '1');
         }
         qDebug() << "[Decodificación] ICM Slave:" << icmSlaveBits
                  << "→" << icmJson[icmSlaveBits].toString();
     }

     // --- Overlay Derecho (bits 11–8) ---
     QString overlaySlaveBits;
     for (int i = 11; i >= 8; --i) {
         overlaySlaveBits.append(inComingMessage->testBit(i) ? '1' : '0');
     }
     this->overlaySlave = overlaySlaveBits;
     qDebug() << "[Decodificación] Overlay Slave:" << overlaySlaveBits;
 }

 void FCDecodificator::decomsg6()
 {
     // --- Handwheel ΔΦ (bits 23–16) ---
     int phiValue = 0;
     for (int i = 0; i < 8; ++i) {
         bool bit = inComingMessage->testBit(23 - i);
         phiValue = (phiValue << 1) | (bit ? 1 : 0);
     }
     // Conversión de complemento a dos (signed 8-bit)
     if (phiValue & 0x80) { // si el bit más significativo está en 1
         phiValue -= 256;
     }

     // --- Handwheel Δρ (bits 15–8) ---
     int rhoValue = 0;
     for (int i = 0; i < 8; ++i) {
         bool bit = inComingMessage->testBit(15 - i);
         rhoValue = (rhoValue << 1) | (bit ? 1 : 0);
     }
     if (rhoValue & 0x80) {
         rhoValue -= 256;
     }

     // Guardamos en los estados internos
     this->handWheelPhiQueue = phiValue;
     this->handWheelRhoQueue = rhoValue;

     qDebug() << "[Decodificación] Handwheel ΔΦ:" << phiValue
              << " Δρ:" << rhoValue;
 }

 void FCDecodificator::decomsg7()
 {
     // --- Rolling Ball ΔX (bits 23–16) ---
     int deltaX = 0;
     for (int i = 0; i < 8; ++i) {
         bool bit = inComingMessage->testBit(23 - i);
         deltaX = (deltaX << 1) | (bit ? 1 : 0);
     }

     // --- Rolling Ball ΔY (bits 15–8) ---
     int deltaY = 0;
     for (int i = 0; i < 8; ++i) {
         bool bit = inComingMessage->testBit(15 - i);
         deltaY = (deltaY << 1) | (bit ? 1 : 0);
     }

     // Guardamos en la cola como enteros
     RollingSteps step;
     step.first = QByteArray(1, static_cast<char>(deltaX));
     step.second = QByteArray(1, static_cast<char>(deltaY));
     this->rollingBallMaster.enqueue(step);

     qDebug() << "[Decodificación] Rolling Ball Master ΔX:" << deltaX
              << " ΔY:" << deltaY;
 }




QByteArray FCDecodificator::byteArrayToBitArray(const QByteArray bitArray)
{
    QBitArray bitArray(byteArray.size() * 8);

    for (int i = 0; i < byteArray.size(); ++i) {
        for (int bit = 0; bit < 8; ++bit) {
            bool value = (byteArray[i] >> (7 - bit)) & 1;
            bitArray.setBit(i * 8 + bit, value);
        }
    }

    return bitArray;
}

FCDecodificator::bitArrayToByteArray(const QBitArray bitArray)
{
    QByteArray byteArray((bitArray.size() + 7) / 8, 0); // Inicializa el QByteArray al tamaño necesario

    // Itera sobre cada bit y usa operaciones de desplazamiento para obtener su valor
    for (int i = 0; i < bitArray.size(); ++i) {
        byteArray[i / 8] |= (bitArray.testBit(i) ? 1 : 0) << (7 - (i % 8));
    }

    return byteArray;
}

void FCDecodificator::readJson()
{
    QString JsonFilePath = ":/jsons/decodificado.json";
    QFile File(JsonFilePath);
    if(File.open(QIODevice::ReadOnly))
    {
        QByteArray Bytes = File.readAll();
        File.close();
        QJsonParseError JsonError;
        QJsonDocument Document = QJsonDocument::fromJson(Bytes,&JsonError);
        if(JsonError.error != QJsonParseError::NoError){
            // qDebug()<<"ERROR in Json Data";
            return;
        }
        if(Document.isObject())
            jsonFile = Document.object();
    }
    // else
    // qDebug()<<"Hubo un error, no se abrio el buttonJson";

    //Tengo que decodificar la MIK también
    // QString MIKFilePath = ":/json/json/mik.json";
    // QFile MIKFile(MIKFilePath);
    // if(MIKFile.open(QIODevice::ReadOnly))
    // {
    //     QByteArray Bytes = MIKFile.readAll();
    //     MIKFile.close();
    //     QJsonParseError MIKJsonError;
    //     QJsonDocument MIKDocument = QJsonDocument::fromJson(Bytes,&MIKJsonError);
    //     if(MIKJsonError.error != QJsonParseError::NoError){
    //         qDebug()<<"ERROR in Json Data";
    //         return;
    //     }
    //     if(MIKDocument.isObject())
    //         MIKJson = MIKDocument.object();
    // }
    // else
    //     qDebug()<<"Hubo un error, no se abrio el buttonJson";
}
