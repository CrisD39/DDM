// overlayhandler.h
#pragma once
#include <QObject>
#include <memory>
#include "obmHandler.h"
#include "qek.h"
#include "ownCursor/owncurs.h"

class OverlayHandler : public QObject {
    Q_OBJECT
public:
    explicit OverlayHandler(QObject* parent = nullptr);

    void setContext(CommandContext *ctx){this->ctx = ctx;};
    void setOBMHandler(OBMHandler *oh){this->obmHandler = oh;}
public slots:
    void onNewOverlay(const QString& overlayName); // e.g. "SPC", "OPS", "APC"
    void onNewQEK(const QString& qekStr);          // e.g. "QEK_20"
    void onNewCursor();

private:
    std::unique_ptr<QEK> myQEK;
    QString currentOverlay;
    CommandContext* ctx;
    OBMHandler* obmHandler;
    std::unique_ptr<QEK> instanceNewQEK(const QString& overlayName);
    void executeQEK(Qek which);
    OwnCurs* owncurse;
};
