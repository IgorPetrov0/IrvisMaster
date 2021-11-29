#include "tcpserver.h"

TCPServer::TCPServer(QObject *parent):
    QTcpServer(parent)
{
    socket = nullptr;
    this->setMaxPendingConnections(1);//обслуживаем только одно соединение безопасности для
    connect(this,&TCPServer::newConnection,this,&TCPServer::newConnectionSlot);

}
////////////////////////////////////////////////////////////////////
TCPServer::~TCPServer(){
    deleteSocket();
}
////////////////////////////////////////////////////////////////////////
void TCPServer::deleteSocket(){
    if(socket != nullptr){
        socket->deleteLater();
        socket = nullptr;
    }
}
//////////////////////////////////////////////////////////////////////////
void TCPServer::newConnectionSlot(){
    deleteSocket();
    socket = this->nextPendingConnection();
    connect(socket,&QTcpSocket::readyRead,this,&TCPServer::dataReadySlot);
    connect(socket,&QTcpSocket::disconnected,this,&TCPServer::disconnectSlot);
}
////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::dataReadySlot(){
    QByteArray dataArray;
    int size = 0;
    uint16_t transIdentifier = 0;
    uint16_t protIdentifier = 0;
    uint16_t packetSize = 0;
    unsigned char devAddress = 0;

    if(socket != nullptr){
        size = socket->bytesAvailable();
        dataArray = socket->read(size);
    }



}
/////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::disconnectSlot(){
    disconnect(socket,&QTcpSocket::readyRead,this,&TCPServer::dataReadySlot);
    disconnect(socket,&QTcpSocket::disconnected,this,&TCPServer::disconnectSlot);
    deleteSocket();
}
