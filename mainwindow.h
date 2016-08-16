#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "console.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "hled.h"
#include <QFile>
#include "define.h"
#include "filedata.h"
#include <QStringList>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
signals:
    void readyUpdate();
public:
    QString check(QString);
    void get_Status();

private slots:
    void readData();
    void writeData(const QByteArray &data);
    void sendCommand();
    void onPortNameChanged(const QString &name);
    void onBaudRateChanged(int idx);
    void onOpenCloseButtonClicked();
    void onPortAddedOrRemoved();
    void onExcuteModulationSettings();
    void onExcuteDemodulationSettings();
    
    void on_btnRefresh_clicked();
    void on_btnExit_clicked();
     void btnModulExcute_clicked();
     void btnDemodExcute_clicked();
    void btnReset_clicked();
    void update();


    void on_cbxModuFecType_currentIndexChanged(const QString &arg1);
    void on_cbxDemodFecType_currentIndexChanged(const QString &arg1);
    void on_cbxDemodRsMode_currentIndexChanged(const QString &arg1);
    void on_cbxModuRsMode_currentIndexChanged(const QString &arg1);
    void on_cbxModuFecOption_currentIndexChanged(const QString &arg1);
    void on_cbxDemodFecOption_currentIndexChanged(const QString &arg1);


private:
    Ui::MainWindow *ui;
    Console *console;
    QTimer *timer;
    QextSerialPort *port;
    QextSerialEnumerator *enumerator;
    QString cmd;
    HLed *led;
    bool isConnect;
    QStringList status_Modul;
    QString buffer_data;


};

#endif // MAINWINDOW_H
