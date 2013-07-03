#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    vector a;   //  Guiding vector for all
    a.x = 0;
    a.y = 1;
    _1 = new wheel (1, normalize(a)); //  Initialize first wheel

    a.x = qCos(2 * Pi / 3);
    a.y = -qSin(2 * Pi / 3);
    _2 = new wheel (2, normalize(a));   //  Second

    a.x = qCos(4 * Pi / 3);
    a.y = -qSin(4 * Pi / 3);
    _3 = new wheel (3, normalize(a));   //  Third
}

void Cyber::turn (float degree)
{
    if (degree > 0)
        turnLeft(degree);   //  If we turn
    else if (degree < 0)
        turnRight(-degree);
}

void Cyber::stop()
{
    _1.stop();
    _2.stop();
    _3.stop();
}

void Cyber::moveByVector(float speed, vector guide)
{
    _1.spin(guide * _1.getGuide() * speed);
    _1.spin(guide * _2.getGuide() * speed);
    _1.spin(guide * _3.getGuide() * speed);
}

void Cyber::turnLeft(float degree)
{
    _1.spinForw(3, degree / degPerSecMCoef);
    _2.spinForw(3, degree / degPerSecMCoef);
    _3.spinForw(3, degree / degPerSecMCoef);
}

void Cyber::turnRight(float degree)
{
    _1.spinForw(3, degree / degPerSecMCoef);
    _2.spinForw(3, degree / degPerSecMCoef);
    _3.spinForw(3, degree / degPerSecMCoef);
}
