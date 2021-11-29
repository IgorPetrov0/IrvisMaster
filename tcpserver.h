#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>



class TCPServer : public QTcpServer
{
    Q_OBJECT

public:
    TCPServer(QObject *parent);
    ~TCPServer();

private:
    void deleteSocket();

protected slots:
    void newConnectionSlot();
    void dataReadySlot();
    void disconnectSlot();



protected:
    QTcpSocket *socket;

};

#endif // TCPSERVER_H
