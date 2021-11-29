/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtSerialBus module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QStatusBar>
#include <QVector>
#include <QDateTime>
#include <QFile>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QTextStream>
#include <QMessageBox>
#include <QCryptographicHash>
#include "otchetdialog.h"
#include "irviscounter.h"
#include "testdialog.h"
#include "daydatablock.h"
#include "passdialog.h"
#include "commentdialog.h"
#include "tcpserver.h"




QT_BEGIN_NAMESPACE

class QModbusClient;
class QModbusReply;

namespace Ui {
class MainWindow;
class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog;
class WriteRegisterModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void writeLog(QString string);
    void writeDataInFile(QString dataString);
    void closeEvent(QCloseEvent *event);
    void message(QString message);
    bool readConfigFile();
    bool writeConfigFile();

    QSystemTrayIcon trayIcon;
    QMenu trayMenu;
    IRVISCounter *counterCom;
    IRVISCounter *counterTex;
    IRVISCounter *currentCounter;
    QString appName;//todo потом переделать на свойство QApplication
    QByteArray checkSumm;
    otchetDialog oDialog;
    TCPServer *server;

private slots:
    void tIconActivated(QSystemTrayIcon::ActivationReason reason);
    void tMenuShowSlot();
    void tMenuCloseSlot();
    void changeTabSlot(int index);
    void turnOnSlot();
    void timeRadioButtonSlot_1(bool checked);
    void timeRadioButtonSlot_2(bool checked);
    void timeRadioButtonSlot_3(bool checked);
    void errorSlot(bool critical, QString error);
    void setCounterTimeSlot();
    void updateCounterTime();
    void dataReadySlot(dayDataBlock block1, dayDataBlock block2, QString comment);
    void changePassSlot();
    void setComment(int timeIndex);
    void timeCommentSlot_1();
    void timeCommentSlot_2();
    void timeCommentSlot_3();


private:
    Ui::MainWindow *ui;

    int result;


};



#endif // MAINWINDOW_H
