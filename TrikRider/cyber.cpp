#include "cyber.h"

const float degPerSecMCoef = 15;

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    _1 = new wheel (1);
    _2 = new wheel (2);
    _3 = new wheel (3);
}

inline vector operator*(matrix *a, vector *b) const
{
    QSharedPointer<vector> toRet;
    //Some multiplying code that multiply.
}
// Deal with this code muffaka
inline vector operator+(vector *a, vector *b) const;
inline int operator*(vector *a, vector *b) const;

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
