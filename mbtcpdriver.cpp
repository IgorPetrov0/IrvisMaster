#include "mbtcpdriver.h"

mbtcpDriver::mbtcpDriver(){
    socket = nullptr;
    transactionIdentifier = 1;
}
/////////////////////////////////////////////////////////////////////////
mbtcpDriver::~mbtcpDriver(){
    deleteSocket();
}
////////////////////////////////////////////////////////////////////////
bool mbtcpDriver::connectWithGateway(QString IPaddress){
    if(socket != nullptr){
        socket->disconnectFromHost();
        socket->deleteLater();
        socket = nullptr;
    }
    const QUrl url = QUrl::fromUserInput(IPaddress);
    if(url.isValid()){
        currentIPAddress = IPaddress;
        socket = new QTcpSocket();
        socket->connectToHost(url.host(), url.port());
        connect(socket,&QTcpSocket::connected,this,&mbtcpDriver::connectedSlot);
        connect(socket,&QTcpSocket::disconnected,this,&mbtcpDriver::disconnectedSlot);
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorSlot(QAbstractSocket::SocketError)));
        if(socket->waitForConnected(3000)){
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////////
void mbtcpDriver::disconnectFromGateway(){
    if(socket != nullptr){
        socket->disconnectFromHost();
    }
}
//////////////////////////////////////////////////////////////////////////////
QString mbtcpDriver::getDeviceInfo(unsigned char devAddress){
    QString rez;
    currentDevAddress = devAddress;

    if(socket->open(QIODevice::ReadWrite)){
        QByteArray array;
        array.append(static_cast<char>(transactionIdentifier>>8));//идентификатор транзакции
        array.append(static_cast<char>(transactionIdentifier));//идентификатор транзакции
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//длина посылки старший байт
        array.append(static_cast<char>(2));//длина посылки без первх 4-х байт
        array.append(currentDevAddress);//адрес устройства
        array.append(static_cast<char>(17));//функция
        socket->write(array);
        if(socket->waitForReadyRead()==true){
            int size = socket->bytesAvailable();
            QByteArray arr = socket->read(size);
            arr.remove(0,9);//удаляем заголовок ModbusTCP
            arr.push_back("\n");
            rez = QString::fromLatin1(arr);
            return rez;
        }
    }
    return rez;
}
//////////////////////////////////////////////////////////////////////////////////////
QByteArray mbtcpDriver::getDayData(unsigned char devAddress,unsigned char numberPP, QDate date){
    QByteArray rez;
    currentDevAddress = devAddress;
    if(socket->open(QIODevice::ReadWrite)){
        QByteArray array;
        array.append(static_cast<char>(transactionIdentifier>>8));//идентификатор транзакции
        array.append(static_cast<char>(transactionIdentifier));//идентификатор транзакции
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//длина посылки старший байт
        array.append(static_cast<char>(12));//длина посылки без первых 4-х байт
        array.append(currentDevAddress);//адрес устройства
        array.append(static_cast<char>(70));//функция
        array.append(static_cast<char>(9));//команда
        array.append(static_cast<char>(numberPP));//номер ПП
        array.append(static_cast<char>(0));//номер строки в суточном архиве
        array.append(static_cast<char>(0));//номер строки в часовом архиве
        array.append(static_cast<char>(24));//Час
        array.append(static_cast<char>(date.day()));//число
        array.append(static_cast<char>(date.month()));//месяц
        array.append(static_cast<char>(date.year()-2000));//год
        array.append(static_cast<char>(00));//пароль старший байт
        array.append(static_cast<char>(00));//пароль младший байт

        socket->write(array);
        if(socket->waitForReadyRead()==true){
            int size = socket->bytesAvailable();
            QByteArray rez = socket->read(size);
            rez.remove(0,9);//удаляем заголовок ModbusTCP
            return rez;
        }
    }
    return rez;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray mbtcpDriver::getDataForDay(unsigned char devAddress, unsigned char mode, unsigned char numberPP, QDate date){
    QByteArray rez;
    currentDevAddress = devAddress;
    if(socket->open(QIODevice::ReadWrite)){
        QByteArray array;
        array.append(static_cast<char>(transactionIdentifier>>8));//идентификатор транзакции
        array.append(static_cast<char>(transactionIdentifier));//идентификатор транзакции
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//длина посылки старший байт
        array.append(static_cast<char>(10));//длина посылки без первых 4-х байт
        array.append(currentDevAddress);//адрес устройства
        array.append(static_cast<char>(70));//функция
        array.append(static_cast<char>(1));//команда
        array.append(static_cast<char>(numberPP));//номер ПП
        array.append(static_cast<char>(mode));//режим
        array.append(static_cast<char>(date.day()));//число
        array.append(static_cast<char>(date.month()));//месяц
        array.append(static_cast<char>(date.year()-2000));//год
        array.append(static_cast<char>(00));//пароль старший байт
        array.append(static_cast<char>(00));//пароль младший байт

        socket->write(array);
        if(socket->waitForReadyRead()==true){
            int size = socket->bytesAvailable();
            QByteArray rez = socket->read(size);
            rez.remove(0,9);//удаляем заголовок ModbusTCP
            return rez;
        }
    }
    return rez;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray mbtcpDriver::getDeviceTime(unsigned char devAddress){
    QByteArray rez;
    currentDevAddress = devAddress;
    if(socket->open(QIODevice::ReadWrite)){
        QByteArray array;
        array.append(static_cast<char>(transactionIdentifier>>8));//идентификатор транзакции
        array.append(static_cast<char>(transactionIdentifier));//идентификатор транзакции
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//идентификатор протокола
        array.append(static_cast<char>(0));//длина посылки старший байт
        array.append(static_cast<char>(6));//длина посылки без первых 4-х байт
        array.append(currentDevAddress);//адрес устройства
        array.append(static_cast<char>(3));//функция
        array.append(static_cast<char>(0));//старший байт адреса
        array.append(static_cast<char>(0x0086));//младший байт адреса
        array.append(static_cast<char>(0));//старший байт количества слов
        array.append(static_cast<char>(3));//младший байт количества слов

        socket->write(array);
        if(socket->waitForReadyRead()==true){
            int size = socket->bytesAvailable();
            rez = socket->read(size);
            rez.remove(0,9);//удаляем заголовок ModbusTCP
            return rez;
        }
    }
    return rez;
}
/////////////////////////////////////////////////////////////////////////////////////////
bool mbtcpDriver::isConnected(){
    if(socket != nullptr){
        if(socket->state() == QAbstractSocket::ConnectedState){
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////
void mbtcpDriver::deleteSocket(){
    if(socket != nullptr){
        socket->deleteLater();
        socket = nullptr;
    }
}
/////////////////////////////////////////////////////////////////////////////
bool mbtcpDriver::socketReady(){
    if(socket != nullptr){
        if(socket->state() == QAbstractSocket::ConnectedState){
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////
void mbtcpDriver::connectedSlot(){
    emit connectedSignal();
}
/////////////////////////////////////////////////////////////////////////
void mbtcpDriver::disconnectedSlot(){
    emit disconnectedSignal();
}
/////////////////////////////////////////////////////////////////////////////
void mbtcpDriver::errorSlot(QAbstractSocket::SocketError socketError){
    emit errorSignal(socketError);
}
/////////////////////////////////////////////////////////////////////////
