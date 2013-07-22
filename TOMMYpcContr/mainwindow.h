#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPainter>
#include <QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QPixmap * back;
    QPainter * man;
    QUdpSocket * sender;
    QHostAddress * address;
    quint16 port;

public slots:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

};

#endif // MAINWINDOW_H
