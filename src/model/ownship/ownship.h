#pragma once
#include <QString>
#include <QRegularExpression>
#include "enums/enumsOwnShip.h"   // Debe definir: enum class Month, Mode, GridLockMode, Colour

class OwnShip {
public:
    OwnShip();

    // ==================== Utils de validación (estáticas) ====================
    static bool isTimeGmtZ(const QString& t);        // "HHMMZ"
    static bool isLocalTime(const QString& t);       // "HHMMXSS" (X=A..Z, sin J)
    static bool isLatitude(const QString& s);        // "DD-MM.M{N|S}"
    static bool isLongitude(const QString& s);       // "DDD-MM.M{E|W}" (si DDD=180 => MM.M=00.0)
    static bool inRange(double v, double lo, double hi);

    // ==================== BLOQUE: Date / Time(GMT) / Month / Year / Local Time ====================
    int         getDate()        const;
    const QString& getTimeGmt()  const;
    Month       getMonth()       const;
    int         getYear()        const;
    const QString& getLocalTime() const;

    bool setDate(int d);                         // 0..31 (00..31 en tabla)
    bool setTimeGmt(const QString& hhmmZ);       // "HHMMZ"
    void setMonth(Month m);
    bool setYear(int yy);                        // 0..99
    bool setLocalTime(const QString& t);         // "HHMMXSS" (X A..Z sin J)

    // ==================== BLOQUE: Curso / Velocidad (con modos) ====================
    Mode  getCourseMode() const;
    int   getCourseDeg()  const;                 // 000..359
    Mode  getSpeedMode()  const;
    double getSpeedDmPerHour() const;            // 00.0..99.9 DM/h

    bool setCourseMode(Mode m);
    bool setCourseDeg(int deg);
    bool setSpeedMode(Mode m);
    bool setSpeedDmPerHour(double dmph);

    // ==================== BLOQUE: Deriva / Corriente ====================
    int          getDriftCourseDeg() const;          // 000..359
    double       getDriftSpeedDmPerHour() const;     // 00.0..99.9
    const QString& getDriftRefPoint() const;         // libre
    const QString& getDriftInitTime() const;         // "HHMMZ"

    bool setDriftCourseDeg(int deg);
    bool setDriftSpeedDmPerHour(double dmph);
    void setDriftRefPoint(const QString& s);
    bool setDriftInitTime(const QString& hhmmZ);

    // ==================== BLOQUE: Punto de Track + Posición (lat/lon) ====================
    const QString& getTrackRef() const;              // "track type + track number" (libre)
    const QString& getLat() const;                   // 00-00.0N … 90-00.0S
    const QString& getLon() const;                   // 000-00.0E … 180-00.0W
    const QString& getTrackInitTime() const;         // "HHMMZ"

    void  setTrackRef(const QString& s);
    bool  setLat(const QString& s);
    bool  setLon(const QString& s);
    bool  setTrackInitTime(const QString& hhmmZ);

    // ==================== BLOQUE: DLRP (Data Link Reference Point) ====================
    const QString& getDlrpLat() const;
    const QString& getDlrpLon() const;
    const QString& getDlrpInitTime() const;

    bool setDlrpLat(const QString& s);
    bool setDlrpLon(const QString& s);
    bool setDlrpInitTime(const QString& hhmmZ);

    // ==================== BLOQUE: Grid Lock ====================
    const QString&  getGridLockRefTrack() const;
    GridLockMode    getGridLockMode() const;
    const QString&  getGridLockInitTime() const;

    void setGridLockRefTrack(const QString& s);
    void setGridLockMode(GridLockMode m);
    bool setGridLockInitTime(const QString& hhmmZ);

    // ==================== BLOQUE: Coordenadas de grilla (con color de cuadrante) ====================
    const QString& getGridCoordsRefTrack() const;
    Colour         getGridQuadrantColour() const;    // G,R,W,B
    double         getGridXdm() const;               // 000.0..256.0 DM
    double         getGridYdm() const;               // 000.0..256.0 DM
    const QString& getGridCoordsInitTime() const;    // "HHMMZ"

    void setGridCoordsRefTrack(const QString& s);
    void setGridQuadrantColour(Colour c);
    bool setGridXdm(double v);
    bool setGridYdm(double v);
    bool setGridCoordsInitTime(const QString& hhmmZ);

    // ==================== BLOQUE: Límites de TN + TN de Own Ship ====================
    int            getTnMin() const;                 // 0100b … 7776b (usamos 100..7776 dec)
    int            getTnMax() const;
    const QString& getOwnShipTN() const;             // "S0001b … S0077b" (aceptamos "Sdddd" o "Sddddb")
    const QString& getTnInitTime() const;            // "HHMMZ"

    bool setTnMin(int v);
    bool setTnMax(int v);
    bool setOwnShipTN(const QString& s);
    bool setTnInitTime(const QString& hhmmZ);

    // ==================== BLOQUE: Colores de cuadrantes de la grilla de superficie ====================
    Colour         getGridNW() const;                // preset R
    Colour         getGridNE() const;                // preset W
    Colour         getGridSE() const;                // preset B
    Colour         getGridSW() const;                // preset G
    const QString& getGridColoursInitTime() const;   // "HHMMZ"

    void setGridNW(Colour c);
    void setGridNE(Colour c);
    void setGridSE(Colour c);
    void setGridSW(Colour c);
    bool setGridColoursInitTime(const QString& hhmmZ);

private:
    // ====== Date/Time Group ======
    int      date_{0};
    QString  timeGmt_{"0000Z"};
    Month    month_{Month::JAN};
    int      year_{0};
    QString  localTime_{"0000Z00"}; // "HHMMXSS"

    // ====== Curso / Velocidad ======
    Mode     courseMode_{Mode::AUT};
    int      courseDeg_{0};
    Mode     speedMode_{Mode::AUT};
    double   speedDmPerHour_{0.0};

    // ====== Deriva / Corriente ======
    int      driftCourseDeg_{0};
    double   driftSpeedDmPerHour_{0.0};
    QString  driftRefPoint_{};
    QString  driftInitTime_{"0000Z"};

    // ====== Punto de Track + Posición ======
    QString  trackRef_{};
    QString  lat_{};
    QString  lon_{};
    QString  trackInitTime_{"0000Z"};

    // ====== DLRP ======
    QString  dlrpLat_{};
    QString  dlrpLon_{};
    QString  dlrpInitTime_{"0000Z"};

    // ====== Grid Lock ======
    QString      gridLockRefTrack_{};
    GridLockMode gridLockMode_{GridLockMode::INVALID};
    QString      gridLockInitTime_{"0000Z"};

    // ====== Coordenadas de Grilla ======
    QString  gridCoordsRefTrack_{};
    Colour   gridQuadrantColour_{Colour::G};
    double   gridX_{0.0};
    double   gridY_{0.0};
    QString  gridCoordsInitTime_{"0000Z"};

    // ====== TN bounds + TN propio ======
    int      tnMin_{200};              // preset 0200
    int      tnMax_{577};              // preset 0577
    QString  ownShipTN_{"S0002"};      // preset S0002 (aceptamos "Sdddd" o "Sddddb")
    QString  tnInitTime_{"0000Z"};

    // ====== Colores cuadrantes grilla ======
    Colour   gridNW_{Colour::R};
    Colour   gridNE_{Colour::W};
    Colour   gridSE_{Colour::B};
    Colour   gridSW_{Colour::G};
    QString  gridColoursInitTime_{"0000Z"};
};
