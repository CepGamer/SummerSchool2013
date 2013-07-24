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
<<<<<<< HEAD
    void spin(qreal nspeed);    //  Spin continiously
    void spin(qreal nspeed, qreal msecs);   //  Spin during some time
    inline qreal getSpeed(){return speed;}
=======
    void spin(float nspeed);    //  Spin continiously
    void spin(float nspeed, float msecs);   //  Spin during some time
    inline float getSpeed(){return speed;}
>>>>>>> 10b39b19c0c40f9d05f04a2a82ae723c337d0b32

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
