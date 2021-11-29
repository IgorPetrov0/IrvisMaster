#include "irvisreader.h"

IRVISReader::IRVISReader(QObject *parent):
    QObject(parent)
{
    lastRequest = nullptr;
    modbusDevice = new QModbusTcpClient(this);
    connect(modbusDevice,SIGNAL(stateChanged(QModbusDevice::State)),this,SLOT(modbusStateChangedSlot(QModbusDevice::State)));

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISReader::connectToIRVIS(QString host, QString pass, int addr){
    if (modbusDevice->state() != QModbusDevice::ConnectedState){
        const QUrl url = QUrl::fromUserInput(host);
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        QModbusDevice::State state=modbusDevice->state();
        if (!modbusDevice->connectDevice()) {
            state=modbusDevice->state();
            lastError = tr("Ошибка соединения: ") + modbusDevice->errorString();
            password.hB=0;
            password.lB=0;
            address=0;
            return false;
        }
        else {
            state=modbusDevice->state();
            int tmp=pass.toInt();
            uint16_t tmpInt=static_cast<uint16_t>(tmp);
            password.lB=tmpInt & 0x00FF;
            password.hB=tmpInt>>8;
            address=addr;
        }
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISReader::readForDay(QDate date){
    QByteArray requestArray;
    requestArray.append(1);//команда

    QModbusRequest request(QModbusPdu::FunctionCode(0x46),requestArray);

    if (auto *reply = modbusDevice->sendRawRequest(request, address)){
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &IRVISReader::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        lastError=tr("Ошибка чтения");
        return false;
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
QString IRVISReader::getLastError(){
    QString tmp = lastError;
    lastError.clear();
    return lastError;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISReader::readReady(){
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QModbusResponse resp=reply->rawResult();
        int (IRVISReader::*cFunct)(const QModbusResponse &);
        cFunct = &IRVISReader::calcFunction;
        //QModbusResponse::registerDataSizeCalculator(static_cast<QModbusPdu::FunctionCode>(70),cFunct);


    }
    reply->deleteLater();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISReader::modbusStateChangedSlot(QModbusDevice::State state){
    switch(state){
        case(QModbusDevice::UnconnectedState):{
            emit changeReaderStateSignal("Not connected");
            break;
        }
        case(QModbusDevice::ConnectingState):{
            emit changeReaderStateSignal("Connecting");
            break;
        }
        case(QModbusDevice::ConnectedState):{
            emit changeReaderStateSignal("Connected");
            break;
        }
        case(QModbusDevice::ClosingState):{
            emit changeReaderStateSignal("Closing");
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISReader::readPacket(uchar mode, QDate date){
    QByteArray requestArray;
    requestArray.append(static_cast<char>(1));//команда
    requestArray.append(static_cast<char>(0));//номер ПП
    requestArray.append(mode);//режим
    requestArray.append(static_cast<char>(date.day()));//дата/число
    requestArray.append(static_cast<char>(date.month()));//дата/месяц
    requestArray.append(static_cast<char>(date.year()));//дата/год


    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int IRVISReader::calcFunction(const QModbusResponse &pdu){
    return 10;
}
//////////////////////////////////////////////////////////////////////////////////////////////
