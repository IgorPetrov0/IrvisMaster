#include "otchetdialog.h"
#include "ui_otchetdialog.h"

otchetDialog::otchetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::otchetDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(hide()));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
otchetDialog::~otchetDialog(){
    delete ui;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void otchetDialog::setBlocks(dayDataBlock block1, dayDataBlock block2){
    QDateTime dt = QDateTime::currentDateTime();
    QTime t = QTime::currentTime();
    dt.setTime(QTime(t.hour(),0,0));

    ui->dateTimeLable->setText(dt.toString("hh часов dd MMMM yyyy года"));
    ui->dateTime1Lable->setText(block1.dateTime.toString("hh часов dd MMMM yyyy года = "));
    ui->dateTime2Lable->setText(block2.dateTime.toString("hh часов dd MMMM yyyy года = "));
    ui->rashod1Lable->setText(QString::number(block1.Vnu));
    ui->rashod2Lable->setText(QString::number(block2.Vnu));
    ui->dayRashodLable->setText(QString::number(block2.Vnu - block1.Vnu));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void otchetDialog::setComment(QString comment){
    ui->commentLabel->setText(comment);
}
