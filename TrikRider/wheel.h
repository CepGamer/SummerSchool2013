#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QFile>
#include <qmath.h>

const float Pi = 3.1415926535;  //  Needed for some calc's
const float nsInMs = 1000;      //  Not 1024!

struct vector
{
    float x, y;
};

struct matrix
{
    float _11;
    float _12;
    float _21;
    float _22;
};

vector operator*(matrix a, vector b);
vector operator+(vector a, vector b);
float operator*(vector a, vector b);
vector normalize (vector a);

class wheel : public QObject
{
    Q_OBJECT
public:
    explicit wheel(int wheelNumber, vector guiding, QObject *parent = 0);
    ~wheel ();      //  Destroys the wheel
    void stop();    //  Stops
    void spin(float speed);     //  Spin continiously
    void spin(float speed, float msecs);    //  Spin during some time
    void setGuide(vector x);    //  Sets guiding vector
    vector *getGuide();         //  returns guiding vector

private:
    //  Serie of files that controls the drives
    QFile * request;        //  1 - open others for write, 0 - close interface
    QFile * period_freq;    //  Signal frequency
    QFile * duty_ns;        //  Width of 1-signal in ns
    QFile * run;            //  Run signal
    vector * guide;             //  Guiding vector
    void spinForw(float speed); //  Precise spin
    void spinBackw(float speed);
    void spinForw(float speed, float msecs);
    void spinBackw(float speed, float msecs);

signals:
    
public slots:
    
};

#endif // WHEEL_H
