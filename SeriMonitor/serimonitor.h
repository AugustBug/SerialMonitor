/*****************************************
  *                                      *
  * Ahmert - Serial Monitor - 21.07.2016 *
  *                                      *
  *****************************************/
/**
  * Communicates over serial port and views in and out data
  * as ascii or hexadecimal with timestamps
  */

#ifndef SERIMONITOR_H
#define SERIMONITOR_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include "message.h"
#include "stringprocessor.h"

#include "ui_serimonitor.h"

namespace Ui {
class SeriMonitor;
}

class SeriMonitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit SeriMonitor(QWidget *parent = 0);
    ~SeriMonitor();

    void refreshCommandCombos();
    void updateCommandFile(QString fileName);

    void addNewMessageSelection();
    void refreshMessageSelectionCombos();
    void sendMessage(int messageSelectionId, bool cr);

    bool openSerialPort(QString speed, const QString name);

    void closeSerialPort();

    void addToMessageList(QDateTime time, QString message, bool isSent, bool isAscii, bool isInitial);

    void readCommandFile(QString fileName);

    QString cleanComboPortName(QString orig);

    void refreshList(bool isAscii);

    void startLogging();
    void addToLog(QDateTime time, QString message, bool isSent);
    void addToLog(QDateTime time, bool connected, QString message = "");


private slots:

    void readData();

    void on_action_k_triggered();

    void on_refreshBtn_clicked();

    void on_connectBtn_clicked();

    void on_disconnectBtn_clicked();

    void on_sendBtn_clicked();

    void on_sendTextEdit_returnPressed();

    void on_btnAutoScroll_clicked();

    void on_btnManualScroll_clicked();

    void on_hexBtn_clicked();

    void on_asciiBtn_clicked();

    void on_btnFontInc_clicked();

    void on_btnFontDec_clicked();

    void on_btnClearLv_clicked();

private:
    Ui::SeriMonitor *ui;

    QSerialPort * serial;

    QStandardItemModel *myModel;

    QFile *logFile;
    QTextStream *logStream;
    bool logFileInitailazed;

    QBrush * sentTextBrushBg;
    QBrush * sentTextBrushFg;
    QBrush * recievedTextBrushBg;
    QBrush * recievedTextBrushFg;

    QString * readBuffer;

    QList<Message *> listMessages;

    QList<QString> listPortNames;

    bool autoScrollEnabled;  // false - manual scroll
    bool asciiViewEnabled;   // false - hex view

    QPixmap * imgConnected;
    QPixmap * imgDisconnected;

    int fontSizeOfList;
    QFont *fontOfList;

    StringProcessor *stringPro;

    int timerId;
    QString activePortName;

    QLayout *layoutV;

    int lastFrameId;

    bool busy;

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // SERIMONITOR_H
