#ifndef DAYDATABLOCK_H
#define DAYDATABLOCK_H

#include <QObject>
#include <QDateTime>

struct dayDataBlock{
    QDateTime dateTime;
    quint16 workTime;
    quint32 Vnu;
    quint32 Vrab;
    quint32 Qnu;
    quint32 Qrab;
    float P;
    float T;
    unsigned char numberOfPost;//номер посылки в которой пришел блок. Пока пусть будет
};

#endif // DAYDATABLOCK_H
