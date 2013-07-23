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
