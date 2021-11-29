#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    appName = "ГАЗ клиент. Тутаевская ПГУ.";
    this->setWindowTitle(appName);
    this->setWindowIcon(QIcon(QApplication::applicationDirPath()+"\\gas.jpg"));


    server = new TCPServer(this);
    if(!server->listen(QHostAddress::LocalHost,1234)){
        message("Не удалось запустить сервер.");
    }


    ui->hostEdit_1->setText(QLatin1Literal("192.9.204.95:502"));
    ui->passEdit_1->setText("0000");

    connect(ui->turnOnButton_1,SIGNAL(clicked()),this,SLOT(turnOnSlot()));
    connect(ui->turnOnButton_2,SIGNAL(clicked()),this,SLOT(turnOnSlot()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(changeTabSlot(int)));
    connect(ui->timeRadioButton_1_1,&QRadioButton::toggled,this,&MainWindow::timeRadioButtonSlot_1);
    connect(ui->timeRadioButton_1_2,&QRadioButton::toggled,this,&MainWindow::timeRadioButtonSlot_1);
    connect(ui->timeRadioButton_2_1,&QRadioButton::toggled,this,&MainWindow::timeRadioButtonSlot_2);
    connect(ui->timeRadioButton_2_2,&QRadioButton::toggled,this,&MainWindow::timeRadioButtonSlot_2);
    connect(ui->timeRadioButton_3_1,&QRadioButton::toggled,this,&MainWindow::timeRadioButtonSlot_3);
    connect(ui->timeRadioButton_3_2,&QRadioButton::toggled,this,&MainWindow::timeRadioButtonSlot_3);
    connect(ui->changeButton_1,&QPushButton::clicked,this,&MainWindow::setCounterTimeSlot);
    connect(ui->changeButton_2,&QPushButton::clicked,this,&MainWindow::setCounterTimeSlot);
    connect(ui->changePassButton,&QPushButton::clicked,this,&MainWindow::changePassSlot);
    connect(ui->commentButton_1_1,&QPushButton::clicked,this,&MainWindow::timeCommentSlot_1);
    connect(ui->commentButton_2_1,&QPushButton::clicked,this,&MainWindow::timeCommentSlot_2);
    connect(ui->commentButton_3_1,&QPushButton::clicked,this,&MainWindow::timeCommentSlot_3);
    connect(ui->commentButton_1_2,&QPushButton::clicked,this,&MainWindow::timeCommentSlot_1);
    connect(ui->commentButton_2_2,&QPushButton::clicked,this,&MainWindow::timeCommentSlot_2);
    connect(ui->commentButton_3_2,&QPushButton::clicked,this,&MainWindow::timeCommentSlot_3);


    //указатели на слоты
    void (MainWindow::*showSlot)();
    void (MainWindow::*closeSlot)();
    showSlot = &MainWindow::tMenuShowSlot;
    closeSlot = &MainWindow::tMenuCloseSlot;

    trayMenu.addAction("Показать главное окно",this,showSlot);
    trayMenu.addSeparator();
    trayMenu.addAction("Закрыть программу",this,closeSlot);
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setToolTip(appName + "\n Программа для формирования \n "
                                  "суточных отчетов по газу. \n"
                                  "Не закрывать!!!!!!");
    trayIcon.setIcon(QIcon(QApplication::applicationDirPath()+"\\gas.jpg"));
    connect(&trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(tIconActivated(QSystemTrayIcon::ActivationReason)));


    trayIcon.show();

    counterCom = new IRVISCounter;
    counterTex = new IRVISCounter;
    currentCounter = counterCom;
    connect(counterCom,SIGNAL(timeChanged()),this,SLOT(updateCounterTime()));
    connect(counterTex,SIGNAL(timeChanged()),this,SLOT(updateCounterTime()));
    connect(counterCom,SIGNAL(errorSignal(bool, QString)),this,SLOT(errorSlot(bool, QString)));
    connect(counterTex,SIGNAL(errorSignal(bool, QString)),this,SLOT(errorSlot(bool, QString)));
    connect(counterCom,SIGNAL(dataReadySignal(dayDataBlock,dayDataBlock,QString)),this,SLOT(dataReadySlot(dayDataBlock,dayDataBlock,QString)));
    connect(counterTex,SIGNAL(dataReadySignal(dayDataBlock,dayDataBlock,QString)),this,SLOT(dataReadySlot(dayDataBlock,dayDataBlock,QString)));

    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(0);
    writeLog("Запуск программы");

    if(readConfigFile()){
        writeLog("Чтение конфигурации успешно.");
    }
    else{
        errorSlot(false,"Ошибка чтения конфигурационного файла при запуске.");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow(){
    trayMenu.hide();
    delete server;
    delete counterCom;
    delete counterTex;
    delete ui;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::writeLog(QString string){
    QString appPath = QApplication::applicationDirPath();
    QFile file(appPath+"\\"+"gasClient.log");
    if(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)){
        QTextStream str(&file);
        str<<QDateTime::currentDateTime().toString("dd,MM,yyyy hh:mm:ss    -   ") + string + "\n";
        file.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::writeDataInFile(QString dataString){
    QString appPath = QApplication::applicationDirPath();
    QFile file(appPath+"\\"+"gasClientReport.log");
    if(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)){
        QTextStream str(&file);
        str<<QDateTime::currentDateTime().toString("dd,MM,yyyy hh:mm:ss    -   ") + dataString + "\n";
        file.close();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *event){
    if(writeConfigFile()){
        writeLog("Сохранение конфигурационного файла.");
    }
    else{
        errorSlot(true,"Ошибка сохранения конфигурационного файла");
    }
    this->hide();
    event->ignore();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::message(QString message){
    QMessageBox box;
    box.setIcon(QMessageBox::Information);
    box.setWindowTitle(appName);
    box.setWindowIcon(QIcon(QApplication::applicationDirPath()+"\\gas.jpg"));
    box.setText(message);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool MainWindow::readConfigFile(){
    QString appPath = QApplication::applicationDirPath();
    QFile file(appPath+"\\"+"gasClient.conf");
    if(file.open(QIODevice::ReadOnly)){
        QDataStream str(&file);
        if(counterCom->deserialise(&str)){
            str >> checkSumm;
            if(counterTex->deserialise(&str)){
                ui->hostEdit_1->setText(counterCom->getIPAddress());
                ui->passEdit_1->setText(QString::number(counterCom->getPassword()));
                ui->devAddressEdit_1->setValue(counterCom->getDevAddress());
                ui->hostEdit_2->setText(counterTex->getIPAddress());
                ui->passEdit_2->setText(QString::number(counterTex->getPassword()));
                ui->devAddressEdit_2->setValue(counterTex->getDevAddress());
                updateCounterTime();
                file.close();
                return true;
            }
        }
        file.close();
        errorSlot(true,"Ошибка при чтении файла конфигурации.");
        return false;
    }
    errorSlot(true,"Невозможно открыть файл конфигурации.");
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MainWindow::writeConfigFile(){
    QString appPath = QApplication::applicationDirPath();
    QFile file(appPath+"\\"+"gasClient.conf");
    if(file.open(QIODevice::WriteOnly)){
        QDataStream str(&file);
        counterCom->serialise(&str);
        str << checkSumm;
        counterTex->serialise(&str);
        file.close();
        return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::tIconActivated(QSystemTrayIcon::ActivationReason reason){
    if(reason == QSystemTrayIcon::DoubleClick){
        this->show();
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::tMenuShowSlot(){
    this->show();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::tMenuCloseSlot(){
    writeLog("Останов программы");
    if(writeConfigFile()){
        writeLog("Сохранение конфигурационного файла.");
    }
    else{
        errorSlot(true,"Ошибка сохранения конфигурационного файла");
    }
    QApplication::quit();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::changeTabSlot(int index){
    switch(index){
        case(0):{
            currentCounter = counterCom;
            break;
        }
        case(1):{
            currentCounter = counterTex;
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::turnOnSlot(){

    this->setCursor(Qt::WaitCursor);
    writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Нажатие кнопки <Старт/Стоп>.");
    switch(ui->tabWidget->currentIndex()){
        case(0):{
            if(!counterCom->isWork()){
                if(!counterCom->setIPAddress(ui->hostEdit_1->text())){
                    errorSlot(true,"Неверный IP адрес!");
                    return;
                }
                if(!counterCom->setDevAddress(ui->devAddressEdit_1->value())){
                    errorSlot(true,"Неверный адрес счетчика!");
                    return;
                }
                if(!counterCom->setPassword(ui->passEdit_1->text())){
                    errorSlot(true,"Неверный пароль!");
                    return;
                }
                counterCom->connectFromHost();//соединяемся со шлюзом........................................................
                if(counterCom->startCounter()){
                    //получаем общие данные
                    testDialog dialog(this);
                    dialog.setInfo(counterCom->getDevData());
                    dialog.setCalcTime(counterCom->getDeviceCalcTime().toString("hh:mm"));
                    dialog.setDevTime(counterCom->readDevDateTime().toString("dd.MM.yyyy hh:mm"));
                    counterCom->disconnectFromHost();//отсоединяемся от шлюза................................................
                    dialog.setWindowTitle(appName + " Общие данные.");
                    dialog.exec();
                    ui->timeRadioButton_1_1->setEnabled(true);
                    ui->timeRadioButton_2_1->setEnabled(true);
                    ui->timeRadioButton_3_1->setEnabled(true);
                    ui->channelCheckBox_1_1_1->setEnabled(true);
                    ui->channelCheckBox_2_1_1->setEnabled(true);
                    ui->channelCheckBox_3_1_1->setEnabled(true);
                    ui->channelCheckBox_4_1_1->setEnabled(true);
                    ui->channelCheckBox_1_2_1->setEnabled(true);
                    ui->channelCheckBox_2_2_1->setEnabled(true);
                    ui->channelCheckBox_3_2_1->setEnabled(true);
                    ui->channelCheckBox_4_2_1->setEnabled(true);
                    ui->channelCheckBox_1_3_1->setEnabled(true);
                    ui->channelCheckBox_2_3_1->setEnabled(true);
                    ui->channelCheckBox_3_3_1->setEnabled(true);
                    ui->channelCheckBox_4_3_1->setEnabled(true);
                    ui->changeButton_1->setEnabled(true);
                    ui->turnOnButton_1->setText("Выключить");
                    updateCounterTime();
                }
            }
            else{
                counterCom->stopCounter();
                ui->turnOnButton_1->setText("Включить");
                ui->timeRadioButton_1_1->setDisabled(true);
                ui->timeRadioButton_2_1->setDisabled(true);
                ui->timeRadioButton_3_1->setDisabled(true);
                ui->channelCheckBox_1_1_1->setDisabled(true);
                ui->channelCheckBox_2_1_1->setDisabled(true);
                ui->channelCheckBox_3_1_1->setDisabled(true);
                ui->channelCheckBox_4_1_1->setDisabled(true);
                ui->channelCheckBox_1_2_1->setDisabled(true);
                ui->channelCheckBox_2_2_1->setDisabled(true);
                ui->channelCheckBox_3_2_1->setDisabled(true);
                ui->channelCheckBox_4_2_1->setDisabled(true);
                ui->channelCheckBox_1_3_1->setDisabled(true);
                ui->channelCheckBox_2_3_1->setDisabled(true);
                ui->channelCheckBox_3_3_1->setDisabled(true);
                ui->channelCheckBox_4_3_1->setDisabled(true);
                ui->changeButton_1->setDisabled(true);
            }
            break;
        }
        case(1):{
            if(!counterTex->isWork()){
                if(!counterTex->setIPAddress(ui->hostEdit_2->text())){
                    errorSlot(true,"Неверный IP адрес!");
                    return;
                }
                if(!counterTex->setDevAddress(ui->devAddressEdit_2->value())){
                    errorSlot(true,"Неверный адрес счетчика!");
                    return;
                }
                if(!counterTex->setPassword(ui->passEdit_2->text())){
                    errorSlot(true,"Неверный пароль!");
                    return;
                }
                counterTex->connectFromHost();//соединяемся со шлюзом.......................................
                if(counterTex->startCounter()){
                    //получаем общие данные
                    testDialog dialog(this);
                    dialog.setInfo(counterTex->getDevData());
                    dialog.setCalcTime(counterTex->getDeviceCalcTime().toString("hh:mm"));
                    dialog.setDevTime(counterTex->readDevDateTime().toString("dd.MM.yyyy hh:mm"));
                    counterTex->disconnectFromHost();//отсоединяемся от шлюза...................................
                    dialog.setWindowTitle(appName + " Общие данные.");
                    dialog.exec();
                    ui->timeRadioButton_1_2->setEnabled(true);
                    ui->timeRadioButton_2_2->setEnabled(true);
                    ui->timeRadioButton_3_2->setEnabled(true);
                    ui->channelCheckBox_1_1_2->setEnabled(true);
                    ui->channelCheckBox_2_1_2->setEnabled(true);
                    ui->channelCheckBox_3_1_2->setEnabled(true);
                    ui->channelCheckBox_4_1_2->setEnabled(true);
                    ui->channelCheckBox_1_2_2->setEnabled(true);
                    ui->channelCheckBox_2_2_2->setEnabled(true);
                    ui->channelCheckBox_3_2_2->setEnabled(true);
                    ui->channelCheckBox_4_2_2->setEnabled(true);
                    ui->channelCheckBox_1_3_2->setEnabled(true);
                    ui->channelCheckBox_2_3_2->setEnabled(true);
                    ui->channelCheckBox_3_3_2->setEnabled(true);
                    ui->channelCheckBox_4_3_2->setEnabled(true);
                    ui->changeButton_2->setEnabled(true);
                    ui->turnOnButton_1->setText("Выключить");

                }
            }
            else{
                counterTex->stopCounter();
                ui->turnOnButton_2->setText("Включить");
                ui->timeRadioButton_1_2->setDisabled(true);
                ui->timeRadioButton_2_2->setDisabled(true);
                ui->timeRadioButton_3_2->setDisabled(true);
                ui->channelCheckBox_1_1_2->setDisabled(true);
                ui->channelCheckBox_2_1_2->setDisabled(true);
                ui->channelCheckBox_3_1_2->setDisabled(true);
                ui->channelCheckBox_4_1_2->setDisabled(true);
                ui->channelCheckBox_1_2_2->setDisabled(true);
                ui->channelCheckBox_2_2_2->setDisabled(true);
                ui->channelCheckBox_3_2_2->setDisabled(true);
                ui->channelCheckBox_4_2_2->setDisabled(true);
                ui->channelCheckBox_1_3_2->setDisabled(true);
                ui->channelCheckBox_2_3_2->setDisabled(true);
                ui->channelCheckBox_3_3_2->setDisabled(true);
                ui->channelCheckBox_4_3_2->setDisabled(true);
                ui->changeButton_2->setDisabled(true);
            }
            break;
        }
    }
    this->setCursor(Qt::ArrowCursor);
}
//////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timeRadioButtonSlot_1(bool checked){
    switch(ui->tabWidget->currentIndex()){
        case(0):{
            if(checked){
                if(counterCom->isWork()){
                    ui->timeEdit_1_1->setEnabled(true);
                }
            }
            else{
                ui->timeEdit_1_1->setDisabled(true);
            }
            break;
        }
        case(1):{
            if(checked){
                if(counterTex->isWork()){
                    ui->timeEdit_1_2->setEnabled(true);
                }
            }
            else{
                ui->timeEdit_1_2->setDisabled(true);
            }
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timeRadioButtonSlot_2(bool checked){
    switch(ui->tabWidget->currentIndex()){
        case(0):{
            if(checked){
                if(counterCom->isWork()){
                    ui->timeEdit_2_1->setEnabled(true);
                }
            }
            else{
                ui->timeEdit_2_1->setDisabled(true);
            }
            break;
        }
        case(1):{
            if(checked){
                if(counterTex->isWork()){
                    ui->timeEdit_2_2->setEnabled(true);
                }
            }
            else{
                ui->timeEdit_2_2->setDisabled(true);
            }
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timeRadioButtonSlot_3(bool checked){
    switch(ui->tabWidget->currentIndex()){
        case(0):{
            if(checked){
                if(counterCom->isWork()){
                    ui->timeEdit_3_1->setEnabled(true);
                }
            }
            else{
                ui->timeEdit_3_1->setDisabled(true);
            }
            break;
        }
        case(1):{
            if(checked){
                if(counterTex->isWork()){
                    ui->timeEdit_3_2->setEnabled(true);
                }
            }
            else{
                ui->timeEdit_3_2->setDisabled(true);
            }
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::errorSlot(bool critical, QString error){
    if(critical){
        message(error);
    }
    writeLog(error);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::setCounterTimeSlot(){
    passDialog dialog(this);
    dialog.setTitle("Введите пароль");
    dialog.setWindowTitle(appName);
    dialog.setWindowIcon(QIcon(QApplication::applicationDirPath()+"\\gas.jpg"));
    if(dialog.exec() == QDialog::Accepted){
        QByteArray pass = dialog.getPass().toLatin1();
        QByteArray result;
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if(!pass.isEmpty()){
            hash.addData(pass);
            result = hash.result();
        }
        if(result == checkSumm){
            switch(ui->tabWidget->currentIndex()){
                case(0):{
                    if(ui->timeRadioButton_1_1->isChecked()){//время 1
                        currentCounter->setTime(1,ui->timeEdit_1_1->time());
                        currentCounter->setTimeComment(1,ui->groupBox_1_1->toolTip());
                        ui->timeOriginEdit_1_1->setTime(counterCom->getOriginTime(1));
                        if(ui->channelCheckBox_1_1_1)currentCounter->setChannel(1,1,true);
                        else currentCounter->setChannel(1,1,false);
                        if(ui->channelCheckBox_2_1_1)currentCounter->setChannel(1,2,true);
                        else currentCounter->setChannel(1,2,false);
                        if(ui->channelCheckBox_3_1_1)currentCounter->setChannel(1,3,true);
                        else currentCounter->setChannel(1,3,false);
                        if(ui->channelCheckBox_4_1_1)currentCounter->setChannel(1,4,true);
                        else currentCounter->setChannel(1,4,false);
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) +
                                 ". Установлено время снятия показаний №1 на " + ui->timeEdit_1_1->time().toString("hh:mm"));
                    }
                    else{
                        currentCounter->disableTime(1);
                        ui->timeOriginEdit_1_1->setTime(QTime(0,0,0));
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Отключено время снятия показаний №1");
                    }

                    if(ui->timeRadioButton_2_1->isChecked()){//время 2
                        currentCounter->setTime(2,ui->timeEdit_2_1->time());
                        ui->timeOriginEdit_2_1->setTime(counterCom->getOriginTime(2));
                        currentCounter->setTimeComment(2,ui->groupBox_2_1->toolTip());
                        if(ui->channelCheckBox_2_2_1)currentCounter->setChannel(2,1,true);
                        else currentCounter->setChannel(2,1,false);
                        if(ui->channelCheckBox_1_2_1)currentCounter->setChannel(2,2,true);
                        else currentCounter->setChannel(2,2,false);
                        if(ui->channelCheckBox_2_2_1)currentCounter->setChannel(2,3,true);
                        else currentCounter->setChannel(2,3,false);
                        if(ui->channelCheckBox_3_2_1)currentCounter->setChannel(2,4,true);
                        else currentCounter->setChannel(2,4,false);
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) +
                                 ". Установлено время снятия показаний №2 на " + ui->timeEdit_2_1->time().toString("hh:mm"));
                    }
                    else{
                        currentCounter->disableTime(2);
                        ui->timeOriginEdit_2_1->setTime(QTime(0,0,0));
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Отключено время снятия показаний №2");
                    }
                    if(ui->timeRadioButton_3_1->isChecked()){//время 3
                        currentCounter->setTime(3,ui->timeEdit_3_1->time());
                        ui->timeOriginEdit_3_1->setTime(counterCom->getOriginTime(3));
                        currentCounter->setTimeComment(3,ui->groupBox_3_1->toolTip());
                        if(ui->channelCheckBox_2_3_1)currentCounter->setChannel(3,1,true);
                        else currentCounter->setChannel(3,1,false);
                        if(ui->channelCheckBox_1_3_1)currentCounter->setChannel(3,2,true);
                        else currentCounter->setChannel(3,2,false);
                        if(ui->channelCheckBox_2_3_1)currentCounter->setChannel(3,3,true);
                        else currentCounter->setChannel(3,3,false);
                        if(ui->channelCheckBox_3_3_1)currentCounter->setChannel(3,4,true);
                        else currentCounter->setChannel(3,4,false);
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) +
                                 ". Установлено время снятия показаний №3 на " + ui->timeEdit_3_1->time().toString("hh:mm"));
                    }
                    else{
                        currentCounter->disableTime(3);
                        ui->timeOriginEdit_3_1->setTime(QTime(0,0,0));
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Отключено время снятия показаний №3");
                    }
                    break;
                }
                case(1):{
                    if(ui->timeRadioButton_1_2->isChecked()){
                        currentCounter->setTime(1,ui->timeEdit_1_2->time());
                        ui->timeOriginEdit_1_2->setTime(counterTex->getOriginTime(1));
                        currentCounter->setTimeComment(1,ui->groupBox_1_2->toolTip());
                        if(ui->channelCheckBox_1_1_2)currentCounter->setChannel(1,1,true);
                        else currentCounter->setChannel(1,1,false);
                        if(ui->channelCheckBox_2_1_2)currentCounter->setChannel(1,2,true);
                        else currentCounter->setChannel(1,2,false);
                        if(ui->channelCheckBox_3_1_2)currentCounter->setChannel(1,3,true);
                        else currentCounter->setChannel(1,3,false);
                        if(ui->channelCheckBox_4_1_2)currentCounter->setChannel(1,4,true);
                        else currentCounter->setChannel(1,4,false);
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) +
                                 ". Установлено время снятия показаний №1 на " + ui->timeEdit_1_2->time().toString("hh:mm"));
                    }
                    else{
                        currentCounter->disableTime(1);
                        ui->timeOriginEdit_1_2->setTime(QTime(0,0,0));
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Отключено время снятия показаний №1");
                    }

                    if(ui->timeRadioButton_2_2->isChecked()){
                        currentCounter->setTime(2,ui->timeEdit_2_2->time());
                        ui->timeOriginEdit_2_2->setTime(counterTex->getOriginTime(2));
                        currentCounter->setTimeComment(2,ui->groupBox_2_2->toolTip());
                        if(ui->channelCheckBox_2_2_2)currentCounter->setChannel(2,1,true);
                        else currentCounter->setChannel(2,1,false);
                        if(ui->channelCheckBox_1_2_2)currentCounter->setChannel(2,2,true);
                        else currentCounter->setChannel(2,2,false);
                        if(ui->channelCheckBox_2_2_2)currentCounter->setChannel(2,3,true);
                        else currentCounter->setChannel(2,3,false);
                        if(ui->channelCheckBox_3_2_2)currentCounter->setChannel(2,4,true);
                        else currentCounter->setChannel(2,4,false);
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) +
                                 ". Установлено время снятия показаний №2 на " + ui->timeEdit_2_2->time().toString("hh:mm"));
                    }
                    else{
                        currentCounter->disableTime(2);
                        ui->timeOriginEdit_2_2->setTime(QTime(0,0,0));
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Отключено время снятия показаний №2");
                    }
                    if(ui->timeRadioButton_3_2->isChecked()){
                        currentCounter->setTime(3,ui->timeEdit_3_2->time());
                        ui->timeOriginEdit_3_2->setTime(counterTex->getOriginTime(3));
                        currentCounter->setTimeComment(3,ui->groupBox_3_2->toolTip());
                        if(ui->channelCheckBox_2_3_2)currentCounter->setChannel(3,1,true);
                        else currentCounter->setChannel(3,1,false);
                        if(ui->channelCheckBox_1_3_2)currentCounter->setChannel(3,2,true);
                        else currentCounter->setChannel(3,2,false);
                        if(ui->channelCheckBox_2_3_2)currentCounter->setChannel(3,3,true);
                        else currentCounter->setChannel(3,3,false);
                        if(ui->channelCheckBox_3_3_2)currentCounter->setChannel(3,4,true);
                        else currentCounter->setChannel(3,4,false);
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) +
                                 ". Установлено время снятия показаний №3 на " + ui->timeEdit_3_2->time().toString("hh:mm"));
                    }
                    else{
                        currentCounter->disableTime(3);
                        ui->timeOriginEdit_3_2->setTime(QTime(0,0,0));
                        writeLog("Счетчик " + QString::number(currentCounter->getDevAddress()) + ". Отключено время снятия показаний №3");
                    }
                    break;
                }
            }
            message("Изменения вступили в силу.");
            return;
        }
        errorSlot(true,"Пароль неверный. \n Изменения не применены");
    }
}
//////////////////////////////////////////////////////////////////////////////////
void MainWindow::updateCounterTime(){
    IRVISTime time = counterCom->getTime(1);
    if(time.isEnable())ui->timeRadioButton_1_1->setChecked(true);
    ui->timeEdit_1_1->setTime(static_cast<QTime>(time));
    ui->groupBox_1_1->setToolTip(time.getComment());
    if(time.getChannel(1))ui->channelCheckBox_1_1_1->setChecked(true);
    if(time.getChannel(2))ui->channelCheckBox_2_1_1->setChecked(true);
    if(time.getChannel(3))ui->channelCheckBox_3_1_1->setChecked(true);
    if(time.getChannel(4))ui->channelCheckBox_4_1_1->setChecked(true);


    time = counterCom->getTime(2);
    if(time.isEnable())ui->timeRadioButton_2_1->setChecked(true);
    ui->timeEdit_2_1->setTime(static_cast<QTime>(time));
    ui->groupBox_2_1->setToolTip(time.getComment());
    if(time.getChannel(1))ui->channelCheckBox_1_1_1->setChecked(true);
    if(time.getChannel(2))ui->channelCheckBox_2_1_1->setChecked(true);
    if(time.getChannel(3))ui->channelCheckBox_3_1_1->setChecked(true);
    if(time.getChannel(4))ui->channelCheckBox_4_1_1->setChecked(true);

    time = counterCom->getTime(3);
    if(time.isEnable()){
        ui->timeRadioButton_3_1->setChecked(true);
        ui->timeEdit_3_1->setTime(static_cast<QTime>(time));
        ui->groupBox_3_1->setToolTip(time.getComment());
    }

    time = counterTex->getTime(1);
    if(time.isEnable()){
        ui->timeRadioButton_1_2->setChecked(true);
        ui->timeEdit_1_2->setTime(static_cast<QTime>(time));
        ui->groupBox_1_2->setToolTip(time.getComment());
    }

    time = counterTex->getTime(2);
    if(time.isEnable()){
        ui->timeRadioButton_2_2->setChecked(true);
        ui->timeEdit_2_2->setTime(static_cast<QTime>(time));
        ui->groupBox_2_2->setToolTip(time.getComment());
    }

    time = counterTex->getTime(3);
    if(time.isEnable()){
        ui->timeRadioButton_3_2->setChecked(true);
        ui->timeEdit_3_2->setTime(static_cast<QTime>(time));
        ui->groupBox_3_2->setToolTip(time.getComment());
    }

    ui->timeOriginEdit_1_1->setTime(counterCom->getOriginTime(1));
    ui->timeOriginEdit_2_1->setTime(counterCom->getOriginTime(2));
    ui->timeOriginEdit_3_1->setTime(counterCom->getOriginTime(3));

    ui->timeOriginEdit_1_2->setTime(counterCom->getOriginTime(1));
    ui->timeOriginEdit_2_2->setTime(counterCom->getOriginTime(1));
    ui->timeOriginEdit_3_2->setTime(counterCom->getOriginTime(1));
}
/////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::dataReadySlot(dayDataBlock block1, dayDataBlock block2, QString comment){
    QString report = block1.dateTime.toString("Начало (dd.MM.yy hh:mm)= ");
    report += QString::number(block1.Vnu);
    report += "  ";
    report += block2.dateTime.toString("Конец (dd.MM.yy hh:mm)= ");
    report += QString::number(block2.Vnu);
    report += "  За период = ";
    report += QString::number(block2.Vnu - block1.Vnu);
    report += "   М куб./сутки";
    writeDataInFile(report);

    if(oDialog.isVisible()){
        oDialog.hide();
    }

    oDialog.setModal(false);
    oDialog.setBlocks(block1,block2);
    oDialog.setWindowTitle(appName + ". " + comment);
    oDialog.setComment(comment);
    oDialog.show();
}
//////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::changePassSlot(){
    passDialog dialog(this);
    dialog.setTitle("Введите старый пароль");
    dialog.setWindowTitle(appName);
    if(dialog.exec() == QDialog::Accepted){
        QByteArray pass = dialog.getPass().toLatin1();
        QByteArray result;
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if(!pass.isEmpty()){
            hash.addData(pass);
            result = hash.result();
            hash.reset();
        }
        if(result == checkSumm){
            result.clear();
            dialog.setTitle("Введите новый пароль");
            if(dialog.exec() == QDialog::Accepted){
                QString newPass = dialog.getPass();
                dialog.setTitle("Подтвердите новый пароль");
                if(dialog.exec() == QDialog::Accepted){
                    if(newPass != dialog.getPass()){
                        errorSlot(true,"Новые пароли не совпадают \n Продолжает действовать старый пароль.");
                        return;
                    }
                    else{
                        errorSlot(true,"Пароль успешно изменен");
                        pass = newPass.toLatin1();
                        hash.addData(pass);
                        checkSumm = hash.result();
                        writeConfigFile();
                        return;
                    }
                }
            }
        }
        errorSlot(true,"Старый пароль введен не верно. \n Продолжает действовать старый пароль.");
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::setComment(int timeIndex){
    CommentDialog cDialog(this);
    cDialog.setWindowTitle(appName);
    cDialog.setComment(currentCounter->getTimeComment(timeIndex));
    if(cDialog.exec() == QDialog::Accepted){
        currentCounter->setTimeComment(timeIndex,cDialog.getComment());
        switch(ui->tabWidget->currentIndex()){
            case(0):{
                switch(timeIndex){
                    case(1):{
                        ui->groupBox_1_1->setToolTip(cDialog.getComment());
                        break;
                    }
                    case(2):{
                        ui->groupBox_2_1->setToolTip(cDialog.getComment());
                        break;
                    }
                    case(3):{
                        ui->groupBox_3_1->setToolTip(cDialog.getComment());
                        break;
                    }
                }
                break;
            }
            case(1):{
                switch(timeIndex){
                    case(1):{
                        ui->groupBox_1_2->setToolTip(cDialog.getComment());
                        break;
                    }
                    case(2):{
                        ui->groupBox_2_2->setToolTip(cDialog.getComment());
                        break;
                    }
                    case(3):{
                        ui->groupBox_3_2->setToolTip(cDialog.getComment());
                        break;
                    }
                }
                break;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timeCommentSlot_1(){
    setComment(1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timeCommentSlot_2(){
    setComment(2);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timeCommentSlot_3(){
    setComment(3);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


