#ifndef IRVISTIME_H
#define IRVISTIME_H

#include <QTime>
#include <QDataStream>

class IRVISTime : public QTime
{
public:
    IRVISTime();
    bool isEnable();
    void setEnable(bool flag);
    void changeTime(QTime time);
    void serialize(QDataStream *str);
    bool deserialise(QDataStream *str);
    QString getComment() const;
    void setComment(const QString &value);
    void setChannel(int index,bool state);
    bool getChannel(int index);

protected:
    bool enable;
    QString comment;
    bool channel1;
    bool channel2;
    bool channel3;
    bool channel4;

};

#endif // IRVISTIME_H
