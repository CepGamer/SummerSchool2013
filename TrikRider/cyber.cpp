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

#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    settings = new QFile("settings");
//    curr = new gyro_pos;
    gyro = new Gyroscope(this);
    mainTimer = new QTimer(this);
    wheels = new QList<wheel *>();
    conServer = new QTcpServer ();
    conSocket = new QTcpSocket ();
    position.x = position.y = \
            direction.x = direction.y = 0;      //  Resetting values
//            acceleration.x = acceleration.y = 0;
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
    matrix toRet;       //  Set spin matrix
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
    delete mainTimer;
    delete wheels;
    delete guide;
    delete gyro;
}

void Cyber::turn (qreal degree)
{
    if (degree > 0)
    {
        turnRight(degree);      //  If we turn
    }
    else if (degree < 0)
        turnLeft(-degree);
}

void Cyber::stop()
{
    for (int i = 0; i < wheelSize; i++)
        wheels->at(i)->stop();      //  Fullstop
}

void Cyber::startOMNI()
{
    if(settings->exists())
    {
        char temp;
        qDebug() << "Load previous settings? Y/N";
        temp = getchar();
        if(temp == 'y' || temp == 'Y')
            loadFromSaved();
        else if (temp != 'n' & temp != 'N')
            qDebug() << "Please press either 'Y' or 'N'";
        else setSettings();
    } else setSettings();

    gyro->setConnection();          //  Start reading gyro
    calibrate();                    //  Calibrating gyro (& accel, when ready (& if needed))
}

void Cyber::moveByVector(vector toMove)
{
    currRad = direction.x = direction.y = 0;
    moving.x = toMove.x;
    moving.y = toMove.y;        //  Resetting values

    for (int i = 0; i < wheelSize; i++)
        wheels->at(i)->spin((moving * guide[i]) * 50);      //  Get those wheels spinning
    //  Will add code when the gyro & accel ready
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(moveVectorSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

/*void Cyber::firstLaunch()
{
    //  This is the main func to provide first user experience. Must be somehow rewritten
    direction.x = direction.y = 0;
//    gyro->setConnection();
    //  Set calibration
    integrand = count = 0;
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(firstLaunchSlot()));
    mainTimer->start(1000 / checksPerSecond);
}*/

void Cyber::turnLeft(qreal degree)
{
    //qDebug() << "Start engines";
/*    wheels->at(0)->spin(100);   //  spinning dem wheels
    wheels->at(1)->spin(100);
    wheels->at(2)->spin(100);*/
    leftRad = degree / 180 * Pi;
    //  Again, some code after gyro
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(turnLeftSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::turnRight(qreal degree)
{
    //qDebug() << "Start engines";
    /*wheels->at(0)->spin(-100);
    wheels->at(1)->spin(-100);
    wheels->at(2)->spin(-100);*/
    leftRad = degree / 180 * Pi;
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(turnRightSlot())); //  Start timer
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::calibrate()
{
    qDebug () << "Sensor Calibration Initiated";
    count = correction = integrand = 0; //  Tune the calibration
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::loadFromSaved()
{
    settings->open(QFile::ReadOnly);
    wheelSize = settings->readLine().toInt();
    wheelMode = (wheelType)settings->readLine().toInt();
    controlMode = (controlType)settings->readLine().toInt();
    setWheels();
}

void Cyber::setSettings()
{
    QString * message = new QString ();
    if(!settings->open(QFile::WriteOnly))
    {
        qDebug() << "Something went wrong!";
        return;
    }
    qDebug() << "How many wheels do you have on your OMNI?";
    wheelSize = getchar() - '0';
    guide = new vector[wheelSize];                  //  Guiding vectors for all

    qDebug() << "Enter type of wheels: (D)C or (S)ervo?";
    char temp = getchar();
    if(temp == 'D' || temp == 'd')
        wheelMode = DC;
    else if (temp == 'S' || temp == 's')
        wheelMode = SERV;
    else qDebug() << "Please press either 'D' or 'C'";

    qDebug() << "Enter type of connection: (A)ndroid, (P)C or A(u)to control?";
    temp = getchar();
    if(temp == 'A' || temp == 'a')
        controlMode = ANDROID_CONTROL;
    else if (temp == 'P' || temp == 'p')
        controlMode = PC_CONTROL;
    else if (temp == 'U' || temp == 'u')
        controlMode = AUTO_MODE;
    else qDebug() << "Please press either 'A', 'P' or 'u'";

    message->append(QString::number(wheelSize) + '\n');
    message->append(QString::number(wheelMode) + '\n');
    message->append(QString::number(controlMode) + '\n');

    if(settings->write(message->toStdString().data()) == -1)
        qDebug() << "Something went wrong!";
    setWheels();
}

void Cyber::setWheels()
{
    for (int i = 0; i < wheelSize; i++)
    {
        guide[i].x = cos(i * 2 * Pi / wheelSize - Pi / 2);
        guide[i].y = sin(i * 2 * Pi / wheelSize - Pi / 2);
        wheels->append( new wheel (i, wheelMode));
    }
}

void Cyber::checkPosition()
{
    vector mov;
    qreal angVelocityC = angVelocity / ((wheels->at(1)->getSpeed() > 75) ? 2 : 1);  //  Get speed - divide speed in 2 types
    vector on50percentSpeed;
//    currRad = currRad % (2 * Pi);
    on50percentSpeed.x = qCos(angVelocityC * 2 * Pi);
    on50percentSpeed.y = qSin(angVelocityC * 2 * Pi);
    mov.x = mov.y = 0;
    for (int i = 0; i < wheelSize; i++)
        mov = mov + guide[i] * (wheels->at(i)->getSpeed() / 100);
/*    for (int i=0; i<3; i++)
        angVelocity += wheels->at(i)->getSpeed();
        angVelocity = convertSpeed();   //  convert percents into speed. Or based on an encoder.
*/
    previous.tiltX = angles.tiltX;
    previous.tiltY = angles.tiltY;
    previous.tiltZ = angles.tiltZ;
//    gyro->readGyroEvent();
    angles.tiltX = gyro->getTiltX();  //  Get gyro statements
    angles.tiltY = gyro->getTiltY();
    angles.tiltZ = gyro->getTiltZ();
/*    qDebug() << "Pos is:\t" << position.x << '\t' << position.y << '\n'\
             << "Dir is:\t" << direction.x << '\t' << direction.y;
    position = position + ( (kalmanCoef / (checksPerSecond * checksPerSecond * 2)) * acceleration \
                            + ((1 - kalmanCoef) / checksPerSecond) * (moving));*/
    //  Kalman filter values
    direction = direction + \
            (kalmanCoef / checksPerSecond) * (setAngle( angles.tiltZ - correction ) * direction) + \
            ((1 - kalmanCoef) / checksPerSecond ) * (on50percentSpeed + direction);
    //  Change direction
    absolute.tiltZ = kalmanCoef * (angles.tiltZ - correction) + (1 - kalmanCoef) * angVelocityC;
    currRad += absolute.tiltZ / checksPerSecond;
}

void Cyber::moveVectorSlot()
{
    count++;
    count %= checksPerSecond;

    checkPosition();
//    moving = normalize(setAngle(-currRad) * moving);     //  Calibrate moving vector

    if(count == 0)
    {
        for (int i = 0; i < wheelSize; i++)
            wheels->at(i)->spin((normalize(setAngle(-currRad) * moving) * guide[i]) * 80);  //  Get those wheels spinning
    }
}

void Cyber::turnLeftSlot()
{
    if(leftRad < 0.2)
    {
        mainTimer->stop();
        mainTimer->disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(turnLeftSlot()));
        stop();
    }
    else if(leftRad < 2 * Pi)
        for(int i = 0; i < wheelSize; i++)
            wheels->at(i)->spin(50);
    checkPosition();
    leftRad -= qAbs(absolute.tiltZ / checksPerSecond);
}

void Cyber::turnRightSlot()
{
    count++;
    count %= checksPerSecond;
    if(count == 0)
        qDebug() << "leftRad is:\t" << QString::number(leftRad, 'f');
    if(leftRad < 0.2)
    {
        mainTimer->stop();
        disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(turnRightSlot()));
        stop();
    }
    else if(leftRad < 2 * Pi)
        for(int i = 0; i < wheelSize; i++)
            wheels->at(i)->spin(-50);
    checkPosition();
    leftRad -= qAbs(absolute.tiltZ / checksPerSecond);
}

void Cyber::calibrateSlot()
{
    count++;
    if (count == (2 * checksPerSecond))
    {
        mainTimer->stop();
        disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
        correction = integrand / (2 * checksPerSecond);
        integrand = 0;
        qDebug () << "Calibration is over, we are ready to roll!\nCorrection is:\t" << correction;
//        moving.x = 1;
//        moving.y = 0;
//        moveByVector(moving);
    }
    checkPosition();
    integrand += (angles.tiltZ + previous.tiltZ);
}

/*void Cyber::firstLaunchSlot()
{
    count++;
    count %= checksPerSecond;
    if(count == 0)
        qDebug() << "Integrand is:\t" << QString::number(integrand, 'f');
    checkPosition();
    integrand += (absolute.tiltZ + angles.tiltZ) / (checksPerSecond * 2);
}
*/

void Cyber::movingState()
{

}

void Cyber::pauseState()
{
    stop();
}
