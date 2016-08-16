/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Fri Jul 22 17:00:00 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      26,   11,   11,   11, 0x08,
      42,   37,   11,   11, 0x08,
      64,   11,   11,   11, 0x08,
      83,   78,   11,   11, 0x08,
     114,  110,   11,   11, 0x08,
     137,   11,   11,   11, 0x08,
     164,   11,   11,   11, 0x08,
     187,   11,   11,   11, 0x08,
     216,   11,   11,   11, 0x08,
     247,   11,   11,   11, 0x08,
     271,   11,   11,   11, 0x08,
     292,   11,   11,   11, 0x08,
     317,   11,   11,   11, 0x08,
     342,   11,   11,   11, 0x08,
     361,   11,   11,   11, 0x08,
     375,  370,   11,   11, 0x08,
     422,  370,   11,   11, 0x08,
     470,  370,   11,   11, 0x08,
     517,  370,   11,   11, 0x08,
     563,  370,   11,   11, 0x08,
     612,  370,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0readyUpdate()\0readData()\0"
    "data\0writeData(QByteArray)\0sendCommand()\0"
    "name\0onPortNameChanged(QString)\0idx\0"
    "onBaudRateChanged(int)\0"
    "onOpenCloseButtonClicked()\0"
    "onPortAddedOrRemoved()\0"
    "onExcuteModulationSettings()\0"
    "onExcuteDemodulationSettings()\0"
    "on_btnRefresh_clicked()\0on_btnExit_clicked()\0"
    "btnModulExcute_clicked()\0"
    "btnDemodExcute_clicked()\0btnReset_clicked()\0"
    "update()\0arg1\0"
    "on_cbxModuFecType_currentIndexChanged(QString)\0"
    "on_cbxDemodFecType_currentIndexChanged(QString)\0"
    "on_cbxDemodRsMode_currentIndexChanged(QString)\0"
    "on_cbxModuRsMode_currentIndexChanged(QString)\0"
    "on_cbxModuFecOption_currentIndexChanged(QString)\0"
    "on_cbxDemodFecOption_currentIndexChanged(QString)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readyUpdate(); break;
        case 1: readData(); break;
        case 2: writeData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: sendCommand(); break;
        case 4: onPortNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: onBaudRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: onOpenCloseButtonClicked(); break;
        case 7: onPortAddedOrRemoved(); break;
        case 8: onExcuteModulationSettings(); break;
        case 9: onExcuteDemodulationSettings(); break;
        case 10: on_btnRefresh_clicked(); break;
        case 11: on_btnExit_clicked(); break;
        case 12: btnModulExcute_clicked(); break;
        case 13: btnDemodExcute_clicked(); break;
        case 14: btnReset_clicked(); break;
        case 15: update(); break;
        case 16: on_cbxModuFecType_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: on_cbxDemodFecType_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: on_cbxDemodRsMode_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: on_cbxModuRsMode_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: on_cbxModuFecOption_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: on_cbxDemodFecOption_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::readyUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
