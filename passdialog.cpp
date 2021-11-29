#include "passdialog.h"
#include "ui_passdialog.h"

passDialog::passDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::passDialog)
{
    ui->setupUi(this);
    connect(ui->okButton,&QPushButton::clicked,this,&passDialog::okSlot);
    connect(ui->cancelButton,&QPushButton::clicked,this,&passDialog::cancelSlot);
}
////////////////////////////////////////////////////////////////
passDialog::~passDialog(){
    delete ui;
}
///////////////////////////////////////////////////////////////
void passDialog::setTitle(QString title){
   ui->groupBox->setTitle(title);
}
////////////////////////////////////////////////////////////////
QString passDialog::getPass(){
    return pass;
}
///////////////////////////////////////////////////////////////
void passDialog::okSlot(){
    pass = ui->lineEdit->text();
    ui->lineEdit->clear();
    this->accept();
}
/////////////////////////////////////////////////////////////////////
void passDialog::cancelSlot(){
    ui->lineEdit->clear();
    this->reject();
}
///////////////////////////////////////////////////////////////
