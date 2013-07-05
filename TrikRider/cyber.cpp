#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    vector a;   //  Guiding vector for all
    a.x = 0;
    a.y = 1;
    _1 = new wheel (0, normalize(a)); //  Initialize first wheel

    a.x = cos(2 * Pi / 3);
    a.y = -sin(2 * Pi / 3);
    _2 = new wheel (1, normalize(a));   //  Second

    a.x = cos(4 * Pi / 3);
    a.y = -sin(4 * Pi / 3);
    _3 = new wheel (2, normalize(a));   //  Third
}

vector operator*(matrix a, vector b)
{
    vector toRet;   //  Plain math, multiplying matrix 2x2 & vector
    toRet.x = a._11 * b.x + a._12 * b.y;
    toRet.y = a._21 * b.x + a._22 * b.y;
    return toRet;
}

vector operator+(vector a, vector b)
{
    vector toRet;   //  Summing vectors
    toRet.x = a.x + b.x;
    toRet.y = a.y + b.y;
    return toRet;
}

float operator*(vector a, vector b)
{
    return a.x * b.x + a.y * b.y;   //  Scalar multiplying
}

vector normalize (vector a)
{
    vector b;   //  Resulting vector
    //  Plain math here, nothing to look at, move along
    float mod = a.x * a.x + a.y * a.y;
    b.x = a.x / mod;
    b.y = a.y / mod;
    return b;
}

matrix setAngle(float radAngle)
{
    matrix toRet;
    toRet._11 = cos(radAngle);
    toRet._12 = -sin(radAngle);
    toRet._21 = sin(radAngle);
    toRet._22 = cos(radAngle);
    return toRet;
}

Cyber::~Cyber()
{
    _1->~wheel();
    _2->~wheel();
    _3->~wheel();
    delete _1;
    delete _2;
    delete _3;
}

void Cyber::turn (float degree)
{
    if (degree > 0)
    {
        printf("Degree is big, running\n");
        turnLeft(degree);   //  If we turn
    }
    else if (degree < 0)
        turnRight(-degree);
}

void Cyber::stop()
{
    _1->stop(); //  Fullstop
    _2->stop();
    _3->stop();
}

void Cyber::moveByVector(float speed, vector guide)
{
    _1->spin((guide * _1->getGuide()) * speed);    //  Get those wheels spinning
    _2->spin((guide * _2->getGuide()) * speed);
    _3->spin((guide * _3->getGuide()) * speed);
    //  Will add code when the gyro ready
}

void Cyber::turnLeft(float degree)
{
    printf ("\nStart engine #1\n");
    _1->spin(50, degree / degPerSecMCoef);   //  spinning dem wheels
    printf ("\nStart engine #2\n");
    _2->spin(50, degree / degPerSecMCoef);
    printf ("\nStart engine #3\n");
    _3->spin(50, degree / degPerSecMCoef);
    //  Again, some code after gyro

}

void Cyber::turnRight(float degree)
{
    _1->spin(3, degree / degPerSecMCoef);
    _2->spin(3, degree / degPerSecMCoef);
    _3->spin(3, degree / degPerSecMCoef);
}
