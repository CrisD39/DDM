#include "cpaCommand.h"
#include "src/model/cpa.h"

CommandResult CpaCommand::execute(const CommandInvocation &inv, CommandContext &ctx) const
{
    const QStringList& args = inv.args;

    if (args.size() < 2) {
        usage();
    }

    bool ok1 = false, ok2 = false;
    int idTrack1 = args[0].toInt(&ok1);
    int idTrack2 = args[1].toInt(&ok2);

    if (!ok1 || !ok2) {
        return {false, "IDs invalidos"};
    }

    CPA _cpa;
    CPAResult ret = _cpa.fromCLI(idTrack1,idTrack2,ctx);

    if(ret.valid == false && ret.tcpa == 0 && ret.dcpa){
        return {false, "Contactos no encontrados"};
    }

    QString out;
    QTextStream oss(&out);

    oss << "CPA RESULT\n";
    oss << "Tracks: " << idTrack1 << " vs " << idTrack2 << "\n";
    oss << "TCPA: " << ret.tcpa << " s\n";
    oss << "CPA Distance: " << ret.dcpa << " DM\n";

    if (ret.tcpa < 0) {
        oss << "Estado: Divergiendo\n";
    } else {
        oss << "Estado: Convergente\n";
    }

    return {true, out};

}
