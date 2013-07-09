#ifndef GYRO_H
#define GYRO_H

#include <QObject>

//struct

class gyro : public QObject
{
    Q_OBJECT
public:
    explicit gyro(QObject *parent = 0);
    float getYaw();
    float getPitch();
    float getRoll();
    void setYaw(float newYaw);
    void setPitch(float newPitch);
    void setRoll(float newRoll);
private:
    float yaw;      //  Yaw
    float pitch;    //  Pitch
    float roll;     //  Roll

signals:
    
public slots:
    
};

#endif // GYRO_H
