#ifndef CYBER_H
#define CYBER_H

#include <QObject>
#include <QSharedPointer>
#include <QList>

#include "wheel.h"
#include "gyro.h"

#include <cmath>

const float degPerSecMCoef = 1;

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
//void operator=(vector a, vector b);
float operator*(vector a, vector b);
vector normalize (vector a);
matrix setAngle(float radAngle);

class Cyber : public QObject
{
    Q_OBJECT
public:
    explicit Cyber(QObject *parent = 0);
    ~Cyber();
    void turn (float degree);    //  NOTE:
    //  We allow turning for more than 360 degree (that's may be bad because of float precision)
    void stop();
    void moveByVector (float speed, vector toMove);  //  Main moving func

private:
    QList<wheel *> * wheels;      //  Wheels.
    QList<vector *> * guide;      //  Guiding vector
    void turnLeft (float degree);   //  Precise turns
    void turnRight (float degree);

signals:
    
public slots:
    
};

#endif // CYBER_H
