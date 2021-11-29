#ifndef OTCHETDIALOG_H
#define OTCHETDIALOG_H

#include <QDialog>
#include "daydatablock.h"

namespace Ui {
class otchetDialog;
}

class otchetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit otchetDialog(QWidget *parent = nullptr);
    ~otchetDialog();
    void setBlocks(dayDataBlock block1, dayDataBlock block2);
    void setComment(QString comment);

private:
    Ui::otchetDialog *ui;
};

#endif // OTCHETDIALOG_H
