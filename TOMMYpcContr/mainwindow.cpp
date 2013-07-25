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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    man = new QPainter(this);
    back = new QPixmap(":/res/img/Background.png");
//    top = new QPixmap(":/res/img/Foreground.png");
    sender = new QUdpSocket(this);
    address = new QHostAddress("192.168.0.105");
    port = 22;
    sender->bind(*address, port);

    man->drawPixmap(0, 0, *back);
    this->setFixedHeight(500);
    this->setFixedWidth(500);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete address;
    delete back;
    delete man;
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    qint32 rad = (event->pos().x() - 250) * (event->pos().x() - 250) +\
            (event->pos().y() - 250) * (event->pos().y() - 250);
    if(rad > 62500)
        return ;
    QByteArray * toSend = new QByteArray ();
    toSend->append(QString::number(event->pos().x() - 250));
    toSend->append(' ');
    toSend->append(QString::number(event->pos().y() - 250));
    toSend->append(' ');
    toSend->append(QString::number(rad));
    qDebug() << toSend->constData();
    sender->write(*toSend);
    delete toSend;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    qint32 rad = (event->pos().x() - 250) * (event->pos().x() - 250) +\
            (event->pos().y() - 250) * (event->pos().y() - 250);
    if (rad > 62500)
        return ;
    QByteArray * toSend = new QByteArray ();
    toSend->append(QString::number(event->pos().x() - 250));
    toSend->append(' ');
    toSend->append(QString::number(event->pos().y() - 250));
    toSend->append(' ');
    toSend->append(QString::number(rad));
    qDebug() << toSend->constData();
    sender->write(*toSend);
    delete toSend;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    QByteArray * toSend = new QByteArray();
    toSend->append("0 0 0");
    sender->write(*toSend);
    delete toSend;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    man->drawPixmap(0, 0, *back);
}
