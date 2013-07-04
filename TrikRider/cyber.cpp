#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    vector a;   //  Guiding vector for all
    a.x = 0;
    a.y = 1;
    _1 = new wheel (0, normalize(a)); //  Initialize first wheel

    a.x = qCos(2 * Pi / 3);
    a.y = -qSin(2 * Pi / 3);
    _2 = new wheel (1, normalize(a));   //  Second

    a.x = qCos(4 * Pi / 3);
    a.y = -qSin(4 * Pi / 3);
    _3 = new wheel (2, normalize(a));   //  Third
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
    _1->spin((guide * *_1->getGuide()) * speed);    //  Get those wheels spinning
    _2->spin((guide * *_2->getGuide()) * speed);
    _3->spin((guide * *_3->getGuide()) * speed);
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
