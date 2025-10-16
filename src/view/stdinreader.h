#pragma once
#include <QObject>
#include <QString>

class StdinReader : public QObject {
    Q_OBJECT
public:
    explicit StdinReader(QObject* parent = nullptr);
public slots:
    void readLoop();
signals:
    void lineRead(const QString& l);
    void finished();
};
