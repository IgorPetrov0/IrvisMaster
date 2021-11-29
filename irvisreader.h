#ifndef IRVISREADER_H
#define IRVISREADER_H

#include <QObject>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QUrl>
#include <QVariant>
#include <QDate>

struct passStruct{
    uchar lB;
    uchar hB;
};

class IRVISReader : public QObject
{
    Q_OBJECT


public:
    IRVISReader(QObject *parent = nullptr);
    bool connectToIRVIS(QString host, QString pass, int addr);
    bool readForDay(QDate date);
    QString getLastError();


private:
    QModbusDataUnit readRequest() const;
    QModbusDataUnit writeRequest() const;

private slots:
    void readReady();
    void modbusStateChangedSlot(QModbusDevice::State state);

protected:
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;
    QString lastError;
    passStruct password;
    int address;

    bool readPacket(uchar mode, QDate date);
    int calcFunction(const QModbusResponse &pdu);



signals:
    void changeReaderStateSignal(QString state);


};

#endif // IRVISREADER_H
