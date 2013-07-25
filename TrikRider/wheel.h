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

#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <qmath.h>

#include <stdio.h>

enum connectionMode {API, I2C};
enum runMode {NEUTRAL = 0, FORW = 1, BACKW = 2, BLOCK = 3};

const qreal Pi = 3.1415926535;  //  Needed for some calc's
const qreal nsInMs = 1000000.0;
const qreal msecsInPerc = 0.2;
const int zeroPoint = 1505000;
const int amplitude = 200000;

class wheel : public QObject
{
    Q_OBJECT
public:
    explicit wheel(int wheelNumber, connectionMode cMode = API, QObject *parent = 0);
    ~wheel ();                  //  Destroys the wheel
    void stop();                //  Stops
    void spin(qreal nspeed);    //  Spin continiously
    void spin(qreal nspeed, qreal msecs);   //  Spin during some time
    inline qreal getSpeed(){return speed;}

private:
    //  Serie of files that controls the drives through API
    QFile * request;        //  1 - open others for write, 0 - close interface
    QFile * period_ns;      //  Signal frequency
    QFile * duty_ns;        //  Width of 1-signal in ns
    QFile * run;            //  Run signal
    QString * path;

    //  Serie of file that controls the drives through I2C
    QString * message;      //  System message for I2C

    //  Independent values
    QTimer * stopTimer;     //  Stopping timer
    int wheelNum;           //  Wheel number
    qreal speed;            //  Wheel speed (in percents)
    connectionMode cMode;   //  Connection mode
    runMode rMode;          //  Running mode

//    void spinForw(float speed);     //  Precise spin
//    void spinBackw(float speed);
//    void spinForw(float speed, float msecs);
//    void spinBackw(float speed, float msecs);
    inline int setDutyNs(int nspeed){return zeroPoint + (nspeed * amplitude) / 100;} //  Convert percents into nsecs

signals:

public slots:
    void stopSlot();
};

#endif // WHEEL_H
