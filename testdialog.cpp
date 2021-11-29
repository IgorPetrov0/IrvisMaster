#include "testdialog.h"
#include "ui_testdialog.h"

testDialog::testDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testDialog)
{
    ui->setupUi(this);
    ui->currentTimeLable->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));

    this->setWindowIcon(QIcon(QApplication::applicationDirPath()+"\\gas.jpg"));


    connect(ui->pushButton,&QPushButton::clicked,this,&testDialog::reject);



}
////////////////////////////////////////////////////////
testDialog::~testDialog(){
    delete ui;
}
//////////////////////////////////////////////////////////////////
void testDialog::setInfo(QString infoString){
    ui->regTypeLable->setText("РИ - " + infoString.mid(2,1));
    ui->versionLable->setText(infoString.mid(4,3));
    int num = infoString.mid(8,2).toInt();
    switch(num){
        case(1):{
            ui->flashTypeLable->setText("AT45DB011");
            break;
        }
        case(2):{
            ui->flashTypeLable->setText("AT45DB021");
            break;
        }
        case(3):{
            ui->flashTypeLable->setText("AT45DB041");
            break;
        }
        case(4):{
            ui->flashTypeLable->setText("AT45DB0081");
            break;
        }
        case(5):{
            ui->flashTypeLable->setText("AT45DB161");
            break;
        }
        case(6):{
            ui->flashTypeLable->setText("AT45DB321");
            break;
        }
        case(7):{
            ui->flashTypeLable->setText("AT45DB642");
            break;
        }
    }
    ui->channelsLable->setText(infoString.mid(11,1));
}
////////////////////////////////////////////////////////////////////////////
void testDialog::setCalcTime(QString time){
    ui->calcTimeLable->setText(time);
}
///////////////////////////////////////////////////////////////////////
void testDialog::setDevTime(QString time){
    ui->devTimeTimeLable->setText(time);
}
///////////////////////////////////////////////////////////////////////////

