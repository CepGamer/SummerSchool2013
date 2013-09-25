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
#include <QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QByteArray>
#include <QDataStream>

int main (int argc, char ** argv)
{
    QCoreApplication * a = new QCoreApplication (argc, argv);
    /*QTcpServer * reciever = new QTcpServer;
    QByteArray * buffer;
    QDataStream * numbers;*/
//    int toRet;
    Cyber * b = new Cyber();
    qDebug() << "Engines is going to run";
    b->startOMNI();

/*    switch (type) {
    case ANDROID_CONTROL:

        break;
    case AUTO_MODE:

        break;
    case PC_CONTROL:
        while(true)
        {
            if (numbers != NULL)
                delete numbers;
            reciever = new QUdpSocket();
            reciever->connectToHost("192.168.0.1", 22);
            numbers = new QDataStream(reciever->read(16));
            *numbers >> c.x >> c.y >> x;
            qDebug() << c.x << '\t' << c.y << 't' << x;
            b->moveByVector(30 + 40 * sqrt(x / 125000.0), normalize(c));
        }
        break;
    default:
        while(scanf("%f", &x))
            switch(qRound(x))
            {
                case 0:
                    b->stop();
                    break;
                case 360:
                    b->~Cyber();
                    break;
                case 500:
                    b->turn(15);
                    break;
                default:
                    qDebug() << "Degree is:\t" << qRound(x);
                    c.x = cos ( x * Pi / 180);
                    c.y = sin ( x * Pi / 180);
                    qDebug() << "Moving vector is:\t" << (qreal)c.x << '\t' << (qreal)c.y;
                    b->moveByVector(50, normalize(c));
                break;
            }
        break;
    }*/
    /*else if (type == PC_CONTROL)
    {
        while(true)
        {
            if (numbers != NULL)
                delete numbers;
            if(buffer != NULL)
                delete buffer;
            reciever->listen(QHostAddress("192.168.0.126"), 22);
            reciever->nextPendingConnection();
            buffer = new QByteArray(reciever->read(16));
            if(buffer && buffer->isEmpty())
                continue;
            numbers = new QDataStream(*buffer);
            *numbers >> c.x >> c.y >> x;
            qDebug() << c.x << '\t' << c.y << '\t' << x;
            b->moveByVector(30 + 40 * sqrt(x / 125000.0), normalize(c));
        }
    }*/
//    delete b;
//    qDebug() << "Exited program";
    return a->exec();
}
