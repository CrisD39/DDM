#include "cpacommand.h"
#include "cpa.h"

CommandResult CpaCommand::execute(const CommandInvocation &inv, CommandContext &ctx) const
{
    const QStringList& args = inv.args;

    if (args.size() < 2) {
        usage();
    }

    bool ok1 = false, ok2 = false;
    int id1 = args[0].toInt(&ok1);
    int id2 = args[1].toInt(&ok2);

    if (!ok1 || !ok2) {
        return {false, "IDs invalidos"};
    }

    const Track *t1 = ctx.findTrackById(id1);
    const Track *t2 = ctx.findTrackById(id2);

    if (!t1 || !t2) {
        return {false, "Uno o ambos tracks no existen"};
    }

    CPA ccppaa;
    CPAResult ret = ccppaa.computeCPA(*t1,*t2);



    QString out;
    QTextStream oss(&out);

    oss << "CPA RESULT\n";
    oss << "Tracks: " << id1 << " vs " << id2 << "\n";
    oss << "TCPA: " << ret.tcpa << " s\n";
    oss << "CPA Distance: " << ret.dcpa << " DM\n";

    if (ret.tcpa < 0) {
        oss << "Estado: Divergiendo\n";
    } else {
        oss << "Estado: Convergente\n";
    }

    return {true, out};

}
