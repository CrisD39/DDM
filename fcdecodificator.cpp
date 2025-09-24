#include "fcdecodificator.h"


#define WORD_SIZE 24
#define WORD_COUNT 9

FCDecodificator::FCDecodificator() {}

void FCDecodificator::decodificate(QByteArray message)
{
    *inComingMessage = byteArrayToBitArray(message);

    decomsg1();
}

FCDecodificator::decomsg1()
{
    //decodificar range
    int i = 0;
    QString rangeAux;
    QJsonObject rangeJson = jsonFile["Range"];
    for(i; i<3;i++){
        rangeAux.append(inComingMessage[i]);
    }




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
