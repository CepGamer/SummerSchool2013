#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QFile>
#include <qmath.h>

const float Pi = 3.1415926535;

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
    explicit wheel(vector guiding, QObject *parent = 0);
    ~wheel ();      //  Destroys the wheel
    void stop();    //  Stops
    void spin(float speed);     //  Spin continiously
    void spin(float speed, float msecs);    //  Spin during some time
    void setGuide(vector x);    //  Sets guiding vector
    vector *getGuide();         //  returns guiding vector

private:
    QList <QFile> * temp; //  Serie of files that controls the drives
    QList <QString> * tempName; //  Full pathes to files
    vector * guide;             //  Guiding vector
    void spinForw(float speed); //  Precise spin
    void spinBackw(float speed);
    void spinForw(float speed, float msecs);
    void spinBackw(float speed, float msecs);

signals:
    
public slots:
    
};

#endif // WHEEL_H
