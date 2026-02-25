#ifndef TYPESDATA_H
#define TYPESDATA_H

#include <QObject>    // Q_GADGET / Q_ENUM
#include <QMetaEnum>
#include <QString>

class TrackData {
    Q_GADGET

public:
    // ==== Enums (estilo ButtonsData: NO "enum class") ====
    enum Type { Air, Surface, Subsurface, RP, ESM };
    Q_ENUM(Type)

    // Tabla SURFACE TRACKS:
    // P Pending
    // A PossFriend
    // F ConfFriend
    // E PossHostile
    // H ConfHostile
    // U EvalUnknown
    // Y Heli
    enum Identity { Pending, PossHostile, PossFriend, ConfHostile, ConfFriend, EvalUnknown, Heli };
    Q_ENUM(Identity)

    enum TrackMode { FC1, FC2, FC3, FC4, FC5, Auto, TentativeAuto, AutomaticLost, RAM, DR, Lost, Blank };
    Q_ENUM(TrackMode)

    // ==== NUEVO: Estados Link Y (R/C/T/S y inválido) ====
    // Requerimiento:
    // R: solo recibido
    // C: correlacionado
    // T: transmitido
    // S: transmitido (otra categoría)
    // - : inválido
    enum LinkYStatus { LinkY_R, LinkY_C, LinkY_T, LinkY_S, LinkY_Invalid };
    Q_ENUM(LinkYStatus)

    // ==== NUEVO: Estado Link 14 (T y inválido) ====
    enum Link14Status { Link14_T, Link14_Invalid };
    Q_ENUM(Link14Status)

    // ==== NUEVO: Asignación FC (1..6, inválido) ====
    // Requerimiento:
    // 1..6 (válido)
    // - (inválido)
    enum AsignacionFC { FC_Invalid = 0, FC_1 = 1, FC_2 = 2, FC_3 = 3, FC_4 = 4, FC_5 = 5, FC_6 = 6 };
    Q_ENUM(AsignacionFC)

    // ==== Helpers: de enum a QString sin switch ====
    static inline QString toQString(Type v)         { return enumName(v, "Unknown"); }
    static inline QString toQString(Identity v)     { return enumName(v, "Unknown"); }
    static inline QString toQString(TrackMode v)    { return enumName(v, "Unknown"); }
    static inline QString toQString(LinkYStatus v)  { return enumName(v, "Unknown"); }
    static inline QString toQString(Link14Status v) { return enumName(v, "Unknown"); }
    static inline QString toQString(AsignacionFC v) { return enumName(v, "Unknown"); }

    // ==== Helpers opcionales: “código” 1 letra según tabla ====
    // Útil para que SITREP muestre P/A/F/E/H/U/Y aunque toQString devuelva "Pending", etc.
    static inline QChar identityCode(Identity v) {
        switch (v) {
        case Pending:     return QLatin1Char('P');
        case PossFriend:  return QLatin1Char('A');
        case ConfFriend:  return QLatin1Char('F');
        case PossHostile: return QLatin1Char('E');
        case ConfHostile: return QLatin1Char('H');
        case EvalUnknown: return QLatin1Char('U');
        case Heli:        return QLatin1Char('Y');
        default:          return QLatin1Char('P');
        }
    }

    static inline QChar linkYCode(LinkYStatus v) {
        switch (v) {
        case LinkY_R: return QLatin1Char('R');
        case LinkY_C: return QLatin1Char('C');
        case LinkY_T: return QLatin1Char('T');
        case LinkY_S: return QLatin1Char('S');
        default:      return QLatin1Char('-');
        }
    }

    static inline QChar link14Code(Link14Status v) {
        switch (v) {
        case Link14_T: return QLatin1Char('T');
        default:       return QLatin1Char('-');
        }
    }

private:
    template <typename E>
    static inline QString enumName(E v, const char* fallback) {
        const QMetaEnum me = QMetaEnum::fromType<E>();
        const char* key = me.valueToKey(static_cast<int>(v));
        return key ? QString::fromLatin1(key) : QString::fromLatin1(fallback);
    }
};

#endif // TYPESDATA_H
