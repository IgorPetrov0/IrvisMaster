#ifndef MBTCPDRIVER_H
#define MBTCPDRIVER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QUrl>
#include <QDate>




class mbtcpDriver : public QObject
{
    Q_OBJECT
public:
    mbtcpDriver();
    ~mbtcpDriver();
    QString getDeviceInfo(unsigned char devAddress);
    QByteArray getDayData(unsigned char devAddress, unsigned char numberPP, QDate date);
    QByteArray getDataForDay(unsigned char devAddress, unsigned char mode, unsigned char numberPP, QDate date);
    QByteArray getDeviceTime(unsigned char devAddress);
    bool isConnected();
    bool connectWithGateway(QString address);
    void disconnectFromGateway();



protected:
    QTcpSocket *socket;
    QString currentIPAddress;
    unsigned char currentDevAddress;
    quint16 transactionIdentifier;
    void deleteSocket();
    bool socketReady();

protected slots:
    void connectedSlot();
    void disconnectedSlot();
    void errorSlot(QAbstractSocket::SocketError socketError);

signals:
    void connectedSignal();
    void disconnectedSignal();
    void errorSignal(QAbstractSocket::SocketError socketError);



};

#endif // MBTCPDRIVER_H
