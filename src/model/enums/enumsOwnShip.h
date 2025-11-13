#ifndef ENUMSOWNSHIP_H
#define ENUMSOWNSHIP_H

#include <QObject>

class OwnShipData {
    Q_GADGET
public:
    // Meses
    enum class Month { JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };
    Q_ENUM(Month)

    // Modos de curso/velocidad
    enum class Mode { AUT, MAN };
    Q_ENUM(Mode)

    // Grid Lock mode
    enum class GridLockMode { AUT, MAN, DLRP, GGRD, INVALID };
    Q_ENUM(GridLockMode)

    // Colores (G=green, R=red, W=white, B=blue)
    enum class Colour { G, R, W, B };
    Q_ENUM(Colour)
};

// ===== Alias en el espacio global =====
using Month        = OwnShipData::Month;
using Mode         = OwnShipData::Mode;
using GridLockMode = OwnShipData::GridLockMode;
using Colour       = OwnShipData::Colour;

#endif // ENUMSOWNSHIP_H
