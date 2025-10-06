#ifndef ENCODERLPD_H
#define ENCODERLPD_H

#include "CommandContext.h"
#include <QObject>
#include <QByteArray>
#include <QPair>
#include <QMap>

// =============================================================
// Clase encoderLPD
// -------------------------------------------------------------
// Responsable de codificar la información del dominio (Tracks)
// en formato binario AB2. Cada Track se convierte en una
// secuencia de bytes que incluye coordenadas, símbolo e ID.
// =============================================================
class encoderLPD : public QObject
{
    Q_OBJECT
public:
    explicit encoderLPD();

    // ---------------------------------------------------------
    // Construye un mensaje AB2 completo (header + descentrado + tracks)
    // ---------------------------------------------------------
    QByteArray buildFullMessage(const CommandContext &ctx);

private:
    // ---------------------------------------------------------
    // Codifica un Track en formato AB2 (coordenadas + símbolo)
    // ---------------------------------------------------------
    QByteArray buildAB2Message(const Track &track);

    // ---------------------------------------------------------
    // Codifica una coordenada (X o Y) en 24 bits:
    // 17 bits en complemento a dos + 7 bits de ID y flags (LS/PV/V/AP)
    // ---------------------------------------------------------
    QByteArray encodeCoordinate(double value, uint8_t idBits);

    // ---------------------------------------------------------
    // Construye los 12 bytes del campo de símbolo AB2:
    // - 2 primeros: código del símbolo (según type + identity)
    // - 3 siguientes: campos fijos (reservados)
    // - 4 siguientes: ID del track en formato octal ASCII
    // - 2 nulos
    // - 1 byte final EOMM (End Of Marker Message)
    // ---------------------------------------------------------
    QByteArray buildSymbolBytes(const Track &track) const;

    // ---------------------------------------------------------
    // Devuelve el par de bytes que identifican el símbolo
    // a partir del tipo de track (Friendly, Enemy, Unknown)
    // y su identidad ("s", "a", "b").
    // ---------------------------------------------------------
    QPair<uint8_t, uint8_t> symbolFor(const Track &track) const;

    // ---------------------------------------------------------
    // Aplica NOT bitwise (~) a todos los bytes del bloque.
    // Se usa para invertir los datos después del header.
    // ---------------------------------------------------------
    QByteArray negateData(const QByteArray &data);
};

// =============================================================
// Constantes globales relacionadas con AB2
// (Declaradas como extern si se necesitan en otros módulos)
// =============================================================

// Bits de estado (por regla del negocio siempre en 1)
constexpr uint8_t BIT_LS = 1 << 6;   // Least Significant
constexpr uint8_t BIT_PV = 1 << 5;   // Present/Valid
constexpr uint8_t BIT_V  = 1 << 4;   // Visible
constexpr uint8_t BIT_AP = 1 << 3;   // Applicable

// IDs de palabra de coordenada
constexpr uint8_t AB2_ID_X = 0x01;
constexpr uint8_t AB2_ID_Y = 0x03;

// Constante de fin de mensaje de marcador
constexpr uint8_t EOMM = 0x17;

// Bytes fijos del header y descentrado
constexpr uint8_t HEADER_BYTES[2]       = { 0x00, 0x00 };
constexpr uint8_t DESCENTRADO_BYTES[6]  = { 0x00, 0x00, 0x19, 0x00, 0x00, 0x0B };

#endif // ENCODERLPD_H
