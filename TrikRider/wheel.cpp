#include "wheel.h"

wheel::wheel(int wheelNumber, connectionMode mod, QObject *parent) :
    QObject(parent)
{
    wheelNum = wheelNumber;
    cMode = mod;
    switch (cMode) {
    case API:
        path = new QString ("/sys/class/pwm/ecap." + QString::number(wheelNum) + "/");    //  Get full path to engines
        request = new QFile (*path + QString("request"));
        if(!request->open(QFile::WriteOnly))
        {
            qDebug() << "Cannot open request #" << wheelNumber;
        }
        if(request->write("1") == -1 || !request->flush())
        {
            qDebug() << "Permission is not given to engine #" << wheelNumber;
        }
        period_ns = new QFile (*path + QString("period_ns"), this);
        duty_ns = new QFile (*path + QString("duty_ns"), this);
        run = new QFile (*path + QString("run"), this);
        if(!(period_ns->open(QFile::WriteOnly) && duty_ns->open(QFile::WriteOnly) && run->open(QFile::WriteOnly)))
        {
            qDebug() << "Cannot open either run or duty or period #" << wheelNumber;
        }
        //  Do I need to set freq right now?
        period_ns->write("20000000");
        period_ns->flush();
        //  Well...
        break;
    case I2C:
        message = new QString();
        break;
    default:
        break;
    }
    stopTimer = new QTimer (this);
    connect(stopTimer, SIGNAL(timeout()), this, SLOT(stopSlot()));
    stopTimer->setSingleShot(true);
}

wheel::~wheel()
{
    switch (cMode) {
    case API:
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
        break;
    case I2C:
        delete message;
        break;
    }
    delete stopTimer;
}

void wheel::stop()
{
    qDebug() << "Stop engine";
    rMode = NEUTRAL;
    switch (cMode) {
    case API:
        run->write("0");
        if(!run->flush())
        {
            qDebug() << "I can't stop! Run isn't flushing";
        }
        request->write("0");
        break;
    case I2C:
        message->clear();
        message->append("i2cset -y 2 0x48 0x0 0x");
        message->append(QString::number((abs((qRound(speed)))) + (wheelNum << 2) + rMode, 16)); //  Number, Base
        message->append(" w");
        qDebug() << message;
        system(message->toStdString().data());
        break;
    }
}

/*void wheel::spinForw(float speed)
{
    if (speed > 100 || speed < -100)
    {
        qDebug() << "Wrong speed on wheel #" << wheelNum;
        return ;
    }
    duty_ns->write(QString::number(setDutyNs (speed)).toStdString().data());
    run->write("1");
    run->flush();

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

    if(duty_ns->write(QString::number((setDutyNs (speed))).toStdString().data()) == -1)
    {
        printf ("Nothing written to duty_ns\n");
    }
    printf ("Writing to run...\n");
    if(run->write("1") == -1)
    {
        printf("Nothing is written to run\n");
    }
    duty_ns->flush();
    run->flush();
//    printf("%s\n%s\n%s\n", period_freq->readAll().constData(), duty_ns->readAll().constData(), run->readAll().constData());
    QTimer::singleShot((int) msecs * 1000, this, SLOT(stopSlot()));
//    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
}*/

/*void wheel::spinBackw(float speed, float msecs)
{

    duty_ns->write(QString::number(setDutyNs (speed)).toStdString().data());  //  Setting speed
    run->write("1");    //  Starting engine
    duty_ns->flush();
    run->flush();
    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
    //  Here'll be better func than spinForw and I'll rewrite it into upper one
}

void wheel::spinBackw(float speed)
{
    duty_ns->write(QString::number(setDutyNs (speed)).toStdString().data());
    run->write("1");
    duty_ns->flush();
    run->flush();
}*/

void wheel::spin(float nspeed)
{
    speed = nspeed;
    if (speed > 100 || speed < -100)
    {
        qDebug() << "Wrong speed on wheel #" << wheelNum;
        return ;
    }
    rMode = (speed > 0) ? FORW : BACKW;
    switch(cMode)
    {
    case API:
        duty_ns->write(QString::number(setDutyNs (speed)).toStdString().data());
        run->write("1");
        duty_ns->flush();
        run->flush();
        break;
    case I2C:
        /*
        message->clear();
        message->append("i2cset -y 2 0x48 0x0 0x");
        message->append(QString::number(((abs((qRound(speed)))) << 8 ) + (wheelNum << 2) + rMode, 16));
        message->append(" w");
        qDebug() << message->toStdString().data();
        system(message->toStdString().data());*/
        char Data[10];
        char output[90] =  "i2cset -y 2 0x48 0x0 0x";

        sprintf(Data,"%x", ((abs((int) speed)) << 8 ) + (wheelNum << 2) + rMode);

        strcat(output,Data);
        strcat(output," w");
        qDebug() << abs((int) speed) << '\t' << wheelNum << '\t' << rMode;
        qDebug() << output;
        system(output);
        break;
    }
}

void wheel::spin(float nspeed, float msecs)
{
    speed = nspeed;
    if (speed > 100 || speed < -100)
    {
        //  We set speed in percents
        qDebug() << "Wrong speed";
    }
    rMode = (speed > 0) ? FORW : BACKW;
    //  Debug printing
    switch(cMode)
    {
    case API:
        if(duty_ns->write(QString::number((setDutyNs (speed))).toStdString().data()) == -1)
        {
            qDebug() << "Nothing written to duty_ns";
        }
        if(run->write("1") == -1)
        {
            qDebug () << "Nothing is written to run";
        }
        duty_ns->flush();
        run->flush();
        break;
    case I2C:
        message->clear();
        message->append("i2cset -y 2 0x48 0x0 0x");
        message->append(QString::number(((abs((qRound(speed)))) << 8 ) + (wheelNum << 2) + rMode, 16));
        message->append(" w");
        system(message->toStdString().data());
        break;
    }
    qDebug() << "Start singleshot, msecs is:\t" << msecs * 1000;
    qDebug() << "stopTimer is singleShot: " << stopTimer->isSingleShot();
//    QTimer::singleShot((int) msecs * 1000, this, SLOT(stopSlot()));
//    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
    stopTimer->start((int) msecs);
}

/*float wheel::getSpeed()
{
    return speed;
}*/

int wheel::setDutyNs(float speed)
{
    return zeroPoint + (speed * amplitude) / 100; //  Convert percents into nsecs
}

void wheel::stopSlot()
{
    qDebug() << "Entered StopSlot";
    stop();
}
