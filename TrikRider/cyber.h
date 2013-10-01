/*
Copyright (c) 2013, Bolotov Sergey
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Saint-Petersburg State University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Bolotov Sergey BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
