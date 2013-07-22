#ifndef CYBER_H
#define CYBER_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QQuaternion>

#include "wheel.h"
#include "gyroscope.h"

#include <cmath>

const float degPerSecMCoef = 1;
const int checksPerSecond = 10;
const float kalmanCoef = 0.5;

//  Garbage struct accel
struct accelerometer
{
    float x;
    float y;
    float z;
};

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
vector operator*(float a, vector b);
inline vector operator*(vector a, float b);
vector operator+(vector a, vector b);
//void operator=(vector a, vector b);
inline float operator*(vector a, vector b);
vector normalize (vector a);
matrix setAngle(float radAngle);

class Cyber : public QObject
{
    Q_OBJECT
public:
    explicit Cyber(connectionMode cMode, QObject *parent = 0);
    ~Cyber();
    void turn (float degree);   //  NOTE:
    //  We allow turning for more than 360 degree (that's may be bad because of float precision)
    void stop();
    void moveByVector (float speed, vector toMove);  //  Main moving func

private:
    QList<wheel *> * wheels;        //  Wheels.
//    QList<vector *> * guide;        //  Guiding vector
    vector * guide;
    vector position;                //  Absolute position in the world
    vector direction;               //  Absolute direction
    vector acceleration;            //  Absolute acceleration

    Gyroscope * gyro;

    gyro_pos angles;             //  Current gyro tilt
//    gyro_pos * absolute;            //  Absolute gyro pos (x axis is forward, z axis is -g)
    void turnLeft (float degree);   //  Precise turns
    void turnRight (float degree);
    void calibrate ();              //  Calibrating function

signals:
    
public slots:

private slots:
    void checkPosition();
    
};

#endif // CYBER_H
