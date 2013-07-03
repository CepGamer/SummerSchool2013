#include "wheel.h"

wheel::wheel(vector guiding, QObject *parent) :
    QObject(parent)
{
    guide = new vector;
    setGuide(guiding);
}

wheel::~wheel()
{
    temp->~QList();
    delete guide;
}

void wheel::stop()
{
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

void wheel::spinForw(float speed, float msecs)
{
}

void wheel::spinBackw(float speed, float msecs)
{
}

void wheel::spinBackw(float speed)
{
}


void wheel::setGuide(vector x)
{
    guide->x = x.x; //  Setting guide vector
    guide->y = x.y;
}

vector *wheel::getGuide()
{
    return guide;
}

void wheel::spinForw(float speed)
{
}

void wheel::spin(float speed)
{
    if (speed > 0)
        spinForw(speed);
    else if (speed < 0)
        spinBackw(-speed);
}

void wheel::spin(float speed, float msecs)
{
    if (speed > 0)
        spinForw(speed, msecs);
    else if (speed < 0)
        spinBackw(-speed, msecs);
}
