/*****************************************
  *                                      *
  * Ahmert - Serial Monitor - 21.07.2016 *
  *                                      *
  *****************************************/

#include "serimonitor.h"

SeriMonitor::SeriMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SeriMonitor)
{
    ui->setupUi(this);

    ui->speedCmb->addItem("1200");
    ui->speedCmb->addItem("2400");
    ui->speedCmb->addItem("4800");
    ui->speedCmb->addItem("9600");
    ui->speedCmb->addItem("19200");
    ui->speedCmb->addItem("38400");
    ui->speedCmb->addItem("57600");
    ui->speedCmb->addItem("115200");
    ui->speedCmb->setCurrentIndex(5);

    stringPro = new StringProcessor();

    // disable gui resizable
    // window()->layout()->setSizeConstraint(QLayout::SetFixedSize);
    QWidget::setFixedSize(670, 950);  // it also works
    busy = false;
    serial = new QSerialPort(this);

    myModel = new QStandardItemModel();

    ui->lvRead->setModel(myModel);

    sentTextBrushBg = new QBrush(*(new QColor(40,76,111)));
    sentTextBrushFg = new QBrush(*(new QColor(180, 230, 180)));
    recievedTextBrushBg = new QBrush(*(new QColor(20,56,91)));
    recievedTextBrushFg = new QBrush(*(new QColor(230,180,180)));

    ui->connectBtn->setEnabled(false);
    ui->disconnectBtn->setEnabled(false);
    ui->sendBtn->setEnabled(false);

    autoScrollEnabled = true;
    ui->btnAutoScroll->setEnabled(false);
    ui->btnManualScroll->setEnabled(true);

    asciiViewEnabled = true;
    ui->hexBtn->setEnabled(true);
    ui->asciiBtn->setEnabled(false);

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    readBuffer = new QString("a");

    // loglamayi baslat
    logFileInitailazed = false;
    startLogging();

    fontSizeOfList = 9;
    fontOfList = new QFont("Arial", fontSizeOfList, QFont::Normal);
    ui->lvRead->setFont(*fontOfList);

    imgConnected = new QPixmap("../icons/connected.png");
    imgDisconnected = new QPixmap("../icons/disconnected.png");
    ui->lblSmiley->setPixmap(*imgDisconnected);
    ui->lblSmiley->setScaledContents(true);

    // buton ikonlari
    ui->refreshBtn->setIcon(QIcon("../icons/refresh.png"));
    ui->refreshBtn->setIconSize(QSize(28, 28));

    ui->connectBtn->setIcon(QIcon("../icons/play.png"));
    ui->connectBtn->setIconSize(QSize(28, 28));

    ui->disconnectBtn->setIcon(QIcon("../icons/pause2.png"));
    ui->disconnectBtn->setIconSize(QSize(28, 28));

    layoutV = new QVBoxLayout;
    layoutV->setContentsMargins(0, 0, 0, 0);
    layoutV->setSpacing(0);

    // scans ports on initialize
    on_refreshBtn_clicked();

    lastFrameId = 1;
    timerId = startTimer(1000);
}

SeriMonitor::~SeriMonitor()
{
    logFile->close();
    killTimer(timerId);
    delete ui;
}

void SeriMonitor::startLogging() {
    if(!QDir("logs").exists()) {
        QDir().mkdir("logs");
    }
    QDateTime nnn = QDateTime::currentDateTime();
    QDateTime *now = &(nnn);
    logFile = new QFile("logs/" + now->toString("yyyy_MM_dd__hh_mm_ss") + "_mm.log");
    if(logFile->open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        logStream = new QTextStream(logFile);

        logFileInitailazed = true;
        *logStream << "[LOG RECORD\tDate: " + QDateTime::currentDateTime().toString("yyyy MM dd _ hh:mm:ss") + "]" << endl << endl << endl;
    }
}

void SeriMonitor::addToLog(QDateTime time, bool connected, QString message) {
    if(logFileInitailazed) {
        if(connected) {
            *logStream << "[" + time.toString("hh:mm:ss.zzz") + "]" << " [Connected\t" + message + "]" << endl;
        } else {
            *logStream << "[" + time.toString("hh:mm:ss.zzz") + "]" << " [Connection Lost\t" + message + "]" << endl << endl;
        }
    }
}

void SeriMonitor::addToLog(QDateTime time, QString message, bool isSent) {
    if(logFileInitailazed) {
        if(isSent) {
            *logStream << "[" + time.toString("hh:mm:ss.zzz") + "]" << " [TX] " + message << endl;
        } else {
            *logStream << "[" + time.toString("hh:mm:ss.zzz") + "]" << " [RX]   " + message << endl;
        }
    }
}


// main menu exit button trigger
void SeriMonitor::on_action_k_triggered()
{
    QCoreApplication::exit(0);
}

void SeriMonitor::on_refreshBtn_clicked()
{
    ui->portCmb->clear();
    Q_FOREACH(QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        if(listPortNames.contains(info.portName())) {
            ui->portCmb->addItem(info.portName());
        } else {
            ui->portCmb->addItem(info.portName() + "  *");
        }
    }

    listPortNames.clear();
    Q_FOREACH(QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        listPortNames.append(info.portName());
    }

    if((ui->portCmb->count() > 0) && (!serial->isOpen())) {
        ui->connectBtn->setEnabled(true);
    }
}

void SeriMonitor::on_disconnectBtn_clicked()
{
    closeSerialPort();
    ui->disconnectBtn->setEnabled(false);
    ui->connectBtn->setEnabled(true);
    ui->sendBtn->setEnabled(false);
    activePortName = "";

}

QString SeriMonitor::cleanComboPortName(QString orig) {
    if(orig.contains("*")) {
        return orig.mid(0, orig.indexOf('*') - 2);
    } else {
        return orig;
    }

}

void SeriMonitor::on_connectBtn_clicked()
{
    if(openSerialPort(ui->speedCmb->currentText(), cleanComboPortName(ui->portCmb->currentText()))) {
        ui->connectBtn->setEnabled(false);
        ui->disconnectBtn->setEnabled(true);
        ui->sendBtn->setEnabled(true);
        activePortName = cleanComboPortName(ui->portCmb->currentText());

    } else {
        ui->disconnectBtn->setEnabled(false);
        ui->connectBtn->setEnabled(true);
        activePortName = "";

    }

}

// default baudrate is 1200
// baudrates and com port info is string
bool SeriMonitor::openSerialPort(QString speed, const QString name) {
    qint32 baudRate = QSerialPort::Baud1200;

    if(speed.compare("1200") == 0) {
        baudRate = QSerialPort::Baud1200;
    } else if(speed.compare("2400") == 0) {
        baudRate = QSerialPort::Baud2400;
    } else if(speed.compare("4800") == 0) {
        baudRate = QSerialPort::Baud4800;
    } else if(speed.compare("9600") == 0) {
        baudRate = QSerialPort::Baud9600;
    } else if(speed.compare("19200") == 0) {
        baudRate = QSerialPort::Baud19200;
    } else if(speed.compare("38400") == 0) {
        baudRate = QSerialPort::Baud38400;
    } else if(speed.compare("57600") == 0) {
        baudRate = QSerialPort::Baud57600;
    } else if(speed.compare("115200") == 0) {
        baudRate = QSerialPort::Baud115200;
    }


    serial->setPortName(name);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadWrite)) {

        ui->statusBar->showMessage(QString("Connected, \tPORT : "  + name + ", BAUDRATE : " + QString::number(serial->baudRate())));
        ui->lblSmiley->setPixmap(*imgConnected);
        addToLog(QDateTime::currentDateTime(), true, "PORT : "  + name + ", BAUDRATE : " + QString::number(serial->baudRate()));
        return true;
    } else {
        ui->statusBar->showMessage(QString("ERROR!! Could not connect, \tPORT : "  + name + ", BAUDRATE : " + QString::number(serial->baudRate())));
        ui->lblSmiley->setPixmap(*imgDisconnected);
        QMessageBox::critical(this, tr("Error"), serial->errorString() + " - " + speed + " - " + name);
        return false;
    }
}

void SeriMonitor::closeSerialPort() {
    serial->close();
    ui->statusBar->showMessage(QString("Connection Closed"));
    ui->lblSmiley->setPixmap(*imgDisconnected);
    addToLog(QDateTime::currentDateTime(), false);
}

void SeriMonitor::readData() {

    QByteArray data = serial->readAll();
    int indexOfReturn;

    readBuffer->append(QString::fromUtf8(data));

    while((indexOfReturn = readBuffer->indexOf("\r")) >= 0) {
        QString *income = new QString("> ");

        income->append(readBuffer->mid(1, indexOfReturn));

        addToMessageList(QDateTime::currentDateTime(), readBuffer->mid(1, indexOfReturn), false, asciiViewEnabled, true);
        addToLog(QDateTime::currentDateTime(), readBuffer->mid(1, indexOfReturn), false);

        // auto scroll if enabled
        if(autoScrollEnabled) {
            ui->lvRead->scrollToBottom();
        }

        QString nQStr = readBuffer->mid(indexOfReturn+1);

        readBuffer = new QString(nQStr);
    }

}


void SeriMonitor::on_sendBtn_clicked()
{
    while(busy);
    busy = true;
    serial->write((ui->sendTextEdit->text() + "\r").toUtf8().constData());

    addToLog(QDateTime::currentDateTime(), ui->sendTextEdit->text(), true);
    addToMessageList(QDateTime::currentDateTime(), ui->sendTextEdit->text(), true, asciiViewEnabled, true);

    // auto scroll if enabled
    if(autoScrollEnabled) {
        ui->lvRead->scrollToBottom();
    }
    busy = false;
}

void SeriMonitor::addToMessageList(QDateTime time, QString message, bool isSent, bool isAscii, bool isInitial) {

    QString messageToView = message;

    if(!isAscii) {
        messageToView = stringPro->convertToHexView(message);
    }

    QStandardItem *item = new QStandardItem(time.toString("hh:mm:ss.zzz") + " >>   " + messageToView);

    if(isInitial) {
        if(isSent) {
            ui->sendTextEdit->setText("");
        }
        listMessages.append(new Message(time, message, isSent));
    }

    item->setEditable(false);

    if(isSent) {
        item->setBackground(*sentTextBrushBg);
        item->setForeground(*sentTextBrushFg);
        item->setTextAlignment(Qt::AlignLeft);
    } else {
        item->setBackground(*recievedTextBrushBg);
        item->setForeground(*recievedTextBrushFg);
        item->setText(time.toString("hh:mm:ss.zzz") + " >>           " + messageToView);
    }

    myModel->appendRow(item);
}



void SeriMonitor::refreshList(bool isAscii) {
    myModel->clear();

    for(int i=0; i<listMessages.size(); i++) {
        addToMessageList(listMessages.at(i)->getMessageTime(), QString::fromUtf8(listMessages.at(i)->getData()), listMessages.at(i)->isMessageSent(), isAscii, false);
    }
}


void SeriMonitor::on_sendTextEdit_returnPressed()
{
    if(ui->sendBtn->isEnabled()) {
        on_sendBtn_clicked();
    }
}


void SeriMonitor::on_btnClearLv_clicked()
{
    myModel->clear();

    listMessages.clear();
}

void SeriMonitor::on_btnAutoScroll_clicked()
{
    autoScrollEnabled = true;
    ui->lvRead->scrollToBottom();
    ui->btnAutoScroll->setEnabled(false);
    ui->btnManualScroll->setEnabled(true);
}

void SeriMonitor::on_btnManualScroll_clicked()
{
    autoScrollEnabled = false;
    ui->btnAutoScroll->setEnabled(true);
    ui->btnManualScroll->setEnabled(false);
}


void SeriMonitor::on_hexBtn_clicked()
{
    asciiViewEnabled = false;
    ui->hexBtn->setEnabled(false);
    ui->asciiBtn->setEnabled(true);

    refreshList(false);
}

void SeriMonitor::on_asciiBtn_clicked()
{
    asciiViewEnabled = true;
    ui->hexBtn->setEnabled(true);
    ui->asciiBtn->setEnabled(false);

    refreshList(true);
}

void SeriMonitor::on_btnFontInc_clicked()
{
    if(fontSizeOfList < 16) {
        fontSizeOfList++;
    }
    fontOfList = new QFont("Arial", fontSizeOfList, QFont::Normal);
    ui->lvRead->setFont(*fontOfList);
}

void SeriMonitor::on_btnFontDec_clicked()
{
    if(fontSizeOfList > 6) {
        fontSizeOfList--;
    }
    fontOfList = new QFont("Arial", fontSizeOfList, QFont::Normal);
    ui->lvRead->setFont(*fontOfList);
}

// checks port if still open every second
void SeriMonitor::timerEvent(QTimerEvent *event) {
    if(activePortName.size() > 1) {
        Q_FOREACH(QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
            if(QString::compare(info.portName(), activePortName) == 0) {
                return;
            }
        }
        on_disconnectBtn_clicked();

        QMessageBox::critical(this, tr("Error"), "Connection Lost!!");
        addToLog(QDateTime::currentDateTime(), false);
    }
}
