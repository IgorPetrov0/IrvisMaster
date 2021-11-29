#ifndef COMMENTDIALOG_H
#define COMMENTDIALOG_H

#include <QDialog>

namespace Ui {
class CommentDialog;
}

class CommentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommentDialog(QWidget *parent = nullptr);
    ~CommentDialog();
    QString getComment() const;
    void setComment(const QString &value);


protected:
    QString comment;

protected slots:
    void rememberSlot();

private:
    Ui::CommentDialog *ui;
};

#endif // COMMENTDIALOG_H
