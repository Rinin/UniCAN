#include "helperthread.h"

HelperThread::HelperThread(QObject *parent) :
    QThread(parent)
{
}

void HelperThread::usleep(unsigned long usecs)
{
    QThread::usleep(usecs);
}

void HelperThread::msleep(unsigned long msecs)
{
    QThread::msleep(msecs);
}
