#ifndef PASSDIALOG_H
#define PASSDIALOG_H

#include <QDialog>

namespace Ui {
class passDialog;
}


class passDialog : public QDialog
{
    Q_OBJECT

public:
    explicit passDialog(QWidget *parent = nullptr);
    ~passDialog();
    void setTitle(QString title);
    QString getPass();

private:
    Ui::passDialog *ui;
    QString pass;

protected slots:
    void okSlot();
    void cancelSlot();



};

#endif // PASSDIALOG_H
