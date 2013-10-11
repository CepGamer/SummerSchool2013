/*
Copyright 2013 Bolotov Sergey

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "wheel.h"

wheel::wheel(int wheelNumber, QObject *parent) :
    QObject(parent)
{
    wheelNum = wheelNumber;
    QString path("/sys/class/pwm/ecap." + QString::number(wheelNum) + "/");  //  Get full path to engine
    request = new QFile (path + QString("request"));

    if(!request->open(QFile::WriteOnly))
    {
        qDebug() << "Cannot open request #" << wheelNumber;
    }

    if(request->write("1") == -1 || !request->flush())
    {
        qDebug() << "Permission is not given to engine #" << wheelNumber;
    }

    period_ns = new QFile (path + QString("period_ns"), this);
    duty_ns = new QFile (path + QString("duty_ns"), this);
    run = new QFile (path + QString("run"), this);

    if(!(period_ns->open(QFile::WriteOnly) && duty_ns->open(QFile::WriteOnly) && run->open(QFile::WriteOnly)))
    {
        qDebug() << "Cannot open either run or duty or period #" << wheelNumber;
    }

    setPeriod(20000000);
}

wheel::~wheel()
{
    if (run != NULL)
    {
        run->write("0");
        run->close();
        delete run;
    }
    if (duty_ns != NULL)
        delete duty_ns;
    if (period_ns != NULL)
        delete period_ns;
    if (request != NULL)
    {
        request->write("0");
        delete request;
    }
}

void wheel::stop()
{
    rMode = NEUTRAL;
    run->write("0");
    if(!run->flush())
    {
        qDebug() << "Run isn't flushing, cannot stop.";
    }
}

void wheel::spin(qreal nspeed)
{
    speed = nspeed;
    if (speed > 100)
        speed = 100;
    else if (speed < -100)
        speed = -100;
    rMode = (speed > 0) ? FORW : BACKW;
    duty_ns->write(QString::number(setDutyNs ((int) speed)).toStdString().data());
    run->write("1");
    duty_ns->flush();
    run->flush();
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

void wheel::setPeriod(quint32 per)
{
    period_ns->write(QString::number(per));
}
