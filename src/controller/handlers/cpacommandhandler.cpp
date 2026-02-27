#include "cpacommandhandler.h"
#include "cpa.h"

CpaCommandHandler::CpaCommandHandler() {}

QString CpaCommandHandler::startCPA(const QJsonObject &args)
{
    //Extraccion de datos
    int index = args.value("index").toInt();
    int idTrackA = args.value("track_a").toInt();
    int idTrackB = args.value("track_b").toInt();
    CPA cpa = new CPA(index);


}
