/*
Copyright 2013 Bolotov Sergey

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#ifndef CYBER_H
#define CYBER_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QTcpServer>
#include <QSocketNotifier>
#include <QTcpSocket>
#include <QSettings>
#include <QTimer>
#include <qmath.h>
#include <QStringList>
#include <QKeyEvent>

#if QT_VERSION >= 0x050000  // wtf?
#include <QCoreApplication>
#else
#include <QtGui/QApplication>
#endif
//#include <QQuaternion>
//#include <QtSql/QSql>

//#include "wheel.h"
#include "MotorCtrl.hpp"
#include "gyroscope.h"

#include <cmath>
#include <stdio.h>

const qreal degPerSecMCoef = 1;     //  Not used
const quint16 checksPerSecond = 25; //  Было 10. Работает наконец.
const qreal kalmanCoef = 1;
const qreal Pi = 3.1415926535;      //  Needed for some calc's

enum controlType {ANDROID_CONTROL, AUTO_MODE, PC_CONTROL, OTHER};
//  Auto mode - some mythical type of controls. So as the Other one

//  Garbage struct accel
struct accelerometer
{
    accelerometer():x(), y(), z(){}
    qreal x;
    qreal y;
    qreal z;
};

struct vector
{
    vector():x(), y(){}
    qreal x, y;
};

struct matrix
{
    qreal _11;
    qreal _12;
    qreal _21;
    qreal _22;
};

vector operator*(matrix a, vector b);
vector operator*(qreal a, vector b);
inline vector operator*(vector a, qreal b) {return b * a;}
vector operator+(vector a, vector b);
//void operator=(vector a, vector b);
inline qreal operator*(vector a, vector b) {return a.x * b.x + a.y * b.y;}
vector normalize (vector a);
matrix setAngle(qreal radAngle);
vector setVAngle(qreal radAngle);

class Cyber : public QObject
{
    Q_OBJECT
public:
    explicit Cyber(QObject *parent = 0);
    ~Cyber();
    void startOMNI ();

private:
    vector * guide;                 //  Array of guiding vectors
    vector position;                //  Absolute position in the world
    vector direction;               //  Absolute direction
    vector moving;
//    vector acceleration;            //  Absolute acceleration
    qreal angVelocity;
    qreal currRad;                  //  Current angle in radians
    qreal leftRad;                  //  Left radians to turn
    qreal integrand;                //  Summary angle of Z tilt (tilt * time)
    qreal correction;               //  Correction coefficient (in radians)
    quint8 wheelSize;               //  Number of wheels on omni
    quint8 speed;                   //  Moving or not
    quint32 count;                  //  Counter
    controlType controlMode;        //  How device is controlled

    QSettings * settings;           //  Settings file
    QList<Motor *> * wheels;        //  Wheels.
    I2cConnection * connection;
    QTimer * mainTimer;             //  Check position timer
    QTcpServer * conServer;
    QTcpSocket * conSocket;
    QString * buffer;

    Gyroscope * gyro;

    gyroPos previous;
    gyroPos angles;                 //  Current gyro tilt
    gyroPos absolute;               //  Filtered gyro pos (x axis is forward, z axis is -g)

    void stop(bool );               //  Stop
    void initialiseMove();          //  Main moving func
    void calibrate();               //  Calibrating function
    void loadFromSaved();           //  Load saved settings
    void setSettings();
    void setWheels();
    void test();
    void andrControl();
    void pcControl();
    void autoControl();
    void parseSignalAndroid(QStringList );
    qint8 round10(qreal );

signals:
    
public slots:

private slots:
    void keyPressEvent(QKeyEvent *);
    void checkPosition();
    void moveVectorSlot();
    void calibrateSlot();
    void tcpDisconnected();
    void setConnection();
    void readTcp();

};

#endif // CYBER_H
