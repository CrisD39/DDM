    #include "enums/enumsOwnShip.h"
    #include "ownship.h"

    // ==================== Ctor ====================
    OwnShip::OwnShip() = default;

    // ==================== Utils ====================
    bool OwnShip::inRange(double v, double lo, double hi) { return v >= lo && v <= hi; }

    bool OwnShip::isTimeGmtZ(const QString& t) {
        static const QRegularExpression rx(R"(^([01]\d|2[0-3])[0-5]\dZ$)");
        return rx.match(t).hasMatch();
    }
    bool OwnShip::isLocalTime(const QString& t) {
        // HHMMXSS, X=A..Z sin J
        static const QRegularExpression rx(R"(^([01]\d|2[0-3])[0-5]\d[A-IK-Z][0-5]\d$)");
        return rx.match(t).hasMatch();
    }
    bool OwnShip::isLatitude(const QString& s) {
        // DD-MM.M{N|S} con DD=00..90, MM=00..59, M=0..9
        static const QRegularExpression rx(R"(^(?:[0-8]\d|90)-[0-5]\d\.[0-9][NS]$)");
        return rx.match(s).hasMatch();
    }
    bool OwnShip::isLongitude(const QString& s) {
        // DDD-MM.M{E|W} con DDD=000..180; si 180 => MM.M = 00.0
        static const QRegularExpression rx(R"(^((?:0\d\d|1[0-7]\d|180)-[0-5]\d\.[0-9])[EW]$)");
        auto m = rx.match(s);
        if (!m.hasMatch()) return false;
        int deg = s.left(3).toInt();
        if (deg == 180) {
            QString mmM = s.mid(4,4); // "MM.M"
            return (mmM == "00.0");
        }
        return true;
    }

    // ==================== DTG ====================
    int OwnShip::getDate() const { return date_; }
    const QString& OwnShip::getTimeGmt() const { return timeGmt_; }
    Month OwnShip::getMonth() const { return month_; }
    int OwnShip::getYear() const { return year_; }
    const QString& OwnShip::getLocalTime() const { return localTime_; }

    bool OwnShip::setDate(int d) {
        if (d < 0 || d > 31) return false;
        date_ = d; return true;
    }
    bool OwnShip::setTimeGmt(const QString& hhmmZ) {
        if (!isTimeGmtZ(hhmmZ)) return false;
        timeGmt_ = hhmmZ; return true;
    }
    void OwnShip::setMonth(Month m) { month_ = m; }
    bool OwnShip::setYear(int yy) {
        if (yy < 0 || yy > 99) return false;
        year_ = yy; return true;
    }
    bool OwnShip::setLocalTime(const QString& t) {
        if (!isLocalTime(t)) return false;
        localTime_ = t; return true;
    }

    // ==================== Curso / Velocidad ====================
    Mode OwnShip::getCourseMode() const { return courseMode_; }
    int  OwnShip::getCourseDeg()  const { return courseDeg_; }
    Mode OwnShip::getSpeedMode()  const { return speedMode_; }
    double OwnShip::getSpeedDmPerHour() const { return speedDmPerHour_; }

    bool OwnShip::setCourseMode(Mode m) { courseMode_ = m; return true; }
    bool OwnShip::setCourseDeg(int deg) {
        if (deg < 0 || deg > 359) return false;
        courseDeg_ = deg; return true;
    }
    bool OwnShip::setSpeedMode(Mode m) { speedMode_ = m; return true; }
    bool OwnShip::setSpeedDmPerHour(double dmph) {
        if (!inRange(dmph, 0.0, 99.9)) return false;
        speedDmPerHour_ = dmph; return true;
    }

    // ==================== Deriva / Corriente ====================
    int OwnShip::getDriftCourseDeg() const { return driftCourseDeg_; }
    double OwnShip::getDriftSpeedDmPerHour() const { return driftSpeedDmPerHour_; }
    const QString& OwnShip::getDriftRefPoint() const { return driftRefPoint_; }
    const QString& OwnShip::getDriftInitTime() const { return driftInitTime_; }

    bool OwnShip::setDriftCourseDeg(int deg) {
        if (deg < 0 || deg > 359) return false;
        driftCourseDeg_ = deg; return true;
    }
    bool OwnShip::setDriftSpeedDmPerHour(double dmph) {
        if (!inRange(dmph, 0.0, 99.9)) return false;
        driftSpeedDmPerHour_ = dmph; return true;
    }
    void OwnShip::setDriftRefPoint(const QString& s) { driftRefPoint_ = s; }
    bool OwnShip::setDriftInitTime(const QString& hhmmZ) {
        if (!isTimeGmtZ(hhmmZ)) return false;
        driftInitTime_ = hhmmZ; return true;
    }

    // ==================== Punto de Track + Posición ====================
    const QString& OwnShip::getTrackRef() const { return trackRef_; }
    const QString& OwnShip::getLat() const { return lat_; }
    const QString& OwnShip::getLon() const { return lon_; }
    const QString& OwnShip::getTrackInitTime() const { return trackInitTime_; }

    void OwnShip::setTrackRef(const QString& s) { trackRef_ = s; }
    bool OwnShip::setLat(const QString& s) { if (!isLatitude(s)) return false; lat_ = s; return true; }
    bool OwnShip::setLon(const QString& s) { if (!isLongitude(s)) return false; lon_ = s; return true; }
    bool OwnShip::setTrackInitTime(const QString& hhmmZ) { if (!isTimeGmtZ(hhmmZ)) return false; trackInitTime_ = hhmmZ; return true; }

    // ==================== DLRP ====================
    const QString& OwnShip::getDlrpLat() const { return dlrpLat_; }
    const QString& OwnShip::getDlrpLon() const { return dlrpLon_; }
    const QString& OwnShip::getDlrpInitTime() const { return dlrpInitTime_; }

    bool OwnShip::setDlrpLat(const QString& s) { if (!isLatitude(s)) return false; dlrpLat_ = s; return true; }
    bool OwnShip::setDlrpLon(const QString& s) { if (!isLongitude(s)) return false; dlrpLon_ = s; return true; }
    bool OwnShip::setDlrpInitTime(const QString& hhmmZ) { if (!isTimeGmtZ(hhmmZ)) return false; dlrpInitTime_ = hhmmZ; return true; }

    // ==================== Grid Lock ====================
    const QString&  OwnShip::getGridLockRefTrack() const { return gridLockRefTrack_; }
    GridLockMode    OwnShip::getGridLockMode() const { return gridLockMode_; }
    const QString&  OwnShip::getGridLockInitTime() const { return gridLockInitTime_; }

    void OwnShip::setGridLockRefTrack(const QString& s) { gridLockRefTrack_ = s; }
    void OwnShip::setGridLockMode(GridLockMode m) { gridLockMode_ = m; }
    bool OwnShip::setGridLockInitTime(const QString& hhmmZ) { if (!isTimeGmtZ(hhmmZ)) return false; gridLockInitTime_ = hhmmZ; return true; }

    // ==================== Coordenadas de Grilla ====================
    const QString& OwnShip::getGridCoordsRefTrack() const { return gridCoordsRefTrack_; }
    Colour          OwnShip::getGridQuadrantColour() const { return gridQuadrantColour_; }
    double          OwnShip::getGridXdm() const { return gridX_; }
    double          OwnShip::getGridYdm() const { return gridY_; }
    const QString&  OwnShip::getGridCoordsInitTime() const { return gridCoordsInitTime_; }

    void OwnShip::setGridCoordsRefTrack(const QString& s) { gridCoordsRefTrack_ = s; }
    void OwnShip::setGridQuadrantColour(Colour c) { gridQuadrantColour_ = c; }
    bool OwnShip::setGridXdm(double v) { if (!inRange(v, 0.0, 256.0)) return false; gridX_ = v; return true; }
    bool OwnShip::setGridYdm(double v) { if (!inRange(v, 0.0, 256.0)) return false; gridY_ = v; return true; }
    bool OwnShip::setGridCoordsInitTime(const QString& hhmmZ) { if (!isTimeGmtZ(hhmmZ)) return false; gridCoordsInitTime_ = hhmmZ; return true; }

    // ==================== TN bounds + TN propio ====================
    int OwnShip::getTnMin() const { return tnMin_; }
    int OwnShip::getTnMax() const { return tnMax_; }
    const QString& OwnShip::getOwnShipTN() const { return ownShipTN_; }
    const QString& OwnShip::getTnInitTime() const { return tnInitTime_; }

    bool OwnShip::setTnMin(int v) {
        if (v < 100 || v > 7776) return false;
        if (v > tnMax_) tnMax_ = v;
        tnMin_ = v; return true;
    }
    bool OwnShip::setTnMax(int v) {
        if (v < 100 || v > 7776 || v < tnMin_) return false;
        tnMax_ = v; return true;
    }
    bool OwnShip::setOwnShipTN(const QString& s) {
        static const QRegularExpression rx(R"(^S\d{4}b?$)");
        if (!rx.match(s).hasMatch()) return false;
        ownShipTN_ = s; return true;
    }
    bool OwnShip::setTnInitTime(const QString& hhmmZ) { if (!isTimeGmtZ(hhmmZ)) return false; tnInitTime_ = hhmmZ; return true; }

    // ==================== Colores de cuadrantes de superficie ====================
    Colour OwnShip::getGridNW() const { return gridNW_; }
    Colour OwnShip::getGridNE() const { return gridNE_; }
    Colour OwnShip::getGridSE() const { return gridSE_; }
    Colour OwnShip::getGridSW() const { return gridSW_; }
    const QString& OwnShip::getGridColoursInitTime() const { return gridColoursInitTime_; }

    void OwnShip::setGridNW(Colour c) { gridNW_ = c; }
    void OwnShip::setGridNE(Colour c) { gridNE_ = c; }
    void OwnShip::setGridSE(Colour c) { gridSE_ = c; }
    void OwnShip::setGridSW(Colour c) { gridSW_ = c; }
    bool OwnShip::setGridColoursInitTime(const QString& hhmmZ) { if (!isTimeGmtZ(hhmmZ)) return false; gridColoursInitTime_ = hhmmZ; return true; }
