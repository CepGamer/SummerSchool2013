#include "wheel.h"

wheel::wheel(int wheelNumber, vector guideV, QObject *parent) :
    QObject(parent)
{
    wheelNum = wheelNumber;
    guide = new vector;
    setGuide(guideV);
    QString s = QString ("/sys/class/pwm/ecap." + QString::number(wheelNum) + "/");    //  Get full path to engines
    request = new QFile (s + QString("request"));
    stopTimer = new QTimer (this);
    connect(stopTimer, SIGNAL(timeout()), this, SLOT(stopSlot()));
    if(!request->open(QFile::WriteOnly))
    {
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
}

void wheel::stop()
{
    printf("Stop engine\n");
    run->open(QFile::WriteOnly);
    run->write("0");
    run->close();
    request->write("0");
}

void wheel::spinForw(float speed)
{
    if (speed > 100 || speed < 0)
    {
        printf("Wrong speed\n");
    }
    duty_ns->write(QString::number(setDutyNs (speed)).toAscii().constData());
    run->write("1");
}

void wheel::spinForw(float speed, float msecs)
{
    printf ("Spinning engine...\n");
    if (speed > 100 || speed < 0)
    {
        //  We set speed in percents
        printf("Wrong speed\n");
    }
    //  Debug printing
    printf ("Writing to duty_ns %d\n", setDutyNs (speed));
//    printf("duty_ns must be:\n%d\nBut now it:\n%d\n", (int) (setDutyNs (speed)), 1700000);

    if(duty_ns->write(QString::number((setDutyNs (speed))).toAscii().constData()) == -1)
    {
        printf ("Nothing written to duty_ns\n");
    }
    printf ("Writing to run...\n");
    if(run->write("1") == -1)
    {
        printf("Nothing is written to run\n");
    }
    run->close();
//    printf("%s\n%s\n%s\n", period_freq->readAll().constData(), duty_ns->readAll().constData(), run->readAll().constData());
    QTimer::singleShot((int) msecs * 1000, this, SLOT(stopSlot()));
//    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
}

void wheel::spinBackw(float speed, float msecs)
{

    duty_ns->write(QString::number(setDutyNs (speed)).toAscii().constData());  //  Setting speed
    run->write("1");    //  Starting engine
    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
    //  Here'll be better func than spinForw and I'll rewrite it into upper one
}

int wheel::setDutyNs(float speed)
{
    return zeroPoint + (speed * amplitude) / 100; //  Convert percents into nsecs
}

void wheel::stopSlot()
{
    printf ("\nEntered StopSlot\n");
    stop();
}

void wheel::spinBackw(float speed)
{
    duty_ns->write(QString::number(setDutyNs (speed)).toAscii().constData());
    run->write("1");
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

void wheel::setGuide(vector guideV)
{
    guide->x = guideV.x;
    guide->y = guideV.y;
}

vector wheel::getGuide()
{
    vector toRet;
    toRet.x = guide->x;
    toRet.y = guide->y;
    return toRet;
}
