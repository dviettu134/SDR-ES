/*  SDR-ES Satellite Control Software
 *  Author: Hoang Trong Hung
 *          Phung Dinh Tai
 *  Embedded Networking Group
 *  Lab411 - C9 - Hanoi University of Science and Technology
 *  Date Created: 26th - June - 2016
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "function_Check.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isConnect = false;
    ui->setupUi(this);
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->cbxPort->addItem(info.portName);
    ui->cbxPort->setEditable(true);

    ui->cbxBaudrate->addItem("1200", BAUD1200);
    ui->cbxBaudrate->addItem("2400", BAUD2400);
    ui->cbxBaudrate->addItem("4800", BAUD4800);
    ui->cbxBaudrate->addItem("9600", BAUD9600);
    ui->cbxBaudrate->addItem("19200", BAUD19200);
    ui->cbxBaudrate->addItem("38400", BAUD38400);
    ui->cbxBaudrate->addItem("115200", BAUD115200);

    ui->cbxBaudrate->setCurrentIndex(5);

    console = new Console;
    console->setEnabled(true);
    console->setLocalEchoEnabled(true);
    ui->layConsole->addWidget(console);

    led = new HLed();
    led->turnOff();
    ui->layLed->addWidget(led);

    timer = new QTimer(this);
    timer->setInterval(40);

    PortSettings settings = {BAUD38400, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    port = new QextSerialPort(ui->cbxPort->currentText(), settings, QextSerialPort::Polling);

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();

    connect(timer, SIGNAL(timeout()), SLOT(readData()));
    connect(port, SIGNAL(readyRead()), SLOT(readData()));
    connect(console, SIGNAL(getData(QByteArray)), SLOT(writeData(QByteArray)));
    connect(console, SIGNAL(getEnter()), SLOT(sendCommand()));
    connect(ui->cbxBaudrate, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ui->cbxPort, SIGNAL(editTextChanged(QString)), SLOT(onPortNameChanged(QString)));
    connect(ui->btnOpenClose, SIGNAL(clicked()), SLOT(onOpenCloseButtonClicked()));
    connect(ui->btnReset,SIGNAL(clicked()), SLOT(btnReset_clicked()));
    connect(ui->btnModuExcute,SIGNAL(clicked()),SLOT(btnModulExcute_clicked()));
    connect(ui->btnDemodExcute,SIGNAL(clicked()),SLOT(btnDemodExcute_clicked()));
    connect(this,SIGNAL(readyUpdate()),this,SLOT(update()));
    
    get_Status();
    ui->tabModulation->setEnabled(false);
    ui->tabDemodulation->setEnabled(false);

    setWindowTitle(tr("SDR-ES Modem Control"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void MainWindow::readData()
{
    if(!port->isOpen()) port->open(QIODevice::ReadWrite);
    int t = port->bytesAvailable();
    if (t) {

        /////////////////////////////////////////////////
        QByteArray ba = port->readAll();
        char* tmp = ba.data();
        buffer_data+=tmp;
         QString str;
        QStringList buff = buffer_data.split(QRegExp("\\s+"));
        if(buff.contains("/root]#"))
        {
        buff.replaceInStrings("<","\0");
        buff.replaceInStrings(">","\0");
        buff.replaceInStrings(":","\0");
        FileData file(DATA_PATH);
        int i;
        int s1 = 0,s2=0;
        int j=buff.count();
        for(i=0;i<j;i++)
        {
            if((buff[i]=="mipc")&&(buff[i-2]=="Set")) buff[i]="mipc1";
            if((buff[i]=="mipc")&&(buff[i-2]=="Max")) buff[i]="mipc2";
            if((buff[i]=="mipc")&&(buff[i-2]=="Min")) buff[i]="mipc3";
        }
        for(i=0;i<j;i++)
        {
            if(file.searchByCode(buff[i])!="\0")
            {
                if(buff[i]=="mim/mdm")
                {
                    if(s1==1) continue;
                else s1=1;
            }
                if(buff[i]=="dim/ddm")
                {
                    if(s2==1) continue;
                else s2=1;
            }
                if((buff[i]=="mip")&&(buff[i+1]=="Enabled"))
                {
                str = buff[i]+","+check(buff[i+3]);
                file.EditByCode(buff[i],str);

            }
            else
            {\
             QString line = file.searchByCode(buff[i]);

             if(buff[i+1]!="=")
               str = buff[i]+","+check(buff[i+1]);
             else
                 str = buff[i]+","+check(buff[i+2]);
             if(str.compare(line))
             {
                 file.EditByCode(buff[i],str);

             }
            }
        }
        }
        
            emit readyUpdate();
        /////////////////////////////////////////////////

        qDebug() << buffer_data<<endl;
        buffer_data.clear();
    }
        console->moveCursor(QTextCursor::End);
        console->putData(ba);
    }


}

void MainWindow::writeData(const QByteArray &data)
{
    cmd += QString(data);
}

void MainWindow::sendCommand()
{
    if(!cmd.compare("clear")) {
        console->clear();
        cmd = "\n";
        port->write(cmd.toLatin1());
    } else if(!cmd.compare("exit")) {
        this->close();
    } else
    {
        cmd += "\n";
        port->write(cmd.toLatin1());
    }
    cmd = "";
}

void MainWindow::onPortNameChanged(const QString & /*name*/)
{
    if (port->isOpen()) {
        port->close();
    }
}

void MainWindow::onBaudRateChanged(int idx)
{
    port->setBaudRate((BaudRateType)ui->cbxBaudrate->itemData(idx).toInt());
}

void MainWindow::onOpenCloseButtonClicked()
{
    if (!port->isOpen()) {
        port->setPortName(ui->cbxPort->currentText());
        isConnect = port->open(QIODevice::ReadWrite);
        if(isConnect)
        {
            led->turnOn();
            ui->btnOpenClose->setText("Close");
            console->insertPlainText("\n------ Port Data Opened -----------\n");
            ui->tabModulation->setEnabled(true);
            ui->tabDemodulation->setEnabled(true);
            update();
        } else{
            console->insertPlainText("Cannot open Device! Please check ...\n");
            ui->tabModulation->setEnabled(false);
            ui->tabDemodulation->setEnabled(false);
        }
    }
    else {
        port->close();
        isConnect = false;
        led->turnOff();
        ui->btnOpenClose->setText("Open");
        console->insertPlainText("\n------- Port Close --------\n");
        ui->tabModulation->setEnabled(false);
        ui->tabDemodulation->setEnabled(false);
    }

    //If using polling mode, we need a QTimer
    if (port->isOpen() && port->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();
}

void MainWindow::onPortAddedOrRemoved()
{
    QString current = ui->cbxPort->currentText();

    ui->cbxPort->blockSignals(true);
    ui->cbxPort->clear();
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->cbxPort->addItem(info.portName);

    ui->cbxPort->setCurrentIndex(ui->cbxPort->findText(current));

    ui->cbxPort->blockSignals(false);
}

void MainWindow::onExcuteModulationSettings()
{
    //Set Mod
   /*Set Data Rate*/
    QString LENH1;
    QString _cmd1=ui->lineModuDataRate->text();
    LENH1="m500ctl md mdr "+_cmd1+"\n";
    QByteArray ba1 = LENH1.toLatin1();
    const char *c_str1 = ba1.data();
    port->write(c_str1);

    /*Set Modulation Mode*/
    QString LENH2;
    int _intcmd=CheckcmbModulation_1(ui->cbxModuMode->currentText());
    switch(_intcmd){
    case 0: LENH2="m500ctl 0 mdm 0\n";break;
    case 1: LENH2="m500ctl 0 mdm 1\n";break;
    case 2: LENH2="m500ctl 0 mdm 2\n";break;
    case 3: LENH2="m500ctl 0 mdm 3\n";break;
    case 4: LENH2="m500ctl 0 mdm 4\n";break;
    case 6: LENH2="m500ctl 0 mdm 6\n";break;
    default: break;
    }
    QByteArray ba2=LENH2.toLatin1();
    const char *c_str2=ba2.data();
    port->write(c_str2);

    /*Set FEC Type*/

    QString LENH3;

    int _cmd3=Check_FEC_Type(ui->cbxModuFecType->currentText());
    switch(_cmd3){
        case 0: LENH3="m500ctl 0 mdf 0\n";break;
        case 1: LENH3="m500ctl 0 mdf 1\n";break;
        case 2: LENH3="m500ctl 0 mdf 2\n";break;
        case 4: LENH3="m500ctl 0 mdf 4\n";break;
        case 5: LENH3="m500ctl 0 mdf 5\n";break;
        case 6: LENH3="m500ctl 0 mdf 6\n";break;
    }
    QByteArray ba3=LENH3.toLatin1();
    const char *c_str3=ba3.data();
    port->write(c_str3);

    /*set FEC Option*/

    QString LENH4;

    int _cmd4=Check_FEC_Option(ui->cbxModuFecOption->currentText());
    switch(_cmd4){
        case 0: LENH4="m500ctl 0 mdo 0\n";break;
        case 1: LENH4="m500ctl 0 mdo 1\n";break;
        case 2: LENH4="m500ctl 0 mdo 2\n";break;
        case 3: LENH4="m500ctl 0 mdo 3\n";break;
        case 4: LENH4="m500ctl 0 mdo 4\n";break;
        case 5: LENH4="m500ctl 0 mdo 5\n";break;
        case 6: LENH4="m500ctl 0 mdo 6\n";break;
    }
    QByteArray ba4=LENH4.toLatin1();
    const char *c_str4=ba4.data();
    port->write(c_str4);

    /*set FEC rate*/
    QString LENH5;
    int _cmd5=Check_FEC_Rate(ui->cbxModuFecRate->currentText(),ui->cbxModuFecType->currentText(),ui->cbxModuFecOption->currentText());
    switch (_cmd5) {
    case 0: LENH5="m500ctl 0 mdc 0\n";break;
    case 1: LENH5="m500ctl 0 mdc 1\n";break;
    case 2: LENH5="m500ctl 0 mdc 2\n";break;
    case 3: LENH5="m500ctl 0 mdc 3\n";break;
    case 4: LENH5="m500ctl 0 mdc 4\n";break;
    case 5: LENH5="m500ctl 0 mdc 5\n";break;
    case 6: LENH5="m500ctl 0 mdc 6\n";break;
    case 7: LENH5="m500ctl 0 mdc 7\n";break;
    case 8: LENH5="m500ctl 0 mdc 8\n";break;
    }
    QByteArray ba5=LENH5.toLatin1();
    const char *c_str5=ba5.data();
    port->write(c_str5);

    /*Set Scramber*/
    QString LENH6;
    int _cmd6=Check_Scamber(ui->cbxModuScrambler->currentText());
    switch(_cmd6){
        case 0: LENH6="m500ctl 0 mds 0\n";break;
        case 1: LENH6="m500ctl 0 mds 1\n";break;
        case 2: LENH6="m500ctl 0 mds 2\n";break;
        case 3: LENH6="m500ctl 0 mds 3\n";break;
        case 4: LENH6="m500ctl 0 mds 4\n";break;
        case 5: LENH6="m500ctl 0 mds 5\n";break;
        case 6: LENH6="m500ctl 0 mds 6\n";break;
        case 7: LENH6="m500ctl 0 mds 7\n";break;
        case 8: LENH6="m500ctl 0 mds 8\n";break;
        case 9: LENH6="m500ctl 0 mds 9\n";break;
    }
    QByteArray ba6=LENH6.toLatin1();
    const char *c_str6=ba6.data();
    port->write(c_str6);

    /*set Clock source*/

    QString LENH7;
    int _cmd7=Check_Mod_Clock_Source(ui->cbxModuClockSource->currentText());
    switch(_cmd7){
        case 0: LENH7="m500ctl 0 mdk 0\n";break;
        case 1: LENH7="m500ctl 0 mdk 1\n";break;
        case 2: LENH7="m500ctl 0 mdk 2\n";break;
        case 3: LENH7="m500ctl 0 mdk 3\n";break;
    }
    QByteArray ba7=LENH7.toLatin1();
    const char *c_str7=ba7.data();
    port->write(c_str7);

    /*Set Reed Solomon Mode*/
    QString LENH8;
    QString n,d,k;

    int _cmd8=Check_Mode(ui->cbxModuRsMode->currentText());
    switch(_cmd8){
            case 0: {
                        LENH8="m500ctl 0 mdrs 0\n";break;
                        }
            case 1: {
                        LENH8="m500ctl 0 mdrs 1\n";break;
                        }
            case 2: {
                        LENH8="m500ctl 0 mdrs 2\n";break;
                        }
            case 3: {
                        LENH8="m500ctl 0 mdrs 3\n";break;
                        }
            case 4: {
                         n=ui->lineModuRsN->text();
                         k=ui->lineModuRsK->text();
                         d=ui->cbxModuRsDepth->currentText();
                         LENH8="m500ctl 0 mdrs 4\n m500ctl 0 mdrp "+n+","+k+","+d+"\n";
                         break;
                    }
            case 5: {LENH8="m500ctl 0 mdrs 5\n";break;}
    }
    QByteArray ba8=LENH8.toLatin1();
    const char *c_str8=ba8.data();
    port->write(c_str8);

    /*Set Spectrum*/
    QString LENH9;
    int _cmd9=Check_Dir(ui->cbxModuSpectrumDir->currentText());
    switch(_cmd9){
        case 0: LENH9="m500ctl 0 mis 0\n";break;
        case 1: LENH9="m500ctl 0 mis 1\n";break;
    }
    QByteArray ba9=LENH9.toLatin1();
    const char *c_str9=ba9.data();
    port->write(c_str9);

    /*Set Filter m500ctl 0 mifl 1*/
    QString LENH10;
    int _cmd10=Check_Filter(ui->cbxModuSpectrumFilter->currentText());
    switch(_cmd10){
        case 0: LENH10="m500ctl 0 mifl 0\n";break;
        case 1: LENH10="m500ctl 0 mifl 1\n";break;
    }
    QByteArray ba10=LENH10.toLatin1();
    const char *c_str10=ba10.data();
    port->write(c_str10);

    /*set Mute m500ctl 0 mit ?*/
    QString LENH11;
    int _cmd11=Check_Mute(ui->cbxModuMute->currentText());
    switch(_cmd11){
        case 0: LENH11="m500ctl 0 mit 0\n";break;
        case 1: LENH11="m500ctl 0 mit 1\n";break;
        case 2: LENH11="m500ctl 0 mit 2\n";break;
        case 3: LENH11="m500ctl 0 mit 3\n";break;
    }
    QByteArray ba11=LENH11.toLatin1();
    const char *c_str11=ba11.data();
    port->write(c_str11);

    /*Set Output*/
    QString LENH12;
    int _cmd12=Check_Output(ui->cbxModuOutput->currentText());
    switch(_cmd12){
    // cho nay rat la vo van: nguoc chieu,tim mai moi ra,haizzz
        case 0: LENH12="m500ctl mie 1\n";break;
        case 1: LENH12="m500ctl mie 0\n";break;
    }
    QByteArray ba12=LENH12.toLatin1();
    const char *c_str12=ba12.data();
    port->write(c_str12);

    /*set AUPC m500ctl 0 mip */
    QString LENH13;
    int _cmd13=Check_AUPC(ui->cbxModuAupc->currentText());
    switch(_cmd13){
        case 0: LENH13="m500ctl 0 mip 0\n";break;
        case 1: LENH13="m500ctl 0 mip 1\n";break;
        case 2: LENH13="m500ctl 0 mip 2\n";break;
    }
    QByteArray ba13=LENH13.toLatin1();
    const char *c_str13=ba13.data();
    port->write(c_str13);




    /*Set Frequency*/

    /*set CXR offset  m500ctl mio*/

    QString cxrofset=ui->lineModuCxrOffset->text();

    double _cxrofset=cxrofset.toDouble();
    _cxrofset=_cxrofset*1000;
    QString LENH17="m500ctl mio "+QString::number(_cxrofset)+"\n";
    QByteArray ba17=LENH17.toLatin1();
    const char *c_str17=ba17.data();
    port->write(c_str17);

    /*set IF*/
    bool _ok;
    QString _mif=ui->lineModuIf->text();
    int __mif=_mif.toInt(&_ok,10);
    __mif=__mif*1000000;
    QString LENH15="m500ctl mif "+QString::number(__mif)+"\n";
    QByteArray ba15=LENH15.toLatin1();
    const char *c_str15=ba15.data();
    port->write(c_str15);

    /*set Cxr lvlsss*/
    QString _cxrlvl=ui->lineModuCxrLvl->text();
    int __cxrlvl=_cxrlvl.toInt(&_ok,10);
    __cxrlvl=__cxrlvl*10;
    QString LENH16="m500ctl mil "+QString::number(__cxrlvl)+"\n";
    QByteArray ba16=LENH16.toLatin1();
    const char *c_str16=ba16.data();
    port->write(c_str16);

    QString LENH="m500ctl  mipc ?\n";
    QByteArray ba=LENH.toLatin1();
    const char *c_str=ba.data();
    port->write(c_str);

    /*set gia tri AUPC m500ctl 0 mipc 50,40,-350*/
    QString _ebno=ui->lineModuEbn0->text();
    double x=_ebno.toDouble();x=x*10;
    QString _minlv=ui->lineModuMinLvl->text();
    double y=_minlv.toDouble();y=y*10;
    QString _maxlv=ui->lineModuMaxLvl->text();
    double z=_maxlv.toDouble();z=z*10;

    QString LENH14;
    LENH14="m500ctl mipc "+QString::number(x)+","+QString::number(z)+","+QString::number(y)+"\n";
    QByteArray ba14=LENH14.toLatin1();
    const char *c_str14=ba14.data();
    port->write(c_str14);
}

void MainWindow::onExcuteDemodulationSettings()
{
    //Set DeMod
    /*Set Data Rate*/
     QString LENH1;
     QString _cmd1=ui->lineDemodDataRate->text();
     LENH1="mq ddr "+_cmd1+"\n";
     QByteArray ba1 = LENH1.toLatin1();
     const char *c_str1 = ba1.data();
     port->write(c_str1);

     /*Set Modulation Mode*/
     QString LENH2;
     int _intcmd=CheckcmbModulation_1(ui->cbxDemodMode->currentText());
     switch(_intcmd){
     case 0: LENH2="m500ctl 0 ddm 0\n";break;
     case 1: LENH2="m500ctl 0 ddm 1\n";break;
     case 2: LENH2="m500ctl 0 ddm 2\n";break;
     case 3: LENH2="m500ctl 0 ddm 3\n";break;
     case 4: LENH2="m500ctl 0 ddm 4\n";break;
     case 6: LENH2="m500ctl 0 ddm 6\n";break;
     default: break;
     }
     QByteArray ba2=LENH2.toLatin1();
     const char *c_str2=ba2.data();
     port->write(c_str2);

     /*Set FEC Type*/

     QString LENH3;

     int _cmd3=Check_FEC_Type(ui->cbxDemodFecType->currentText());
     switch(_cmd3){
         case 0: LENH3="m500ctl 0 ddf 0\n";break;
         case 1: LENH3="m500ctl 0 ddf 1\n";break;
         case 2: LENH3="m500ctl 0 ddf 2\n";break;
         case 4: LENH3="m500ctl 0 ddf 4\n";break;
         case 5: LENH3="m500ctl 0 ddf 5\n";break;
         case 6: LENH3="m500ctl 0 ddf 6\n";break;
     }
     QByteArray ba3=LENH3.toLatin1();
     const char *c_str3=ba3.data();
     port->write(c_str3);

     /*set FEC Option*/

     QString LENH4;

     int _cmd4=Check_FEC_Option(ui->cbxDemodFecOption->currentText());
     switch(_cmd4){
         case 0: LENH4="m500ctl 0 ddo 0\n";break;
         case 1: LENH4="m500ctl 0 ddo 1\n";break;
         case 2: LENH4="m500ctl 0 ddo 2\n";break;
         case 3: LENH4="m500ctl 0 ddo 3\n";break;
         case 4: LENH4="m500ctl 0 ddo 4\n";break;
         case 5: LENH4="m500ctl 0 ddo 5\n";break;
         case 6: LENH4="m500ctl 0 ddo 6\n";break;
     }
     QByteArray ba4=LENH4.toLatin1();
     const char *c_str4=ba4.data();
     port->write(c_str4);

     /*set FEC rate*/
     QString LENH5;
     int _cmd5=Check_FEC_Rate(ui->cbxDemodFecRate->currentText(),ui->cbxDemodFecType->currentText(),ui->cbxDemodFecOption->currentText());
     switch (_cmd5) {
     case 0: LENH5="m500ctl 0 ddc 0\n";break;
     case 1: LENH5="m500ctl 0 ddc 1\n";break;
     case 2: LENH5="m500ctl 0 ddc 2\n";break;
     case 3: LENH5="m500ctl 0 ddc 3\n";break;
     case 4: LENH5="m500ctl 0 ddc 4\n";break;
     case 5: LENH5="m500ctl 0 ddc 5\n";break;
     case 6: LENH5="m500ctl 0 ddc 6\n";break;
     case 7: LENH5="m500ctl 0 ddc 7\n";break;
     case 8: LENH5="m500ctl 0 ddc 8\n";break;
     }
     QByteArray ba5=LENH5.toLatin1();
     const char *c_str5=ba5.data();
     port->write(c_str5);

     /*Set Scramber*/
     QString LENH6;
     int _cmd6=Check_Scamber(ui->cbxDemodScrambler->currentText());
     switch(_cmd6){
         case 0: LENH6="m500ctl 0 dds 0\n";break;
         case 1: LENH6="m500ctl 0 dds 1\n";break;
         case 2: LENH6="m500ctl 0 dds 2\n";break;
         case 3: LENH6="m500ctl 0 dds 3\n";break;
         case 4: LENH6="m500ctl 0 dds 4\n";break;
         case 5: LENH6="m500ctl 0 dds 5\n";break;
         case 6: LENH6="m500ctl 0 dds 6\n";break;
         case 7: LENH6="m500ctl 0 dds 7\n";break;
         case 8: LENH6="m500ctl 0 dds 8\n";break;
         case 9: LENH6="m500ctl 0 dds 9\n";break;
     }
     QByteArray ba6=LENH6.toLatin1();
     const char *c_str6=ba6.data();
     port->write(c_str6);

     /*set Clock source*/

     QString LENH7;
     int _cmd7=Check_Demod_Clock_Source(ui->cbxDemodClockSource->currentText());
     switch(_cmd7){
         case 0: LENH7="m500ctl 0 ddk 0\n";break;
         case 1: LENH7="m500ctl 0 ddk 1\n";break;
         case 2: LENH7="m500ctl 0 ddk 2\n";break;
         case 3: LENH7="m500ctl 0 ddk 3\n";break;
     }
     QByteArray ba7=LENH7.toLatin1();
     const char *c_str7=ba7.data();
     port->write(c_str7);

     /*Set Reed Solomon Mode*/
     QString LENH8;
     QString n,d,k;

     int _cmd8=Check_Mode(ui->cbxDemodRsMode->currentText());
     switch(_cmd8){
             case 0: {
                         LENH8="m500ctl 0 ddrs 0\n";break;
                         }
             case 1: {
                         LENH8="m500ctl 0 ddrs 1\n";break;
                         }
             case 2: {
                         LENH8="m500ctl 0 ddrs 2\n";break;
                         }
             case 3: {
                         LENH8="m500ctl 0 ddrs 3\n";break;
                         }
             case 4: {
                          n=ui->lineDemodRsN->text();
                          k=ui->lineDemodRsK->text();
                          d=ui->cbxDemodDepth->currentText();
                          LENH8="m500ctl 0 ddrs 4\n m500ctl 0 ddrp "+n+","+k+","+d+"\n";
                          break;
                     }
             case 5: {LENH8="m500ctl 0 ddrs 5\n";break;}
     }
     QByteArray ba8=LENH8.toLatin1();
     const char *c_str8=ba8.data();
     port->write(c_str8);

     /*set IF*/
     /*Lenh ktra
     QString LENH="m500ctl dif ?\n";
     QByteArray ba=LENH.toLatin1();
     const char *c_str=ba.data();
     port->write(c_str);*/

     bool _ok;
     QString _mif=ui->lineDemodIfFrequency->text();
     int __mif=_mif.toInt(&_ok,10);
     __mif=__mif*1000000;
     QString LENH15="m500ctl dif "+QString::number(__mif)+"\n";
     QByteArray ba15=LENH15.toLatin1();
     const char *c_str15=ba15.data();
     port->write(c_str15);

     /*Set Spectrum*/
     QString LENH9;
     int _cmd9=Check_Dir(ui->cbxDemodDir->currentText());
     switch(_cmd9){
         case 0: LENH9="m500ctl 0 dis 0\n";break;
         case 1: LENH9="m500ctl 0 dis 1\n";break;
     }
     QByteArray ba9=LENH9.toLatin1();
     const char *c_str9=ba9.data();
     port->write(c_str9);

     /*Set Filter m500ctl 0 difl 1*/
     QString LENH10;
     int _cmd10=Check_Filter(ui->cbxDemodFilter->currentText());
     switch(_cmd10){
         case 0: LENH10="m500ctl 0 difl 0\n";break;
         case 1: LENH10="m500ctl 0 difl 1\n";break;
     }
     QByteArray ba10=LENH10.toLatin1();
     const char *c_str10=ba10.data();
     port->write(c_str10);

     /*Set Sweep Range*/
     QString LENH11;
     QString _sweep=ui->lineDemodSweep->text();
     double __sweep=_sweep.toDouble();
     __sweep=__sweep*1000;
     LENH11="m500ctl dia "+QString::number(__sweep)+"\n";
     QByteArray ba11=LENH11.toLatin1();
     const char *c_str11=ba11.data();
     port->write(c_str11);

     /*Set Cxr Lvl*/
     QString _cxr=ui->lineDemodCxr->text();
     double __cxr=_cxr.toDouble();
     QString LENH12="m500ctl dil "+QString::number(__cxr)+"\n";
     QByteArray ba12=LENH12.toLatin1();
     const char *c_str12=ba12.data();
     port->write(c_str12);

     /*Set Eb/No*/

     QString _ebno=ui->lineDemodEbn0->text();
     double __ebno=_ebno.toDouble();
     QString LENH13="m500ctl die "+QString::number(__ebno)+"\n";
     QByteArray ba13=LENH13.toLatin1();
     const char *c_str13=ba13.data();
     port->write(c_str13);
}

void MainWindow::on_btnRefresh_clicked()
{
    ui->cbxPort->clear();
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->cbxPort->addItem(info.portName);
}

void MainWindow::on_btnExit_clicked()
{
    this->close();
}

void MainWindow::on_cbxModuFecType_currentIndexChanged(const QString &arg1)
{
    if(arg1=="UnCoded"){
        while(ui->cbxModuFecOption->count()>0){
            ui->cbxModuFecOption->removeItem(0);
        }
    ui->cbxModuFecOption->addItem("None");
   }
    else if(arg1=="Viterbi"){
        while(ui->cbxModuFecOption->count()>0){
            ui->cbxModuFecOption->removeItem(0);
        }
        ui->cbxModuFecOption->addItem("Normal");
        ui->cbxModuFecOption->addItem("Swap C0/C1");
        ui->cbxModuFecOption->addItem("CT");
    }
    else if(arg1=="TCM"){
        while(ui->cbxModuFecOption->count()>0){
            ui->cbxModuFecOption->removeItem(0);
        }

        ui->cbxModuFecOption->addItem("Normal");
    }
    else if(arg1=="TPC"){
        while(ui->cbxModuFecOption->count()>0){
            ui->cbxModuFecOption->removeItem(0);
        }

        ui->cbxModuFecOption->addItem("Advanced");
        ui->cbxModuFecOption->addItem("M5 Full");
        ui->cbxModuFecOption->addItem("M5 Short");
        ui->cbxModuFecOption->addItem("M5 Legacy");
        ui->cbxModuFecOption->addItem("CT");
    }
    else if(arg1=="LDPC"){
        while(ui->cbxModuFecOption->count()>0){
            ui->cbxModuFecOption->removeItem(0);
        }

        ui->cbxModuFecOption->addItem("256 Block");
        ui->cbxModuFecOption->addItem("512 Block");
        ui->cbxModuFecOption->addItem("1K Block");
        ui->cbxModuFecOption->addItem("2K Block");
        ui->cbxModuFecOption->addItem("4K Block");
        ui->cbxModuFecOption->addItem("8K Block");
        ui->cbxModuFecOption->addItem("16K Block");

    }
    else if(arg1=="S-Tec"){
        while(ui->cbxModuFecOption->count()>0){
            ui->cbxModuFecOption->removeItem(0);
        }

        ui->cbxModuFecOption->addItem("Large");
        ui->cbxModuFecOption->addItem("Medium");
        ui->cbxModuFecOption->addItem("Small");
        ui->cbxModuFecOption->addItem("XLarge");
    }
}

void MainWindow::on_cbxDemodFecType_currentIndexChanged(const QString &arg1)
{
    if(arg1=="UnCoded"){
        while(ui->cbxDemodFecOption->count()>0){
            ui->cbxDemodFecOption->removeItem(0);
        }
    ui->cbxDemodFecOption->addItem("None");
   }
    else if(arg1=="Viterbi"){
        while(ui->cbxDemodFecOption->count()>0){
            ui->cbxDemodFecOption->removeItem(0);
        }
        ui->cbxDemodFecOption->addItem("Normal");
        ui->cbxDemodFecOption->addItem("Swap C0/C1");
        ui->cbxDemodFecOption->addItem("CT");
    }
    else if(arg1=="TCM"){
        while(ui->cbxDemodFecOption->count()>0){
            ui->cbxDemodFecOption->removeItem(0);
        }

        ui->cbxDemodFecOption->addItem("Normal");
    }
    else if(arg1=="TPC"){
        while(ui->cbxDemodFecOption->count()>0){
            ui->cbxDemodFecOption->removeItem(0);
        }

        ui->cbxDemodFecOption->addItem("Advanced");
        ui->cbxDemodFecOption->addItem("M5 Full");
        ui->cbxDemodFecOption->addItem("M5 Short");
        ui->cbxDemodFecOption->addItem("M5 Legacy");
        ui->cbxDemodFecOption->addItem("CT");
    }
    else if(arg1=="LDPC"){
        while(ui->cbxDemodFecOption->count()>0){
            ui->cbxDemodFecOption->removeItem(0);
        }

        ui->cbxDemodFecOption->addItem("256 Block");
        ui->cbxDemodFecOption->addItem("512 Block");
        ui->cbxDemodFecOption->addItem("1K Block");
        ui->cbxDemodFecOption->addItem("2K Block");
        ui->cbxDemodFecOption->addItem("4K Block");
        ui->cbxDemodFecOption->addItem("8K Block");
        ui->cbxDemodFecOption->addItem("16K Block");

    }
    else if(arg1=="S-Tec"){
        while(ui->cbxDemodFecOption->count()>0){
            ui->cbxDemodFecOption->removeItem(0);
        }

        ui->cbxDemodFecOption->addItem("Large");
        ui->cbxDemodFecOption->addItem("Medium");
        ui->cbxDemodFecOption->addItem("Small");
        ui->cbxDemodFecOption->addItem("XLarge");
    }
}

void MainWindow::on_cbxDemodRsMode_currentIndexChanged(const QString &arg1)
{
    if(arg1=="Disabled"|| arg1=="IESS-308" || arg1=="IESS-309" || arg1=="IESS-310" ||arg1=="CT 220/200")
    {
        ui->lineDemodRsN->clear();
        ui->lineDemodRsK->clear();
        ui->lineDemodRsN->setReadOnly(true);
        ui->lineDemodRsK->setReadOnly(true);
        ui->cbxDemodDepth->setDisabled(true);
    }
    else {
        ui->cbxDemodDepth->setDisabled(false);
        ui->lineDemodRsN->setReadOnly(false);
        ui->lineDemodRsK->setReadOnly(false);
    }
}

void MainWindow::on_cbxModuRsMode_currentIndexChanged(const QString &arg1)
{
    if(arg1=="Disabled"|| arg1=="IESS-308" || arg1=="IESS-309" || arg1=="IESS-310" ||arg1=="CT 220/200")
    {
        ui->lineModuRsN->clear();
        ui->lineModuRsK->clear();
        ui->lineModuRsN->setReadOnly(true);
        ui->lineModuRsK->setReadOnly(true);
        ui->cbxModuRsDepth->setDisabled(true);
    }
    else {
        ui->cbxModuRsDepth->setDisabled(false);
        ui->lineModuRsN->setReadOnly(false);
        ui->lineModuRsK->setReadOnly(false);
    }
}

void MainWindow::on_cbxModuFecOption_currentIndexChanged(const QString &arg1)
{
    QString Type =ui->cbxModuFecType->currentText();
    if(Type == "UnCoded"){
        while(ui->cbxModuFecRate->count()>0){
            ui->cbxModuFecRate->removeItem(0);
        }
        if(arg1 == "None")
        ui->cbxModuFecRate->addItem("N/A");
   }
    else if(Type == "Viterbi"){
        while(ui->cbxModuFecRate->count()>0){
            ui->cbxModuFecRate->removeItem(0);
        }
        if(arg1 == "Normal"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("5/6");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "Swap C0/C1"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("5/6");
            ui->cbxModuFecRate->addItem("7/8");}
        else if(arg1 == "CT"){
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("7/8");
            }

   }
    else if( Type == "TCM"){
        while(ui->cbxModuFecRate->count()>0){
            ui->cbxModuFecRate->removeItem(0);
        }
        if(arg1 == "Normal"){
            ui->cbxModuFecRate->addItem("2/3");

        }
   }
    else if( Type == "TPC"){
        while(ui->cbxModuFecRate->count()>0){
            ui->cbxModuFecRate->removeItem(0);
        }
        if(arg1 == "Advanced"){
            ui->cbxModuFecRate->addItem("0.453-16K");
            ui->cbxModuFecRate->addItem("1/2-16K");
            ui->cbxModuFecRate->addItem("1/2-4K");
            ui->cbxModuFecRate->addItem("3/4-16K");
            ui->cbxModuFecRate->addItem("3/4-4K");
            ui->cbxModuFecRate->addItem("7/8-16K");
            ui->cbxModuFecRate->addItem("7/8-4K");
            ui->cbxModuFecRate->addItem("0.922-16K");
            ui->cbxModuFecRate->addItem("0.950-4K");
        }
        else if(arg1 == "M5 Full"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "M5 Short"){
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "M5 Legacy"){
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "CT"){
            ui->cbxModuFecRate->addItem("5/16");
            ui->cbxModuFecRate->addItem("21/44");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("0.95");
        }
   }
    else if(Type == "LDPC"){
        while(ui->cbxModuFecRate->count()>0){
            ui->cbxModuFecRate->removeItem(0);
        }
        if(arg1 == "256 Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
        else if(arg1 == "512 Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
        else if(arg1 == "1K Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
        else if(arg1 == "2K Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
        else if(arg1 == "4K Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
        else if(arg1 == "8K Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
        else if(arg1 == "16K Block"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("2/3");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("14/17");
            ui->cbxModuFecRate->addItem("7/8");
            ui->cbxModuFecRate->addItem("10/11");
            ui->cbxModuFecRate->addItem("16/17");
        }
    }
    else if(Type == "S-Tec"){
        while(ui->cbxModuFecRate->count()>0){
            ui->cbxModuFecRate->removeItem(0);}
        if(arg1 == "Large"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/5");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("4/5");
            ui->cbxModuFecRate->addItem("5/6");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "Medium"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/5");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("4/5");
            ui->cbxModuFecRate->addItem("5/6");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "Small"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/5");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("4/5");
            ui->cbxModuFecRate->addItem("5/6");
            ui->cbxModuFecRate->addItem("7/8");
        }
        else if(arg1 == "XLarge"){
            ui->cbxModuFecRate->addItem("1/2");
            ui->cbxModuFecRate->addItem("3/5");
            ui->cbxModuFecRate->addItem("3/4");
            ui->cbxModuFecRate->addItem("4/5");
            ui->cbxModuFecRate->addItem("5/6");
            ui->cbxModuFecRate->addItem("7/8");
        }
    }
}

void MainWindow::on_cbxDemodFecOption_currentIndexChanged(const QString &arg1)
{
    QString Type = ui->cbxDemodFecType->currentText();
    if(Type == "UnCoded"){
        while(ui->cbxDemodFecRate->count()>0){
            ui->cbxDemodFecRate->removeItem(0);
        }
        if(arg1 == "None")
        ui->cbxDemodFecRate->addItem("N/A");
   }
    else if(Type == "Viterbi"){
        while(ui->cbxDemodFecRate->count()>0){
            ui->cbxDemodFecRate->removeItem(0);
        }
        if(arg1 == "Normal"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("5/6");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "Swap C0/C1"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("5/6");
            ui->cbxDemodFecRate->addItem("7/8");}
        else if(arg1 == "CT"){
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("7/8");
            }

   }
    else if(Type == "TCM"){
        while(ui->cbxDemodFecRate->count()>0){
            ui->cbxDemodFecRate->removeItem(0);
        }
        if(arg1 == "Normal"){
            ui->cbxDemodFecRate->addItem("2/3");

        }
   }
    else if(Type == "TPC"){
        while(ui->cbxDemodFecRate->count()>0){
            ui->cbxDemodFecRate->removeItem(0);
        }
        if(arg1 == "Advanced"){
            ui->cbxDemodFecRate->addItem("0.453-16K");
            ui->cbxDemodFecRate->addItem("1/2-16K");
            ui->cbxDemodFecRate->addItem("1/2-4K");
            ui->cbxDemodFecRate->addItem("3/4-16K");
            ui->cbxDemodFecRate->addItem("3/4-4K");
            ui->cbxDemodFecRate->addItem("7/8-16K");
            ui->cbxDemodFecRate->addItem("7/8-4K");
            ui->cbxDemodFecRate->addItem("0.922-16K");
            ui->cbxDemodFecRate->addItem("0.950-4K");
        }
        else if(arg1 == "M5 Full"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "M5 Short"){
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "M5 Legacy"){
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "CT"){
            ui->cbxDemodFecRate->addItem("5/16");
            ui->cbxDemodFecRate->addItem("21/44");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("0.95");
        }
   }
    else if(Type == "LDPC"){
        while(ui->cbxDemodFecRate->count()>0){
            ui->cbxDemodFecRate->removeItem(0);
        }
        if(arg1 == "256 Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
        else if(arg1 == "512 Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
        else if(arg1 == "1K Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
        else if(arg1 == "2K Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
        else if(arg1 == "4K Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
        else if(arg1 == "8K Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
        else if(arg1 == "16K Block"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("2/3");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("14/17");
            ui->cbxDemodFecRate->addItem("7/8");
            ui->cbxDemodFecRate->addItem("10/11");
            ui->cbxDemodFecRate->addItem("16/17");
        }
    }
    else if(Type == "S-Tec"){
        while(ui->cbxDemodFecRate->count()>0){
            ui->cbxDemodFecRate->removeItem(0);}
        if(arg1 == "Large"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/5");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("4/5");
            ui->cbxDemodFecRate->addItem("5/6");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "Medium"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/5");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("4/5");
            ui->cbxDemodFecRate->addItem("5/6");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "Small"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/5");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("4/5");
            ui->cbxDemodFecRate->addItem("5/6");
            ui->cbxDemodFecRate->addItem("7/8");
        }
        else if(arg1 == "XLarge"){
            ui->cbxDemodFecRate->addItem("1/2");
            ui->cbxDemodFecRate->addItem("3/5");
            ui->cbxDemodFecRate->addItem("3/4");
            ui->cbxDemodFecRate->addItem("4/5");
            ui->cbxDemodFecRate->addItem("5/6");
            ui->cbxDemodFecRate->addItem("7/8");
        }
    }
}

void MainWindow::btnModulExcute_clicked()
{

    QString cmd = "m500ctl";
    // Set Data:

    // Set Data rate"
    QString data_rate = ui->lineModuDataRate->text();
    if(data_rate.compare(status_Modul.value(2)))
    {
    data_rate.replace(".","\0");
    cmd+=" mdr ";
    cmd+=data_rate;

    }

    //Set Modul Mode
    int mode = ui->cbxModuMode->currentIndex();
    if(QString::number(mode).compare(status_Modul.value(20)))
    {
    cmd+=" mdm ";
    cmd+= QString::number(mode);
//    cmd+=" mim ";
//    cmd+= QString::number(mode);


    }

    //Set FEC Type
     int type = ui->cbxModuFecType->currentIndex();
    if(QString::number(type).compare(status_Modul.value(4)))

    {
   
    cmd+=" mdf ";
    cmd+=QString::number(type);

}

    //Set FEC option
    int option = ui->cbxModuFecOption->currentIndex();
    if(QString::number(option).compare(status_Modul.value(6)))
    {
    cmd+= " mdo ";
    cmd+=QString::number(option);;

}

    //Set FEC Rate
    int rate = ui->cbxModuFecRate->currentIndex();
    if(QString::number(rate).compare(status_Modul.value(8)))
    {
    cmd+=" mdc ";
    cmd+=QString::number(rate);;

}

    // Set Scambler
    int scram = ui->cbxModuScrambler->currentIndex();
    if(QString::number(scram).compare(status_Modul.value(10)))
    {
    cmd += " mds ";
    cmd+=QString::number(scram);

}

    // Set Data clock source

    int clock = ui->cbxModuClockSource->currentIndex();
    if(QString::number(clock).compare(status_Modul.value(12)))
    {
    cmd+=" mdk ";
    cmd += QString::number(clock);

}
    // Set RS Mode
    int rs_mode = ui->cbxModuRsMode->currentIndex();
    if(QString::number(rs_mode).compare(status_Modul.value(14)))
    {
    cmd+=" mdrs ";
    cmd += QString::number(rs_mode);

}
    //Set RS n,k,depth
    QString n = ui->lineModuRsN->text();
    QString k= ui->lineModuRsK->text();
    QString depth = ui->cbxModuRsDepth->currentText();
    if((n.compare(status_Modul.value(16)))||(k.compare(status_Modul.value(17)))||(depth.compare(status_Modul.value(18))))
    {
        cmd+= " mdrp ";
        cmd+= n+","+k+","+depth;

    }


    //Set Frequency
    //Set IF
    QString If = ui->lineModuIf->text();
    if(If.compare(status_Modul.value(44)))
    {
    If.replace(".","\0");
    cmd+=" miif ";
    cmd+=If;

    }

    //Set Offset
    QString off = ui->lineModuCxrOffset->text();
    //qDebug()<<off;
    if(off.compare(status_Modul.value(24)))
    {
    off.replace(".","\0");
    cmd+=" mio ";
    cmd+=off;

    }

    //Set Level
    QString lvl = ui->lineModuCxrLvl->text();
    if(lvl.compare(status_Modul.value(26)))
    {
    lvl.replace(".","\0");
    cmd+=" mil ";
    cmd+=lvl;

    }

    //Set output
    int out = ui->cbxModuOutput->currentIndex();
    if(QString::number(out).compare(status_Modul.value(28)))
    {
    cmd+=" mie ";
    cmd += QString::number(out);

}
    //Set Spectrum
    int spec = ui->cbxModuSpectrumDir->currentIndex();
    if(QString::number(spec).compare(status_Modul.value(30)))
    {
    cmd+=" mis ";
    cmd += QString::number(spec);

}
    //Set Filter
    int filt = ui->cbxModuSpectrumFilter->currentIndex();
    if(QString::number(filt).compare(status_Modul.value(32)))
    {
    cmd+=" mifl ";
    cmd += QString::number(filt);

}


    //Set Mute
    int mute = ui->cbxModuMute->currentIndex();
    if(QString::number(mute).compare(status_Modul.value(34)))
    {
    cmd+=" mit ";
    cmd += QString::number(mute);\

}

    // Set AUPC
    int au = ui->cbxModuAupc->currentIndex();
    if(QString::number(au).compare(status_Modul.value(36)))
    {
    cmd+=" mip ";
    cmd += QString::number(au);


}

    QString EbNo = ui->lineModuEbn0->text();
    QString min = ui->lineModuMinLvl->text();
    QString max = ui->lineModuMaxLvl->text();
    if((EbNo.compare(status_Modul.value(38)))||(min.compare(status_Modul.value(40)))||(max.compare(status_Modul.value(42))))
    {
        cmd+= " mipc ";
    cmd+= EbNo.replace(".","\0")+","+min.replace(",","\0")+","+max.replace(",","\0");

}






    cmd+=" md mi\n";
    QByteArray ba2 = cmd.toLatin1();
    const char *c_str2 = ba2.data();
    port->write(c_str2);
//    sleep(3);
//    port->write("m500ctl md mi\n");


}


void MainWindow::btnDemodExcute_clicked()
{
    QString cmd = "m500ctl";
    // Set Data:

    // Set Data rate"
    QString data_rate = ui->lineDemodDataRate->text();
    if(data_rate.compare(status_Modul.value(50)))
    {
    data_rate.replace(".","\0");
    cmd+=" ddr ";
    cmd+=data_rate;

    }

    //Set Modul Mode
    int mode = ui->cbxDemodMode->currentIndex();
    if(QString::number(mode).compare(status_Modul.value(72)))
    {
    cmd+=" ddm ";
    cmd+= QString::number(mode);


    }

    //Set FEC Type
     int type = ui->cbxDemodFecType->currentIndex();
    if(QString::number(type).compare(status_Modul.value(52)))

    {

    cmd+=" ddf ";
    cmd+=QString::number(type);

}

    //Set FEC option
    int option = ui->cbxDemodFecOption->currentIndex();
    if(QString::number(option).compare(status_Modul.value(54)))
    {
    cmd+= " ddo ";
    cmd+=QString::number(option);;

}

    //Set FEC Rate
    int rate = ui->cbxDemodFecRate->currentIndex();
    if(QString::number(rate).compare(status_Modul.value(56)))
    {
    cmd+=" ddc ";
    cmd+=QString::number(rate);;

}

    // Set Scambler
    int scram = ui->cbxDemodScrambler->currentIndex();
    if(QString::number(scram).compare(status_Modul.value(58)))
    {
    cmd += " dds ";
    cmd+=QString::number(scram);

}

    // Set Data clock source

    int clock = ui->cbxDemodClockSource->currentIndex();
    if(QString::number(clock).compare(status_Modul.value(60)))
    {
    cmd+=" ddk ";
    cmd += QString::number(clock);

}
    // Set RS Mode
    int rs_mode = ui->cbxDemodRsMode->currentIndex();
    if(QString::number(rs_mode).compare(status_Modul.value(62)))
    {
    cmd+=" ddrs ";
    cmd += QString::number(rs_mode);

}
    //Set RS n,k,depth
    QString n = ui->lineDemodRsN->text();
    QString k= ui->lineDemodRsK->text();
    QString depth = ui->cbxDemodDepth->currentText();
    if((n.compare(status_Modul.value(64)))||(k.compare(status_Modul.value(65)))||(depth.compare(status_Modul.value(66))))
    {
        cmd+= " ddrp ";
        cmd+= n+","+k+","+depth;

    }


    //Set Frequency
    //Set IF
    QString If = ui->lineDemodIfFrequency->text();
    if(If.compare(status_Modul.value(90)))
    {
    If.replace(".","\0");
    cmd+=" diif ";
    cmd+=If;

    }

    //Set Sweep range
    QString off = ui->lineDemodSweep->text();
    if(off.compare(status_Modul.value(78)))
    {
    off.replace(".","\0");
    cmd+=" dia ";
    cmd+=off;

    }

    //Set Level
    QString lvl = ui->lineDemodCxr->text();
    if(lvl.compare(status_Modul.value(88)))
    {
    lvl.replace(".","\0");
    cmd+=" dil ";
    cmd+=lvl;

    }


    //Set Spectrum
    int spec = ui->cbxDemodDir->currentIndex();
    if(QString::number(spec).compare(status_Modul.value(82)))
    {
    cmd+=" dis ";
    cmd += QString::number(spec);

}
    //Set Filter
    int filt = ui->cbxDemodFilter->currentIndex();
    if(QString::number(filt).compare(status_Modul.value(84)))
    {
    cmd+=" difl ";
    cmd += QString::number(filt);

}




    QString EbNo = ui->lineDemodEbn0->text();
    if((EbNo.compare(status_Modul.value(86))))
    {
        cmd+= " die ";
    cmd+= EbNo.replace(".","\0");

}






    cmd+=" dd di\n";
    QByteArray ba2 = cmd.toLatin1();
    const char *c_str2 = ba2.data();
    port->write(c_str2);
}


void MainWindow::btnReset_clicked()
{
    QString cmd = "m500ctl md mi dd di\n";

    QByteArray ba1 = cmd.toLatin1();

    const char *c_str1 = ba1.data();

    port->write(c_str1);

     qDebug()<< "cmd:  "<< cmd;



}

QString MainWindow::check(QString s)
{
    QString out;
    if(Check_Output(s)!=-1)
    {
        out = QString::number(Check_Output(s));
        return out;
    }
    if(Check_Dir(s)!=-1)
    {
        out = QString::number(Check_Dir(s));
        return out;
    }
    if(Check_Filter(s)!=-1)
    {
        out = QString::number(Check_Filter(s));
        return out;
    }
    if(Check_AUPC(s)!=-1)
    {
        out = QString::number(Check_AUPC(s));
        return out;
    }
    return s;
}

void MainWindow::update()
{
    FileData file(DATA_PATH);
        QString line = file.searchByCode("mdr");
        QStringList lst = line.split(",");
        ui->lineModuDataRate->setText(lst.value(1));
        line = file.searchByCode("mim/mdm");
        lst=line.split(",");
        ui->cbxModuMode->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mdf");
        lst=line.split(",");
        ui->cbxModuFecType->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mdo");
        lst=line.split(",");
        ui->cbxModuFecOption->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mdc");
        lst=line.split(",");
        ui->cbxModuFecRate->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mds");
        lst=line.split(",");
        ui->cbxModuScrambler->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mdk");
        lst=line.split(",");
        ui->cbxModuClockSource->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("miif");
        lst=line.split(",");
        ui->lineModuIf->setText(lst.value(1));
        line = file.searchByCode("mio");
        lst=line.split(",");
        ui->lineModuCxrOffset->setText(lst.value(1));
        line = file.searchByCode("mil");
        lst=line.split(",");
        ui->lineModuCxrLvl->setText(lst.value(1));
        line = file.searchByCode("mdrs");
        lst=line.split(",");
        ui->cbxModuRsMode->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mdrp");
        lst=line.split(",");
        ui->lineModuRsN->setText(lst.value(1));
        ui->lineModuRsK->setText(lst.value(2));
        ui->cbxModuRsDepth->setCurrentIndex(Check_depth(lst.value(3)));
        line = file.searchByCode("mip");
        lst=line.split(",");
        ui->cbxModuAupc->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mipc1");
        lst=line.split(",");
        ui->lineModuEbn0->setText(lst.value(1));
        line = file.searchByCode("mipc2");
        lst=line.split(",");
        ui->lineModuMaxLvl->setText(lst.value(1));
        line = file.searchByCode("mipc3");
        lst=line.split(",");
        ui->lineModuMinLvl->setText(lst.value(1));
        line = file.searchByCode("mis");
        lst=line.split(",");
        ui->cbxModuSpectrumDir->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mifl");
        lst=line.split(",");
        ui->cbxModuSpectrumFilter->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mie");
        lst=line.split(",");
        ui->cbxModuOutput->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("mit");
        lst=line.split(",");
        ui->cbxModuMute->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("ddr");
        lst = line.split(",");
        ui->lineDemodDataRate->setText(lst.value(1));
        line = file.searchByCode("ddf");
        lst=line.split(",");
        ui->cbxDemodFecType->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("ddo");
        lst = line.split(",");
        ui->cbxDemodFecOption->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("ddc");
        lst = line.split(",");
        ui->cbxDemodFecRate->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("dds");
        lst = line.split(",");
        ui->cbxDemodScrambler->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("ddk");
        lst = line.split(",");
        ui->cbxDemodClockSource->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("diif");
        lst = line.split(",");
        ui->lineDemodIfFrequency->setText(lst.value(1));
        line = file.searchByCode("dia");
        lst = line.split(",");
        ui->lineDemodSweep->setText(lst.value(1));
        line = file.searchByCode("dil");
        lst = line.split(",");
        ui->lineDemodCxr->setText(lst.value(1));
        line = file.searchByCode("die");
        lst = line.split(",");
        ui->lineDemodEbn0->setText(lst.value(1));
        line = file.searchByCode("dis");
        lst = line.split(",");
        ui->cbxDemodDir->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("difl");
        lst = line.split(",");
        ui->cbxDemodFilter->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("ddrs");
        lst = line.split(",");
        ui->cbxDemodRsMode->setCurrentIndex(lst.value(1).toInt());
        line = file.searchByCode("ddrp");
        lst=line.split(",");
        ui->lineDemodRsN->setText(lst.value(1));
        ui->lineDemodRsK->setText(lst.value(2));
        ui->cbxDemodDepth->setCurrentIndex(Check_depth(lst.value(3)));
        line = file.searchByCode("dim/ddm");
        lst = line.split(",");
        ui->cbxDemodMode->setCurrentIndex(lst.value(1).toInt());

        get_Status();
    
}
void MainWindow::get_Status()
{
    status_Modul.clear();
    FileData file(DATA_PATH);
   int i;
   QString line;
   QStringList list;
   for(i=0;i<45;i++)
   {
       line = file.readLine(i);
       list = line.split(",");
       status_Modul.append(list);

   }
   qDebug()<<status_Modul<<endl;

}
