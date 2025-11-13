#ifndef DCLCONCCONTROLLER_H
#define DCLCONCCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QByteArray>

class ITransport;   // interfaz de transporte (UDP, Local IPC, etc.)
class ConcDecoder;

class DclConcController : public QObject
{
    Q_OBJECT
public:
    explicit DclConcController(ITransport* link,
                               ConcDecoder* decodificator,
                               QObject* parent = nullptr);

    QByteArray negateData(const QByteArray &data);
private slots:
    void askForConcentrator();
public slots:
    void onDatagram(const QByteArray&);

private:
    ITransport* m_link;
    ConcDecoder* m_decoder;
    QTimer m_timer;

    static QByteArray buildPedidoDclConc();
    static QByteArray buildAckFromSeq(quint16);
};

#endif // DCLCONCCONTROLLER_H
