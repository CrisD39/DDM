#include "cpa.h"

std::array<CPA, 10> CPA::m_instances;
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
    QPair<double,double> Pos_rel {
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
        double dist = std::sqrt(Pos_rel.first*Pos_rel.first + Pos_rel.second*Pos_rel.second);
        return {0.0, dist, false};
    }

    // TCPA
    double tcpa = - ((Pos_rel.first * Vr.first) + (Pos_rel.second * Vr.second)) / vr2;

    if (tcpa < 0)
        tcpa = 0;

    // Posición relativa en CPA
    QPair<double,double> Pcpa {
        Pos_rel.first + Vr.first * tcpa,
        Pos_rel.second + Vr.second * tcpa
    };

    // DCPA
    double dcpa = std::sqrt(Pcpa.first * Pcpa.first +
                            Pcpa.second * Pcpa.second);

    // Posiciones absolutas en CPA
    QPair<double,double> Pa_cpa {
        Pa.first  + Va_vec.first  * tcpa,
        Pa.second + Va_vec.second * tcpa
    };

    QPair<double,double> Pb_cpa {
        Pb.first  + Vb_vec.first  * tcpa,
        Pb.second + Vb_vec.second * tcpa
    };

    // Vector desde A hacia B en CPA
    QPair<double,double> Vec_cpa {
        Pb_cpa.first  - Pa_cpa.first,
        Pb_cpa.second - Pa_cpa.second
    };

    // Azimut (0° Norte, sentido horario)
    double az = std::atan2(Vec_cpa.first, Vec_cpa.second);
    double az_deg = qRadiansToDegrees(az);

    if (az_deg < 0)
        az_deg += 360.0;

    qDebug() << "\nRESULTADO CPA:" << tcpa << "-" << dcpa;

    return { tcpa, dcpa, az_deg, true };
}



CPAResult CPA::fromCLI(int idTrack1, int idTrack2, CommandContext &ctx)
{
    const Track *track1 = ctx.findTrackById(idTrack1);
    const Track *track2 = ctx.findTrackById(idTrack2);

    if (!track1 || !track2) {
        return {0,0,false};
    }

    return computeCPA(*track1,*track2);
}

CPAResult CPA::fromJSON(int idTrack1, int idTrack2, CommandContext &ctx)
{
    const Track *track1 = ctx.findTrackById(idTrack1);
    const Track *track2 = ctx.findTrackById(idTrack2);

    if (!track1 || !track2) {
        return {0,0,false};
    }

    return computeCPA(*track1,*track2);
}

CPA &CPA::instance(int index)
{
     return m_instances.at(index);
}
