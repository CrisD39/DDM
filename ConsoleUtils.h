#pragma once
#include <QTextStream>

namespace Console {
inline void clear(QTextStream& ts) {
    ts << "\x1b[2J\x1b[H"; // clear screen + cursor home
    ts.flush();
}
}
