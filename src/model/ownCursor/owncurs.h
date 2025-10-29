#ifndef OWNCURS_H
#define OWNCURS_H

#include <QObject>
#include <QPair>
#include <qfloat16.h>

#include "entities/cursorEntity.h"  // Usa setters y tipos de CursorEntity
#include "commandContext.h"
#include "obmHandler.h"
#define ORIGIN {0.0,0.0}

class OwnCurs : public QObject {
    Q_OBJECT
public:
    explicit OwnCurs(CommandContext* ctx, OBMHandler* newObm, QObject* parent = nullptr);

public slots:
    void cuOrOffCent();    // ← faltaba 'void'
    void cuOrCent();
    void offCent();
    void cent();

    void updateHandwheel(const QPair<qfloat16, qfloat16>& update);     // ← const& para evitar copia

private:
    CursorEntity*   cursor = nullptr;
    CommandContext* ctx    = nullptr;
    OBMHandler*     obm    = nullptr;
};

#endif // OWNCURS_H
