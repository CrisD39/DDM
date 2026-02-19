#include "cpa.h"

CPA::CPA(QObject *parent)
    : QObject{parent}
{}

CPAResult CPA::computeCPA(const Track& a, const Track& b)
{
    // Posiciones
    QPair<double,double> Pa { a.getX(), a.getY() };
    QPair<double,double> Pb { b.getX(), b.getY() };

    // Velocidad en DM/s
    double Va = a.speedKnots() / 3600.0;
    double Vb = b.speedKnots() / 3600.0;

    // Rumbo en radianes
    double ha = qDegreesToRadians(a.course());
    double hb = qDegreesToRadians(b.course());

    // Vectores velocidad
    QPair<double,double> Va_vec {
        Va * std::sin(ha),
        Va * std::cos(ha)
    };

    QPair<double,double> Vb_vec {
        Vb * std::sin(hb),
        Vb * std::cos(hb)
    };

    // Posición relativa
    QPair<double,double> Pr {
        Pb.first - Pa.first,
        Pb.second - Pa.second
    };

    // Velocidad relativa
    QPair<double,double> Vr {
        Vb_vec.first - Va_vec.first,
        Vb_vec.second - Va_vec.second
    };

    // |Vr|^2
    double vr2 = Vr.first * Vr.first + Vr.second * Vr.second;

    if (vr2 < 1e-9) {
        double dist = std::sqrt(Pr.first*Pr.first + Pr.second*Pr.second);
        return {0.0, dist, false};
    }

    // TCPA
    double tcpa = - ((Pr.first * Vr.first) + (Pr.second * Vr.second)) / vr2;

    if (tcpa < 0)
        tcpa = 0;

    // Posición relativa en CPA
    QPair<double,double> Pcpa {
        Pr.first + Vr.first * tcpa,
        Pr.second + Vr.second * tcpa
    };

    // DCPA
    double dcpa = std::sqrt(Pcpa.first * Pcpa.first +
                            Pcpa.second * Pcpa.second);

    qDebug() << "\nRESULTADO CPA:" << tcpa << "-" << dcpa;

    return { tcpa, dcpa, true };
}
