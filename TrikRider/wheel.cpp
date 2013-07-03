#include "wheel.h"

wheel::wheel(int wheelNumber, vector guiding, QObject *parent) :
    QObject(parent)
{
    guide.x = guiding.x;
    guide.y = guiding.y;
    temp = new QFile(QString("/sys/devices/platform/ecap.0/pwm/ecap.0/period_ns"));

}

vector operator*(matrix a, vector b) const
{
    vector toRet;
    toRet.x = a._11 * b.x + a._12 * b.y;
    toRet.y = a._21 * b.x + a._22 * b.y;
    return toRet;
}

vector operator+(vector a, vector b) const
{
    vector toRet;
    toRet.x = a.x + b.x;
    toRet.y = a.y + b.y;
    return toRet;
}

inline int operator*(vector a, vector b) const
{
    return a.x * b.x + a.y * b.y;
}

vector normalize (vector a) const
{
    vector b;   //  Resulting vector
    //  Plain math here, nothing to look at, move along
    float mod = a.x * a.x + a.y * a.y;
    b.x = a.x / mod;
    b.y = a.y / mod;
    return b;
}

void wheel::spinForw(float speed, int msecs)
{
}

void wheel::spinBackw(float speed)
{
}


void wheel::setGuide(vector x)
{
    guide.x = x.x;
    guide.y = x.y;
}

vector wheel::getGuide()
{
    return guide;
}

void wheel::spin(float speed)
{
    if (speed > 0)
        spinForw(speed);
    else if (speed < 0)
        spinBackw(speed);
}

void wheel::spin(float speed, float msecs)
{
    if (speed > 0)
        spinForw(speed, msecs);
    else if (speed < 0)
        spinBackw(speed, msecs);
}
