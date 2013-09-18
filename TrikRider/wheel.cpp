/*
Copyright (c) 2013, Bolotov Sergey
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Saint-Petersburg State University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Bolotov Sergey BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "wheel.h"

wheel::wheel(int wheelNumber, wheelType mod, QObject *parent) :
    QObject(parent)
{
    wheelNum = wheelNumber;
    cMode = mod;
    switch (cMode) {
    case SERV:
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
    case DC:
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
    case SERV:
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
    case DC:
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
    case SERV:
        run->write("0");
        if(!run->flush())
        {
            qDebug() << "I can't stop! Run isn't flushing";
        }
        request->write("0");
        break;
    case DC:
        message->clear();
        message->append("i2cset -y 2 0x48 0x0 0x");
        message->append(QString::number((abs((roundSpeed())) << 8) + (wheelNum << 2) + rMode, 16)); //  Number, Base
        message->append(" w");
        qDebug() << message;
        system(message->toStdString().data());
        break;
    }
}

/*void wheel::spinForw(qreal speed)
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

void wheel::spinForw(qreal speed, qreal msecs)
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

/*void wheel::spinBackw(qreal speed, qreal msecs)
{

    duty_ns->write(QString::number(setDutyNs (speed)).toStdString().data());  //  Setting speed
    run->write("1");    //  Starting engine
    duty_ns->flush();
    run->flush();
    stopTimer->singleShot((int) msecs, this, SLOT(stopSlot()));
    //  Here'll be better func than spinForw and I'll rewrite it into upper one
}

void wheel::spinBackw(qreal speed)
{
    duty_ns->write(QString::number(setDutyNs (speed)).toStdString().data());
    run->write("1");
    duty_ns->flush();
    run->flush();
}*/

void wheel::spin(qreal nspeed)
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
    case SERV:
        duty_ns->write(QString::number(setDutyNs ((int) speed)).toStdString().data());
        run->write("1");
        duty_ns->flush();
        run->flush();
        break;
    case DC:

        message->clear();
        message->append("i2cset -y 2 0x48 0x0 0x");
        message->append(QString::number(((abs(roundSpeed())) << 8 ) + (wheelNum << 2) + rMode, 16));
        message->append(" w");
        //qDebug() << message->toStdString().data();
        system(message->toStdString().data());

        /*char Data[10];
        char output[90] =  "i2cset -y 2 0x48 0x0 0x";
        sprintf(Data,"%x", ((abs(roundSpeed())) << 8 ) + (wheelNum << 2) + rMode);
        strcat(output,Data);
        strcat(output," w");
//        qDebug() << "Wheel #" << wheelNum << " speed is " << qRound(speed);
        system(output);*/

        break;
    }
}

void wheel::spin(qreal nspeed, qreal msecs)
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
    case SERV:
        if(duty_ns->write(QString::number((setDutyNs ((int) speed))).toStdString().data()) == -1)
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
    case DC:
        message->clear();
        message->append("i2cset -y 2 0x48 0x0 0x");
        message->append(QString::number(((abs((roundSpeed()))) << 8 ) + (wheelNum << 2) + rMode, 16));
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

quint16 wheel::roundSpeed()
{
    speed /= 10;
    speed = qRound(speed);
    speed *= 10;
    if(speed == 0)
	rMode = NEUTRAL;
    return speed;
}

void wheel::stopSlot()
{
    qDebug() << "Entered StopSlot";
    stop();
}
