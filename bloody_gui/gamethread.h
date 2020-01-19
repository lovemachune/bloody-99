#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>
#include "mainwindow.h"

using namespace std;

class GameThread : public QThread
{
    Q_OBJECT
public:
    explicit GameThread(MainWindow *mw,QObject *parent = 0);

    void run();
    void play_card(int plid, int n);

    MainWindow *game_gui;

signals:

public slots:

private:
    vector<int> commands;
    int command;
    card temp_card;
    int index,target;
    bool deal_flag;

};

#endif // GAMETHREAD_H
