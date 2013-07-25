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
#include <QQuaternion>

#include "wheel.h"
#include "gyroscope.h"

#include <cmath>

const qreal degPerSecMCoef = 1;
const quint16 checksPerSecond = 50;
const qreal kalmanCoef = 1;
const qreal angVelocity = 4.487989339;

//  Garbage struct accel
struct accelerometer
{
    qreal x;
    qreal y;
    qreal z;
};

struct vector
{
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
    explicit Cyber(connectionMode cMode, QObject *parent = 0);
    ~Cyber();
    void turn (qreal degree);   //  NOTE:
    //  We allow turning for more than 360 degree (that's may be bad because of qreal precision)
    void stop();
    void moveByVector (vector toMove);  //  Main moving func
    void firstLaunch();

private:
    QList<wheel *> * wheels;        //  Wheels.
//    QList<vector *> * guide;        //  Guiding vector
    vector * guide;
    vector position;                //  Absolute position in the world
    vector direction;               //  Absolute direction
    vector moving;
//    vector acceleration;            //  Absolute acceleration
    qreal currRad;                  //  Current angle in radians
    qreal leftRad;                  //  Left radians to turn
    qreal integrand;                //  Summary angle of Z tilt (tilt * time)
    qreal correction;               //  Correction coefficient (in radians)
    quint16 count;

    QTimer * mainTimer;            //  Check position timer

    Gyroscope * gyro;

    gyro_pos angles;                //  Current gyro tilt
    gyro_pos absolute;              //  Filtered gyro pos (x axis is forward, z axis is -g)
    void turnLeft (qreal degree);   //  Precise turns
    void turnRight (qreal degree);
    void calibrate();               //  Calibrating function

signals:
    
public slots:

private slots:
    void checkPosition();
    void moveVectorSlot();
    void turnLeftSlot();
    void turnRightSlot();
    void calibrateSlot();
    void firstLaunchSlot();

};

#endif // CYBER_H
