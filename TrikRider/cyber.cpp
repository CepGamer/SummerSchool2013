#include "cyber.h"

Cyber::Cyber(connectionMode cMode, QObject *parent) :
    QObject(parent)
{
    wheels = new QList<wheel *>();
//    guide = new QList<vector *>();
    gyro = new Gyroscope(this);
    guide = new vector[3];         //  Guiding vectors for all
//    curr = new gyro_pos;

    gyro->setConnection();
    guide[0].x = cos(-Pi / 2);
    guide[0].y = sin(-Pi / 2);
    wheels->append( new wheel (1, cMode));      //  First wheel

    guide[1].x = cos(2 * Pi / 3 - Pi / 2);
    guide[1].y = sin(2 * Pi / 3 - Pi / 2);
    wheels->append( new wheel (3, cMode));     //  Second

    guide[2].x = cos(4 * Pi / 3 - Pi / 2);
    guide[2].y = sin(4 * Pi / 3 - Pi / 2);
    wheels->append( new wheel (2, cMode));     //  Third

    position.x = position.y = \
            direction.x = direction.y = \
            acceleration.x = acceleration.y = 0;

    checkTimer = new QTimer(this);
    count = 0;
}

vector operator *(matrix a, vector b)
{
    vector toRet;   //  Plain math, multiplying matrix 2x2 & vector
    toRet.x = a._11 * b.x + a._12 * b.y;
    toRet.y = a._21 * b.x + a._22 * b.y;
    return toRet;
}

vector operator*(qreal a, vector b)
{
    vector toRet;
    toRet.x = b.x * a;
    toRet.y = b.y * a;
    return toRet;
}

vector operator*(vector a, qreal b)
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
qreal operator*(vector a, vector b)
{
    return a.x * b.x + a.y * b.y;   //  Scalar multiplying
}

vector normalize(vector a)
{
    vector b;   //  Resulting vector
    //  Plain math here, nothing to look at, move along
    qreal mod = a.x * a.x + a.y * a.y;
    b.x = a.x / mod;
    b.y = a.y / mod;
    return b;
}

matrix setAngle(qreal radAngle)
{
    matrix toRet;
    toRet._11 = cos(radAngle);
    toRet._12 = -sin(radAngle);
    toRet._21 = sin(radAngle);
    toRet._22 = cos(radAngle);
    return toRet;
}

vector setVAngle(qreal radAngle)
{
    vector toRet;
    toRet.x = qCos(radAngle);
    toRet.y = qSin(radAngle);
    return toRet;
}

Cyber::~Cyber()
{
    delete wheels->at(0);
    delete wheels->at(1);
    delete wheels->at(2);

    delete checkTimer;
    delete wheels;
    delete guide;
    delete gyro;
}

void Cyber::turn (qreal degree)
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

void Cyber::moveByVector(qreal speed, vector toMove)
{

    wheels->at(0)->spin((toMove * guide[0]) * speed);    //  Get those wheels spinning

    wheels->at(1)->spin((toMove * guide[1]) * speed);

    wheels->at(2)->spin((toMove * guide[2]) * speed);
    //  Will add code when the gyro & accel ready
}

void Cyber::turnLeft(qreal degree)
{
    qDebug() << "Start engines";
/*    wheels->at(0)->spin(100);   //  spinning dem wheels
    wheels->at(1)->spin(100);
    wheels->at(2)->spin(100);*/
    leftRad = degree / 180 * Pi;
    //  Again, some code after gyro
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(turnLeftSlot()));
    checkTimer->start(1000 / checksPerSecond);
}

void Cyber::turnRight(qreal degree)
{
    qDebug() << "Start engines";
    /*wheels->at(0)->spin(-100);
    wheels->at(1)->spin(-100);
    wheels->at(2)->spin(-100);*/
    leftRad = degree / 180 * Pi;
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(turnRightSlot()));
    checkTimer->start(1000 / checksPerSecond);
}

void Cyber::calibrate()
{
    direction.x = direction.y = position.x = position.y = 0;
//    gyro->setConnection();
    angles.m_tiltX = gyro->getTiltX();
    angles.m_tiltY = gyro->getTiltY();
    angles.m_tiltZ = gyro->getTiltZ();
    //    QTimer::singleShot(10000, this, SLOT());
    //  Set calibration
    integrand = 0;
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
    checkTimer->start(1000 / checksPerSecond);
}

void Cyber::checkPosition()
{
    vector moving;
    qreal angVelocityC = angVelocity / ((wheels->at(1)->getSpeed() > 75) ? 2 : 1);
    vector on50percentSpeed;
    currRad += angVelocityC / checksPerSecond;
//    currRad = currRad % (2 * Pi);
    on50percentSpeed.x = qCos(angVelocityC * 2 * Pi);
    on50percentSpeed.y = qSin(angVelocityC * 2 * Pi);
    moving.x = 0;
    moving.y = 0;
    for (int i=0; i<3; i++)
        moving = moving + guide[i] * (wheels->at(i)->getSpeed() / 100);
/*    for (int i=0; i<3; i++)
        angVelocity += wheels->at(i)->getSpeed();
        angVelocity = convertSpeed();   //  convert percents into speed. Or based on an encoder.
*/
//    gyro->readGyroEvent();
    angles.m_tiltX = gyro->getTiltX();
    angles.m_tiltY = gyro->getTiltY();
    angles.m_tiltZ = gyro->getTiltZ();
    //  Angles correction
/*    qDebug() << "Pos is:\t" << position.x << '\t' << position.y << '\n'\
             << "Dir is:\t" << direction.x << '\t' << direction.y;
    position = position + ( (kalmanCoef / (checksPerSecond * checksPerSecond * 2)) * acceleration \
                            + ((1 - kalmanCoef) / checksPerSecond) * (moving));*/
    direction = direction + (kalmanCoef / checksPerSecond) * (setAngle( angles.m_tiltZ ) * direction) + \
            ((1 - kalmanCoef) / checksPerSecond ) * on50percentSpeed;
    absolute.m_tiltZ = kalmanCoef * angles.m_tiltZ + (1 - kalmanCoef) * angVelocityC;
}

void Cyber::turnLeftSlot()
{
    if(leftRad < 0.2)
    {
        checkTimer->stop();
        checkTimer->disconnect(checkTimer, SIGNAL(timeout()), this, SLOT(turnLeftSlot()));
        stop();
    }
    else if(leftRad < 2 * Pi)
        for(int i=0; i<3; i++)
            wheels->at(i)->spin(50);
    checkPosition();
    leftRad -= qAbs(absolute.m_tiltZ / checksPerSecond);
}

void Cyber::turnRightSlot()
{
    count++;
    count %= checksPerSecond;
    if(count == 0)
        qDebug() << "leftRad is:\t" << QString::number(leftRad, 'f');
    if(leftRad < 0.2)
    {
        checkTimer->stop();
        checkTimer->disconnect(checkTimer, SIGNAL(timeout()), this, SLOT(turnRightSlot()));
        stop();
    }
    else if(leftRad < 2 * Pi)
        for(int i=0; i<3; i++)
            wheels->at(i)->spin(-50);
    checkPosition();
    leftRad -= qAbs(absolute.m_tiltZ / checksPerSecond);
}

void Cyber::calibrateSlot()
{
    count++;
    count %= checksPerSecond;
    if(count == 0)
        qDebug() << "Integrand is:\t" << QString::number(integrand, 'f');
    checkPosition();
    integrand += angles.m_tiltZ / checksPerSecond;
}
