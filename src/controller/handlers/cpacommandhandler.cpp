#include "cpacommandhandler.h"
#include "cpa.h"
#include "../json/jsonresponsebuilder.h"

CpaCommandHandler::CpaCommandHandler(CommandContext *c)
    : ctx(c)
{
    Q_ASSERT(ctx);
}

QByteArray CpaCommandHandler::startCPA(const QJsonObject &args)
{
    //Extraccion de datos
    int index = args.value("index").toInt();
    int idTrackA = args.value("track_a").toInt();
    int idTrackB = args.value("track_b").toInt();

    CPAResult result = CPA::instance(index).fromJSON(idTrackA,idTrackB, *ctx);
    return buildStartResponse(index, result);

}

QByteArray CpaCommandHandler::buildStartResponse(int index, CPAResult _cpa)
{
    QJsonObject args;
    args["index"] = index;
    QString toRet = "AZ "   + QString::number(_cpa.azimuth) + "° / DT "
                            + QString::number(_cpa.dcpa) + "MN - "
                            + QString::number(_cpa.tcpa);
    args["result"] = toRet;
    return JsonResponseBuilder::buildSuccessResponse("cpa_start", args);
}
