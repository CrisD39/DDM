/*
    Helpers para prefijos/formatos de salida en consola (p. ej., OK/ERROR con color).
*/

#pragma once
#include <QTextStream>

namespace Console {
inline void clear(QTextStream& ts) {
    ts << "\x1b[2J\x1b[H"; // clear screen + cursor home
    ts.flush();
}
}
