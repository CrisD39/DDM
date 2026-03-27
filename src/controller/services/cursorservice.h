#pragma once

#include <QString>
#include <QJsonArray>

class CommandContext;

struct CursorCreateRequest {
    double azimut = 0.0;
    double length = 0.0;
    double x = 0.0;
    double y = 0.0;
    int type = 0;
};

struct CursorOperationResult {
    bool success = false;
    QString errorCode;
    QString message;
    int cursorId = -1;
    QString lineId;
};

class CursorService {
public:
    explicit CursorService(CommandContext* context);

    CursorOperationResult createCursor(const CursorCreateRequest& request);
    CursorOperationResult deleteCursorById(int cursorId);

    QJsonArray serializeCursors() const;

    static QString lineIdFromCursorId(int cursorId);
    static int cursorIdFromLineId(const QString& lineId, bool* ok = nullptr);

private:
    CommandContext* m_context;
};
