#include "commentdialog.h"
#include "ui_commentdialog.h"

CommentDialog::CommentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommentDialog)
{
    ui->setupUi(this);
    connect(ui->okButton,&QPushButton::clicked,this,&CommentDialog::rememberSlot);
    connect(ui->cancelButton,&QPushButton::clicked,this,&CommentDialog::reject);
}
/////////////////////////////////////////////////////////////////////
CommentDialog::~CommentDialog(){
    delete ui;
}
/////////////////////////////////////////////////////////////////////
QString CommentDialog::getComment() const{
    return comment;
}
////////////////////////////////////////////////////////////////////
void CommentDialog::setComment(const QString &value){
    comment = value;
    ui->lineEdit->setText(value);
}
/////////////////////////////////////////////////////////////////////
void CommentDialog::rememberSlot(){
    comment = ui->lineEdit->text();
    this->accept();
}
/////////////////////////////////////////////////////////////////////
