#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    wheels = new QList<wheel *>;
    guide = new QList<vector *>;
    vector a;   //  Guiding vector for all
    a.x = 0;
    a.y = 1;
    guide->append( &a );
    wheels->append(new wheel (0));    //  First wheel

    a.x = cos(2 * Pi / 3);
    a.y = -sin(2 * Pi / 3);
    guide->append( &a );
    wheels->append( new wheel (1));    //  Second

    a.x = cos(4 * Pi / 3);
    a.y = -sin(4 * Pi / 3);
    guide->append( &a );
    wheels->append( new wheel (2));    //  Third
}

vector operator *(matrix a, vector b)
{
    vector toRet;   //  Plain math, multiplying matrix 2x2 & vector
    toRet.x = a._11 * b.x + a._12 * b.y;
    toRet.y = a._21 * b.x + a._22 * b.y;
    return toRet;
}

vector operator +(vector a, vector b)
{
    vector toRet;   //  Summing vectors
    toRet.x = a.x + b.x;
    toRet.y = a.y + b.y;
    return toRet;
}

/*vector operator =(vector a, vector b)
{
    a.x = b.x;
    a.y = b.y;
}
*/
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
    wheels->at(0)->~wheel();
    wheels->at(1)->~wheel();
    wheels->at(2)->~wheel();
    delete wheels;
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
    wheels->at(0)->stop(); //  Fullstop
    wheels->at(1)->stop();
    wheels->at(2)->stop();
}

void Cyber::moveByVector(float speed, vector toMove)
{
    wheels->at(0)->spin((toMove * *guide->at(0)) * speed);    //  Get those wheels spinning
    wheels->at(1)->spin((toMove * *guide->at(1)) * speed);
    wheels->at(2)->spin((toMove * *guide->at(2)) * speed);
    //  Will add code when the gyro ready
}

void Cyber::turnLeft(float degree)
{
    printf ("\nStart engine #1\n");
    wheels->at(0)->spin(50, degree / degPerSecMCoef);   //  spinning dem wheels
    printf ("\nStart engine #2\n");
    wheels->at(1)->spin(50, degree / degPerSecMCoef);
    printf ("\nStart engine #3\n");
    wheels->at(2)->spin(50, degree / degPerSecMCoef);
    //  Again, some code after gyro

}

void Cyber::turnRight(float degree)
{
    wheels->at(0)->spin(3, degree / degPerSecMCoef);
    wheels->at(1)->spin(3, degree / degPerSecMCoef);
    wheels->at(2)->spin(3, degree / degPerSecMCoef);
}
