#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QFile>
#include <qmath.h>

const float Pi = 3.1415926535;

struct vector
{
    int x, y;
};

struct matrix
{
    int _11;
    int _12;
    int _21;
    int _22;
};

vector operator*(matrix a, vector b) const;
vector operator+(vector a, vector b) const;
inline int operator*(vector a, vector b) const;
vector normalize (vector a) const;

class wheel : public QObject
{
    Q_OBJECT
public:
    explicit wheel(int wheelNumber, vector guiding, QObject *parent = 0);
    void stop();
    void spin(float speed);
    void spin(float speed, float msecs);
    void setGuide(vector x);
    vector getGuide();

private:
    QFile temp; //  Serie of files that controls the drives
    vector guide;
    void spinForw(float speed);
    void spinBackw(float speed);
    void spinForw(float speed, float msecs);
    void spinBackw(float speed, float msecs);

signals:
    
public slots:
    
};

#endif // WHEEL_H
