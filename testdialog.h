#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class testDialog;
}

class testDialog : public QDialog
{
    Q_OBJECT

public:
    explicit testDialog(QWidget *parent = nullptr);
    ~testDialog();
    void setInfo(QString infoString);
    void setCalcTime(QString time);
    void setDevTime(QString time);



protected:



private:
    Ui::testDialog *ui;
};

#endif // TESTDIALOG_H
