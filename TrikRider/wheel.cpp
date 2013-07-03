#include "wheel.h"

wheel::wheel(int wheelNumber, vector guiding, QObject *parent) :
    QObject(parent)
{
    QString s = QString ("/sys/class/pwm/ecap." + QString::number(wheelNumber));    //  Get full path to engines
    guide = new vector; //  Get new vector
    setGuide(guiding);
    request = new QFile (s + QString("request"));
    if(!request->open(QFile::WriteOnly))
    {
        qDebug << "Cannot open request #" << wheelNumber << "\n";
    }
    request->write("1");
    period_freq = new QFile (s + QString("period_freq"));
    duty_ns = new QFile (s + QString("duty_ns"));
    run = new QFile (s + QString("run"));
    if(!(period_freq->open(QFile::WriteOnly) && duty_ns->open(QFile::WriteOnly) && run->open(QFile::WriteOnly)))
    {
        qDebug() << "Cannot open either run or duty or period #" << wheelNumber;
    }
    //  Do I need to set freq right now?
}

wheel::~wheel()
{
    run->write("0");
    //  Do we need to clean other files?
    run->close();
    period_freq->close();
    duty_ns->close();
    request->write("0");
    request->close();
    //  Delete after closing
    delete run;
    delete duty_ns;
    delete period_freq;
    delete request;

    delete guide;
}

void wheel::stop()
{
    run->write("0");
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

void wheel::spinForw(float speed)
{
    duty_ns->write(QString::number());
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
