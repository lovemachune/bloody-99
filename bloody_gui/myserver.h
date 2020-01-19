// myserver.h

#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include "mythread.h"
#include "mainwindow.h"
#include "gamethread.h"


class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = 0);
    GameThread *gameThread;
    void startServer();
    MainWindow *game_gui;


signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QObject *server_parent;

};

#endif // MYSERVER_H
