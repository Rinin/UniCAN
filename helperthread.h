#ifndef HELPERTHREAD_H
#define HELPERTHREAD_H

#include <QThread>

class HelperThread : public QThread
{
    Q_OBJECT
public:
    explicit HelperThread(QObject *parent = 0);

public slots:
    static void usleep ( unsigned long usecs );
    static void msleep ( unsigned long msecs );
    
};

#endif // HELPERTHREAD_H
