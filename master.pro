QT += serialbus serialport widgets network

TARGET = modbusmaster
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        commentdialog.cpp \
        irviscounter.cpp \
        irvistime.cpp \
        mainwindow.cpp \
        mbtcpdriver.cpp \
        otchetdialog.cpp \
        passdialog.cpp \
        tcpserver.cpp \
        testdialog.cpp

HEADERS  += mainwindow.h \
    commentdialog.h \
    daydatablock.h \
    irviscounter.h \
    irvistime.h \
    mbtcpdriver.h \
    otchetdialog.h \
    passdialog.h \
    tcpserver.h \
    testdialog.h

FORMS    += mainwindow.ui \
         commentdialog.ui \
         otchetdialog.ui \
         passdialog.ui \
         settingsdialog.ui \
         testdialog.ui

RESOURCES += \
    master.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialbus/modbus/master
INSTALLS += target
