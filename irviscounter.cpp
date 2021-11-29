#include "irviscounter.h"

IRVISCounter::IRVISCounter(){
    devAddress = 0;
    password = 0;
    connect(&this->timer,SIGNAL(timeout()),this,SLOT(timerSlot()));
    interval = 500;//интервал проверки времени
    connect(&tcpDriver,&mbtcpDriver::connectedSignal,this,&IRVISCounter::connectedSlot);
    connect(&tcpDriver,&mbtcpDriver::disconnectedSignal,this,&IRVISCounter::disconnectedSlot);
    connect(&tcpDriver,&mbtcpDriver::errorSignal,this,&IRVISCounter::errorSlot);
}
/////////////////////////////////////////////////////////////////////////
bool IRVISCounter::isConnected(){
    if(tcpDriver.isConnected()){
        return true;
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////
bool IRVISCounter::isWork(){
    return timer.isActive();
}
///////////////////////////////////////////////////////////////////////////////
void IRVISCounter::stopCounter(){
    timer.stop();
}
//////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::startCounter(){
    timer.start(interval);
    devData = tcpDriver.getDeviceInfo(devAddress);
    QByteArray arr = tcpDriver.getDayData(devAddress,1,QDate::currentDate().addDays(-1));

    QDateTime dt = readDevDateTime();
    //получаем рассчетный час
    if(arr.size() != 0){
        if(arr.at(0) == 1){//сверяем номер ПП
            if(arr.at(2) != 0){
                unsigned char hour = arr.at(59);
                deviceCalcTime = QTime(static_cast<int>(hour),0,0,0);//рассчетный час
                return true;
            }
        }
    }
    emit errorSignal(true,"Счетчик " + QString::number(devAddress) + ". Не удалось подключиться к счетчику.");
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::timerSlot(){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDateTime tmp = QDateTime::currentDateTime();

    static bool readed1 = false;//флаги для однократного чтения
    static bool readed2 = false;//флаги для однократного чтения
    static bool readed3 = false;//флаги для однократного чтения

    if(time1.isEnable()){
        if((time1.hour() == currentDateTime.time().hour()) && (time1.minute() == currentDateTime.time().minute())){//время пришло
            if(!readed1){
                connectFromHost();//соединяемся со шлюзом...........................................................................
                currentDevDateTime = readDevDateTime();
                tmp.setTime(static_cast<QTime>(time1Origin));
                if(tmp > currentDevDateTime){//если часы на счетчике отстают
                    qint64 devDateTime = currentDevDateTime.toMSecsSinceEpoch();
                    qint64 dateTime = currentDateTime.toMSecsSinceEpoch();
                    time1.changeTime(time1.addMSecs((dateTime - devDateTime) + 60000));
                    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Автоматически изменено время снятия показаний №1 на " +
                                     time1.toString("hh:mm"));
                    emit timeChanged();
                    return;
                }
                readDevHoursData();
                disconnectFromHost();//отсоединяемся от шлюза.......................................................................
                readed1 = true;

                //поиск в отчетах часовых блоков
                int size = blocksArray.size();
                int index1 = -1;
                int index2 = -1;
                for(int n = 0;n != size;n++){
                    if(blocksArray.at(n).dateTime.time().hour() == time1.hour()){
                        if(index1 == -1){
                            index1 = n;
                        }
                        else if(index2 == -1){
                            index2 = n;
                            break;
                        }
                    }
                }
                if((index1 == -1) || (index2 == -1)){
                    emit errorSignal(true,"Ошибка при поиске часовой записи в выгруженном архиве.");
                    return;
                }
                emit dataReadySignal(blocksArray.at(index1),blocksArray.at(index2),time1.getComment());
                blocksArray.clear();
            }
        }
        else{
            readed1 = false;
        }
    }
    if(time2.isEnable()){
        if((time2.hour() == currentDateTime.time().hour()) && (time2.minute() == currentDateTime.time().minute())){//время пришло
            if(!readed2){
                connectFromHost();//соединяемся со шлюзом....................................................................
                currentDevDateTime = readDevDateTime();
                tmp.setTime(static_cast<QTime>(time2Origin));
                if(tmp > currentDevDateTime){//если часы на счетчике отстают
                    qint64 devDateTime = currentDevDateTime.toMSecsSinceEpoch();
                    qint64 dateTime = currentDateTime.toMSecsSinceEpoch();
                    time2.changeTime(time2.addMSecs((dateTime - devDateTime) + 60000));
                    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Автоматически изменено время снятия показаний №2 на " +
                                     time2.toString("hh:mm"));
                    emit timeChanged();
                    return;
                }
                readDevHoursData();
                disconnectFromHost();//отсоединяемся от шлюза.......................................................................
                readed2 = true;

                //поиск в отчетах часовых блоков
                int size = blocksArray.size();
                int index1 = -1;
                int index2 = -1;
                for(int n = 0;n != size;n++){
                    if(blocksArray.at(n).dateTime.time().hour() == time2.hour()){
                        if(index1 == -1){
                            index1 = n;
                        }
                        else if(index2 == -1){
                            index2 = n;
                            break;
                        }
                    }
                }
                if((index1 == -1) || (index2 == -1)){
                    emit errorSignal(true,"Ошибка при поиске часовой записи в выгруженном архиве.");
                    return;
                }
                emit dataReadySignal(blocksArray.at(index1),blocksArray.at(index2),time2.getComment());
                blocksArray.clear();
            }
        }
        else{
            readed2 = false;
        }
    }
    if(time3.isEnable()){
        if((time3.hour() == currentDateTime.time().hour()) && (time3.minute() == currentDateTime.time().minute())){//время пришло
            if(!readed3){
                connectFromHost();//соединяемся со шлюзом....................................................................
                currentDevDateTime = readDevDateTime();
                tmp.setTime(static_cast<QTime>(time3Origin));
                if(tmp > currentDevDateTime){//если часы на счетчике отстают
                    qint64 devDateTime = currentDevDateTime.toMSecsSinceEpoch();
                    qint64 dateTime = currentDateTime.toMSecsSinceEpoch();
                    time3.changeTime(time3.addMSecs((dateTime - devDateTime) + 60000));
                    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Автоматически изменено время снятия показаний №3 на " +
                                     time3.toString("hh:mm"));
                    emit timeChanged();
                    return;
                }
                readDevHoursData();
                disconnectFromHost();//отсоединяемся от шлюза.......................................................................
                readed3 = true;

                //поиск в отчетах часовых блоков
                int size = blocksArray.size();
                int index1 = -1;
                int index2 = -1;
                for(int n = 0;n != size;n++){
                    if(blocksArray.at(n).dateTime.time().hour() == time3.hour()){
                        if(index1 == -1){
                            index1 = n;
                        }
                        else if(index2 == -1){
                            index2 = n;
                            break;
                        }
                    }
                }
                if((index1 == -1) || (index2 == -1)){
                    emit errorSignal(true,"Ошибка при поиске часовой записи в выгруженном архиве.");
                    return;
                }
                emit dataReadySignal(blocksArray.at(index1),blocksArray.at(index2),time3.getComment());
                blocksArray.clear();
            }
        }
        else{
            readed3 = false;
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::addBlocks(QByteArray *array){
    int blockOffset = 3;
    unsigned char blocksCount = array->at(2);
    if(!array->isEmpty()){
        if(blocksCount != 0){
            for(int n=0;n!=blocksCount;n++){
                blockOffset += 33*n;//сдвигаем на смещение следующего блока
                dayDataBlock block;
                block.numberOfPost = array->at(1);//номер посылки
                QDateTime dateTime;
                QDate date;
                QTime time;
                time.setHMS(static_cast<int>(array->at(blockOffset+1)),     //часы
                            static_cast<int>(array->at(blockOffset+0)),0);  //минуты
                dateTime.setTime(time);
                date.setDate(static_cast<int>(array->at(blockOffset+4)+2000),    //год
                             static_cast<int>(array->at(blockOffset+3)),    //месяц
                             static_cast<int>(array->at(blockOffset+2)));   //день
                dateTime.setDate(date);
                block.dateTime = dateTime;//дата время
                memcpy(&block.workTime,array->data()+blockOffset+7,2);//часы наработки
                memcpy(&block.Vnu,array->data()+blockOffset+9,4);//Vну
                memcpy(&block.Vrab,array->data()+blockOffset+13,4);//Vраб
                memcpy(&block.Qnu,array->data()+blockOffset+17,4);//Qну
                memcpy(&block.Qrab,array->data()+blockOffset+21,4);//Qраб
                memcpy(&block.P,array->data()+blockOffset+25,4);//P
                memcpy(&block.T,array->data()+blockOffset+29,4);//T
                blocksArray.append(block);
            }
            return true;
        }
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::readDevHoursData(){
    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Чтение часовых данных.");//не ошибка. просто для записи в лог

    QByteArray arr;

    //читаем позавчерашний день
    arr = tcpDriver.getDataForDay(devAddress,0,1,QDate::currentDate().addDays(-2));//первый запрос в режиме 0
    if(addBlocks(&arr)){//если первый блок прочитался
        do{
            arr = tcpDriver.getDataForDay(devAddress,1,1,QDate::currentDate().addDays(-2));//последующие запросы в режиме 1
        }while(addBlocks(&arr));//продолжаем если блок существует и добавился
    }

    //читаем вчерашний день
    arr = tcpDriver.getDataForDay(devAddress,0,1,QDate::currentDate().addDays(-1));//первый запрос в режиме 0
    if(addBlocks(&arr)){//если первый блок прочитался
        do{
            arr = tcpDriver.getDataForDay(devAddress,1,1,QDate::currentDate().addDays(-1));//последующие запросы в режиме 1
        }while(addBlocks(&arr));//продолжаем если блок существует и добавился
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::readDevDayData(){


}
////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime IRVISCounter::readDevDateTime(){
    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Чтение даты и времени.");
    QDateTime deviceTime;
    QByteArray rez = tcpDriver.getDeviceTime(devAddress);//получаем время устройства. Время на счетчике может уходить.
    if(rez.size() == 6){
        unsigned char tmp = 0;
        int min = 0;
        int hour = 0;
        int day = 0;
        int month = 0;
        int year = 0;

        tmp = static_cast<unsigned char>(rez[0]);
        char tmp1 = tmp/16;
        min = static_cast<int>(tmp1 * 10);
        min += static_cast<int>(tmp - (tmp1 * 16));

        tmp = static_cast<char>(rez[3]);
        tmp1 = tmp/16;
        hour = static_cast<int>(tmp1 * 10);
        hour += static_cast<int>(tmp - (tmp1 * 16));

        tmp = static_cast<char>(rez[2]);
        tmp1 = tmp/16;
        day = static_cast<int>(tmp1 * 10);
        day += static_cast<int>(tmp - (tmp1 * 16));

        tmp = static_cast<char>(rez[5]);
        tmp1 = tmp/16;
        month = static_cast<int>(tmp1 * 10);
        month += static_cast<int>(tmp - (tmp1 * 16));

        tmp = static_cast<char>(rez[4]);
        tmp1 = tmp/16;
        year = static_cast<int>(tmp1 * 10);
        year += static_cast<int>(tmp - (tmp1 * 16));

        deviceTime.setTime(QTime(hour,min,0));
        deviceTime.setDate(QDate(year+2000,month,day));
        return deviceTime;
    }
    emit errorSignal(true,"Счетчик " + QString::number(devAddress) + ". Не удалось получить время счетчика по адресу " + QString::number(devAddress));
    return deviceTime;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::serialise(QDataStream *str){
    *str << IPAddress;
    *str << devAddress;
    *str << password;

    time1.serialize(str);
    time2.serialize(str);
    time3.serialize(str);

    *str << time1Origin;
    *str << time2Origin;
    *str << time3Origin;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::deserialise(QDataStream *str){

    *str >> IPAddress;
    QUrl url = QUrl::fromUserInput(IPAddress);
    url.fromUserInput(IPAddress);
    if(url.isValid()){
        *str >> devAddress;
        *str >> password;
        if(time1.deserialise(str)){
            if(time2.deserialise(str)){
                if(time3.deserialise(str)){
                    *str >> time1Origin;
                    *str >> time2Origin;
                    *str >> time3Origin;
                    return true;
                }
            }
        }
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::connectFromHost(){
    tcpDriver.connectWithGateway(IPAddress);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::disconnectFromHost(){
    tcpDriver.disconnectFromGateway();
}
//////////////////////////////////////////////////////////////////////////////////////////////
QString IRVISCounter::getTimeComment(int index){
    switch(index){
        case(1):{
            return time1.getComment();
            break;
        }
        case(2):{
            return time2.getComment();
            break;
        }
        case(3):{
            return time3.getComment();
            break;
        }
    }
    return QString();
}
////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::setTimeComment(int index, QString comment){
    switch(index){
        case(1):{
            time1.setComment(comment);
            break;
        }
        case(2):{
            time2.setComment(comment);
            break;
        }
        case(3):{
            time3.setComment(comment);
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::setChannel(int time, int channel, bool state){
    switch (time){
        case(1):{
            time1.setChannel(channel,state);
            break;
        }
        case(2):{
            time2.setChannel(channel,state);
            break;
        }
        case(3):{
            time3.setChannel(channel,state);
            break;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::getChannel(int time, int channel){
    switch (time){
        case(1):{
            return time1.getChannel(channel);
        }
        case(2):{
            return time2.getChannel(channel);
        }
        case(3):{
            return time3.getChannel(channel);
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////
QString IRVISCounter::getDevData() const{
    return devData;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::disableTime(int index){
    switch(index){
        case(1):{
            time1.setEnable(false);
            break;
        }
        case(2):{
            time2.setEnable(false);
            break;
        }
        case(3):{
            time3.setEnable(false);
            break;
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
int IRVISCounter::getPassword() const{
    return password;
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::setPassword(QString pass){
    bool ok = false;
    password = static_cast<quint16>(pass.toInt(&ok));
    if(!ok){
        return false;
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
unsigned char IRVISCounter::getDevAddress() const{
    return devAddress;
}
//////////////////////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::setDevAddress(int value){
    if((value < 0) || (value > 255)){
        return false;
    }
    devAddress = static_cast<unsigned char>(value);
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
QTime IRVISCounter::getDeviceCalcTime() const{
    return deviceCalcTime;
}
//////////////////////////////////////////////////////////////////////////////////////////////
IRVISTime IRVISCounter::getTime(int index) const{
    switch(index){
        case(1):{
            return time1;
            break;
        }
        case(2):{
            return time2;
            break;
        }
        case(3):{
            return time3;
            break;
        }
        default:{
            IRVISTime time;
            return time;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::setTime(int index, const QTime &value){
    IRVISTime time;
    time.changeTime(value);
    switch(index){
        case(1):{
            time1 = time;
            time1.setEnable(true);
            time1Origin = static_cast<QTime>(time1);
            break;
        }
        case(2):{
            time2 = time;
            time2.setEnable(true);
            time2Origin = static_cast<QTime>(time2);
            break;
        }
        case(3):{
            time3 = time;
            time3.setEnable(true);
            time3Origin = static_cast<QTime>(time3);
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////
QTime IRVISCounter::getOriginTime(int index) const{
    switch(index){
        case(1):{
            return time1Origin;
            break;
        }
        case(2):{
            return time2Origin;
            break;
        }
        case(3):{
            return time3Origin;
            break;
        }
        default:{
            QTime time;
            return time;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////
QString IRVISCounter::getIPAddress() const{
    return IPAddress;
}
/////////////////////////////////////////////////////////////////////////////////////
bool IRVISCounter::setIPAddress(const QString &value){
    QUrl url = QUrl::fromUserInput(value);
    if(!url.isValid()){
        return false;
    }
    IPAddress = value;
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::connectedSlot(){
    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Соединение со шлюзом.");//это не ошибка. Просто для записи в лог
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::disconnectedSlot(){
    emit errorSignal(false,"Счетчик " + QString::number(devAddress) + ". Отсоединение от шлюза.");//это не ошибка. Просто для записи в лог
}
////////////////////////////////////////////////////////////////////////////////////////////////
void IRVISCounter::errorSlot(QAbstractSocket::SocketError error){
    switch(error){
        case(QAbstractSocket::RemoteHostClosedError):{
            emit errorSignal(true,"Счетчик " + QString::number(devAddress) + ". Удаленный хост разорвал соединение.");
            break;
        }
        case(QAbstractSocket::NetworkError):{
            emit errorSignal(true,"Счетчик " + QString::number(devAddress) + ". Удаленный хост разорвал соединение.");
            break;
        }
    }
}
