#include "gyro.h"

gyro::gyro(QObject *parent) :
    QObject(parent)
{
}

float gyro::getYaw()
{
    return yaw;
}

float gyro::getPitch()
{
    return pitch;
}

float gyro::getRoll()
{
    return roll;
}

void gyro::setYaw(float newYaw)
{
    yaw = newYaw;
}

void gyro::setPitch(float newPitch)
{
    pitch = newPitch;
}

void gyro::setRoll(float newRoll)
{
    roll = newRoll;
}
