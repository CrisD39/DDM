#ifndef CPA_H
#define CPA_H

#include <QObject>
#include "iCommand.h"        // FUNDAMENTAL para CommandResult
#include "commanddispatcher.h" // FUNDAMENTAL para CommandContext

struct CPAResult {
    double tcpa;
    double dcpa;
    bool valid;
};

class CPA : public QObject
{
    Q_OBJECT
public:
    explicit CPA(QObject *parent = nullptr);

    // Agregamos la declaración aquí
    CommandResult fromCLI(int id1, int id2, CommandContext& ctx);

    CPAResult computeCPA(const Track &track_a, const Track &track_b);
};

#endif // CPA_H