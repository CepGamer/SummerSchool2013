#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QString>

#include <stdio.h>

const float Pi = 3.1415926535;  //  Needed for some calc's
const float nsInMs = 1000000.0;
const float msecsInPerc = 0.2;
const int zeroPoint = 1505000;
const int amplitude = 200000;

class wheel : public QObject
{
    Q_OBJECT
public:
    explicit wheel(int wheelNumber, QObject *parent = 0);
    ~wheel ();                  //  Destroys the wheel
    void stop();                //  Stops
    void spin(float nspeed);    //  Spin continiously
    void spin(float nspeed, float msecs);   //  Spin during some time
    inline float getSpeed();

private:
    //  Serie of files that controls the drives
    QFile * request;        //  1 - open others for write, 0 - close interface
    QFile * period_ns;      //  Signal frequency
    QFile * duty_ns;        //  Width of 1-signal in ns
    QFile * run;            //  Run signal
    QTimer * stopTimer;     //  Stopping timer
    int wheelNum;           //  Wheel number
    float speed;            //  Wheel speed (in percents)
//    void spinForw(float speed);     //  Precise spin
//    void spinBackw(float speed);
//    void spinForw(float speed, float msecs);
//    void spinBackw(float speed, float msecs);
    inline int setDutyNs(float speed);

signals:

public slots:
    void stopSlot();
};

#endif // WHEEL_H
