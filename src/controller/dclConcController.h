#ifndef DCLCONCCONTROLLER_H
#define DCLCONCCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QByteArray>

class clientSocket;
class FCDecodificator;

class DclConcController : public QObject
{
    Q_OBJECT
public:
    explicit DclConcController(clientSocket* socket,
                               FCDecodificator* decodificator,
                               QObject* parent = nullptr);

    QByteArray negateData(const QByteArray &data);
private slots:
    void askForConcentrator();
    void onDatagram(const QByteArray&);

private:
    clientSocket* m_socket;
    FCDecodificator* m_decoder;
    QTimer m_timer;

    static QByteArray buildPedidoDclConc();
    static QByteArray buildAckFromSeq(quint16);
};

#endif // DCLCONCCONTROLLER_H
