#include "irvistime.h"

IRVISTime::IRVISTime(){
    enable = false;
}
////////////////////////////////////////////////////////
bool IRVISTime::isEnable(){
    return enable;
}
///////////////////////////////////////////////////////////
void IRVISTime::setEnable(bool flag){
    enable = flag;
}
/////////////////////////////////////////////////////////////
void IRVISTime::changeTime(QTime time){
    this->setHMS(time.hour(),time.minute(),time.second());
}
/////////////////////////////////////////////////////////////////
void IRVISTime::serialize(QDataStream *str){
    *str << enable;
    *str << this->hour() << this->minute();
    *str << comment;
    *str << channel1;
    *str << channel2;
    *str << channel3;
    *str << channel4;
}
///////////////////////////////////////////////////////////////////////
bool IRVISTime::deserialise(QDataStream *str){
    *str >> enable;
    int hour = 0;
    int min = 0;
    *str >> hour;
    *str >> min;
    *str >> comment;
    *str >> channel1;
    *str >> channel2;
    *str >> channel3;
    *str >> channel4;
    if(comment.size() > 300){
        return false;
    }
    if(enable){
        if((hour >= 0) && (hour < 24)){
            if((min >= 0) && (min < 60)){
                this->setHMS(hour,min,0);
                return true;
            }
        }
        return false;
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////
QString IRVISTime::getComment() const{
    return comment;
}
/////////////////////////////////////////////////////////////////////////
void IRVISTime::setComment(const QString &value){
    comment = value;
}
/////////////////////////////////////////////////////////////////////////////
void IRVISTime::setChannel(int index, bool state){
    switch(index){
        case(1):{
            channel1 = state;
            break;
        }
        case(2):{
            channel2 = state;
            break;
        }
        case(3):{
            channel3 = state;
            break;
        }
        case(4):{
            channel4 = state;
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
bool IRVISTime::getChannel(int index){
    switch(index){
        case(1):{
            return channel1;
        }
        case(2):{
            return channel2;
        }
        case(3):{
            return channel3;
        }
        case(4):{
            return channel4;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////
