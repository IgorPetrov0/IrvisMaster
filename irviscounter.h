#ifndef IRVISCOUNTER_H
#define IRVISCOUNTER_H

#include <QDateTime>
#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QTimer>
#include "mbtcpdriver.h"
#include "daydatablock.h"
#include "irvistime.h"

class IRVISCounter : public QObject
{
    Q_OBJECT

public:
    IRVISCounter();
    bool isConnected();
    bool isWork();
    void stopCounter();
    bool startCounter();
    QString getIPAddress() const;
    bool setIPAddress(const QString &value);
    IRVISTime getTime(int index) const;
    void setTime(int index, const QTime &value);
    QTime getOriginTime(int index) const;
    QTime getDeviceCalcTime() const;
    unsigned char getDevAddress() const;
    bool setDevAddress(int value);
    int getPassword() const;
    bool setPassword(QString pass);
    QString getDevData() const;
    void disableTime(int index);
    QDateTime readDevDateTime();
    void serialise(QDataStream *str);
    bool deserialise(QDataStream *str);
    void connectFromHost();
    void disconnectFromHost();
    QString getTimeComment(int index);
    void setTimeComment(int index, QString comment);
    void setChannel(int time, int channel, bool state);
    bool getChannel(int time, int channel);

protected:
    bool addBlocks(QByteArray *array);
    void readDevHoursData();
    void readDevDayData();


    QTime deviceCalcTime;
    IRVISTime time1;
    IRVISTime time2;
    IRVISTime time3;
    //времена без сдвига на уход часов счетчика
    QTime time1Origin;
    QTime time2Origin;
    QTime time3Origin;

    QString IPAddress;
    unsigned char devAddress;
    quint16 password;
    QString devData;
    mbtcpDriver tcpDriver;
    QVector<dayDataBlock> blocksArray;
    QTimer timer;
    int interval;
    QDateTime currentDevDateTime;



protected slots:
    void connectedSlot();
    void disconnectedSlot();
    void errorSlot(QAbstractSocket::SocketError error);
    void timerSlot();


signals:
    void errorSignal(bool critical, QString error);
    void timeChanged();
    void dataReadySignal(dayDataBlock block1, dayDataBlock block2, QString timeComment);
};

#endif // IRVISCOUNTER_H
