#include "wheel.h"

wheel::wheel(int wheelNumber, vector guiding, QObject *parent) :
    QObject(parent)
{
    QString s = QString ("/sys/class/pwm/ecap." + QString::number(wheelNumber) + "/");    //  Get full path to engines
    guide = new vector; //  Get new vector
    setGuide(guiding);
    request = new QFile (s + QString("request"));
    stopTimer = new QTimer (this);
    connect(stopTimer, SIGNAL(timeout()), this, SLOT(stopSlot()));
    if(!request->open(QFile::ReadWrite))
    {
        printf("%s\n", s.toAscii().constData());
        printf("%s\n", request->fileName().toAscii().constData());
        printf("Cannot open request #%d\n", wheelNumber);
    }
    if(request->write("1") == -1)
    {
        printf ("Permission is not given to engine #%d\n", wheelNumber);
    }
    period_ns = new QFile (s + QString("period_ns"), this);
    duty_ns = new QFile (s + QString("duty_ns"), this);
    run = new QFile (s + QString("run"), this);
    if(!(period_ns->open(QFile::WriteOnly) && duty_ns->open(QFile::WriteOnly) && run->open(QFile::WriteOnly)))
    {
        printf("Cannot open either run or duty or period #%d\n\n", wheelNumber);
    }
    //  Do I need to set freq right now?
    period_ns->write("20000000");
    period_ns->close();
    request->close();
    //  Well...
}

wheel::~wheel()
{
    run->write("0");
    //  Do we need to clean other files?
    run->close();
    period_ns->close();
    duty_ns->close();
    request->write("0");
    request->close();
    //  Delete after closing
    delete run;
    delete duty_ns;
    delete period_ns;
    delete request;

    delete guide;
}

void wheel::stop()
{
    run->write("0");
    run->close();
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
    if (speed > 100 || speed < 0)
    {
        printf("Wrong speed\n");
    }
    duty_ns->write(QString::number(speed * msecsInPerc * nsInMs).toAscii().constData());
}

void wheel::spinForw(float speed, float msecs)
{
    if (!period_ns->open(QFile::ReadOnly))
    {
        printf("Period freq isn't opened\n");
    }
    else
    {
        printf("period_ns is:\n%s", period_ns->readAll().constData());
        period_ns->close();
/*
 *
 *        period_ns->open(QFile::WriteOnly);
        period_ns->write("20000000");
        period_ns->close();*/
//        printf("%s\n", period_freq->readAll().constData());
//        period_freq->close();
    }
    printf ("Spinning engine...\n");
    if (speed > 100 || speed < 0)
    {
        printf("Wrong speed\n");
    }
    printf ("\nWriting to duty_ns...\n");

    printf("%f\n", speed * msecsInPerc * nsInMs);
    if(duty_ns->write(QString::number(((int)(speed * msecsInPerc * nsInMs))).toAscii().constData()) == -1)
    {
        printf ("Nothing written to duty_ns\n");
    }
    duty_ns->close();

    printf ("Writing to run...\n");
    if(run->write("1") == -1)
    {
        printf("Nothing is written to run\n");
    }
//    printf("%s\n%s\n%s\n", period_freq->readAll().constData(), duty_ns->readAll().constData(), run->readAll().constData());
//    QTimer::singleShot((int) msecs, this, SLOT(stopSlot()));
    run->close();
//    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
}

void wheel::spinBackw(float speed, float msecs)
{
}

void wheel::stopSlot()
{
    printf ("\nEntered StopSlot\n");
    stop();
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
