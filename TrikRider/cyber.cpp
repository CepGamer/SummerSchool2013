#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    wheels = new QList<wheel *>();
//    guide = new QList<vector *>();
    gyro = new Gyroscope(this);
    guide = new vector[3];         //  Guiding vectors for all
//    curr = new gyro_pos;

    guide[0].x = cos(-Pi / 2);
    guide[0].y = sin(-Pi / 2);
    wheels->append(new wheel (0));      //  First wheel

    guide[1].x = cos(2 * Pi / 3 - Pi / 2);
    guide[1].y = sin(2 * Pi / 3 - Pi / 2);
    wheels->append( new wheel (1));     //  Second

    guide[2].x = cos(4 * Pi / 3 - Pi / 2);
    guide[2].y = sin(4 * Pi / 3 - Pi / 2);
    wheels->append( new wheel (2));     //  Third
}

vector operator *(matrix a, vector b)
{
    vector toRet;   //  Plain math, multiplying matrix 2x2 & vector
    toRet.x = a._11 * b.x + a._12 * b.y;
    toRet.y = a._21 * b.x + a._22 * b.y;
    return toRet;
}

vector operator*(float a, vector b)
{
    vector toRet;
    toRet.x = b.x * a;
    toRet.y = b.y * a;
    return toRet;
}

vector operator*(vector a, float b)
{
    return b * a;
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

vector normalize(vector a)
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
    delete guide;
    delete gyro;
}

void Cyber::turn (float degree)
{
    if (degree > 0)
    {
        turnRight(degree);   //  If we turn
    }
    else if (degree < 0)
        turnLeft(-degree);
}

void Cyber::stop()
{
    wheels->at(0)->stop(); //  Fullstop
    wheels->at(1)->stop();
    wheels->at(2)->stop();
}

void Cyber::moveByVector(float speed, vector toMove)
{
    qDebug() << "First engine speed:\t" << (qreal)(toMove * guide[0]);
    qDebug() << "First engine vector:\t" << guide[0].x << '\t' << guide[0].y;
    qDebug() << "guide[0]:\t" << (qreal)guide[0].x << '\t' << (qreal)guide[0].y;
    wheels->at(0)->spin((toMove * guide[0]) * speed);    //  Get those wheels spinning

    qDebug() << "Second engine speed:\t" << (qreal)(toMove * guide[1]);
    qDebug() << "Second engine vector:\t" << guide[1].x << '\t' << guide[1].y;
    qDebug() << "guide[1]:\t" << (qreal)guide[1].x << '\t' << (qreal)guide[1].y;
    wheels->at(1)->spin((toMove * guide[1]) * speed);

    qDebug() << "Third engine speed:\t" << (qreal)(toMove * guide[2]);
    qDebug() << "First engine vector:\t" << guide[2].x << '\t' << guide[2].y;
    qDebug() << "guide[2]:\t" << (qreal)guide[2].x << '\t' << (qreal)guide[2].y;
    wheels->at(2)->spin((toMove * guide[2]) * speed);
    //  Will add code when the gyro ready
}

void Cyber::turnLeft(float degree)
{
    qDebug() << "Start engine #1";
    wheels->at(0)->spin(50, degree / degPerSecMCoef);   //  spinning dem wheels

    qDebug() << "Start engine #2";
    wheels->at(1)->spin(50, degree / degPerSecMCoef);

    qDebug() << "Start engine #3";
    wheels->at(2)->spin(50, degree / degPerSecMCoef);
    //  Again, some code after gyro

}

void Cyber::turnRight(float degree)
{
    qDebug() << "Start engine #1, DpMSec is:\t" << degree / degPerSecMCoef;
    wheels->at(0)->spin(-50, degree / degPerSecMCoef);

    qDebug() << "Start engine #2";
    wheels->at(1)->spin(-50, degree / degPerSecMCoef);

    qDebug() << "Start engine #3";
    wheels->at(2)->spin(-50, degree / degPerSecMCoef);
}

void Cyber::calibrate()
{
    direction.x = direction.y = position.x = position.y = 0;
    gyro->setConnection();
    angles->m_tiltX = gyro->getTiltX();
    angles->m_tiltY = gyro->getTiltY();
    angles->m_tiltZ = gyro->getTiltZ();
    //    QTimer::singleShot(10000, this, SLOT());
}

void Cyber::checkPosition()
{
    vector moving;
    moving.x = 0;
    moving.y = 0;
    for (int i=0; i<3; i++)
        moving = moving + guide[i] * (wheels->at(i)->getSpeed() / 100);
    gyro->setConnection();
    angles.m_tiltX = gyro->getTiltX();
    angles.m_tiltY = gyro->getTiltY();
    angles.m_tiltZ = gyro->getTiltZ();
    //  Angles correction

    position = position + ( (kalmanCoef / (checksPerSecond * checksPerSecond * 2)) * acceleration + ((1 - kalmanCoef)) * (moving));
//    position = kalmanCoef * current->m_tiltZ + (1 - kalmanCoef) * (position + moving);
}
