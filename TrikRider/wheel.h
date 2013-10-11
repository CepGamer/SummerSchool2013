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

#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <qmath.h>

#include <stdio.h>

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
    explicit wheel(int wheelNumber, QObject *parent = 0);
    ~wheel ();                  //  Destroys the wheel
    void stop();                //  Stops
    void spin(qreal nspeed);    //  Spin continiously
    inline qreal getSpeed(){return speed;}

private:
    //  Serie of files that controls the drives through API
    QFile * request;        //  1 - open others for write, 0 - close interface
    QFile * period_ns;      //  Signal frequency
    QFile * duty_ns;        //  Width of 1-signal in ns
    QFile * run;            //  Run signal

    //  Independent values
    quint8 wheelNum;        //  Wheel number
    qreal speed;            //  Wheel speed (in percents)
    runMode rMode;          //  Running mode

    inline int setDutyNs(int nspeed){return zeroPoint + (nspeed * amplitude) / 100;} //  Convert percents into nsecs
    quint16 roundSpeed();   //  Round speed to degree of 10
    void setPeriod (quint32 per);
signals:

public slots:
};

#endif // WHEEL_H
