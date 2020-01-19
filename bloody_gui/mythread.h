// mythread.h

#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <vector>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "mainwindow.h"
#include "cards.h"
#include <QSemaphore>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(qintptr ID, MainWindow *gui, QObject *parent = 0);
    void run();
    void play_card(int n);
    void change_set(int plid);
    void show_circle(bool cw);
    void next_player(int plid);

    MainWindow *game_gui;


signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    int my_id;
    player myplayer;
    QSemaphore *sem;

};

#endif // MYTHREAD_H
