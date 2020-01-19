// mythread.cpp

#include "mythread.h"

MyThread::MyThread(qintptr ID, MainWindow *gui, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    game_gui = gui;
    my_id = game_gui->addplayer();
}

void MyThread::run()
{
    // thread starts here
    qDebug() << " Thread started ";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }
    qDebug() << "socket descriptor " << this->socketDescriptor;

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << socketDescriptor << " Client connected";


    stringstream ss;
    sem = new QSemaphore();
    game_gui->mysem.push_back(sem);
    game_gui->players.push_back(myplayer);

    while(1)
    {
        if(game_gui->g==false)
            continue;
        ss << "Enter any char for ok "<<my_id<<endl;
        socket->write(QString(ss.str().c_str()).toUtf8());
        socket->waitForReadyRead(-1);
        QByteArray data =  socket->readAll();

        break;
    }

    game_gui->game_sem->acquire();

    game_gui->turn_count++;


    game_gui->game_sem->release();

    ss.str("");
    ss << game_gui->players[my_id].getString();
    socket->write(ss.str().c_str());
    cout << "to player id:" << my_id << " card:" << ss.str() << endl;
    QTest::qWait(1000);


    while(1){
        game_gui->mysem[my_id]->acquire();
        if(game_gui->players[my_id].get_total_hand().size()==0){
            game_gui->player_num --;
            game_gui->rcv="";
            game_gui->game_sem->release();
            ss.str("");
            ss << "game over";
            socket->write(ss.str().c_str());
            game_gui->game_sem->release();
            break;
        }

        if(game_gui->player_num==1){
            cout << "you fucking win" << endl;
            emit game_gui->end_game_sig();
        }

        cout<<"in "<<my_id<<endl;
        ss.str("");
        ss << game_gui->cards.get_point() << " " << game_gui->players[my_id].getString();
        socket->write(ss.str().c_str());
        QTest::qWait(500);
        cout << "send to id:" << my_id << " card:" << ss.str() << endl;

        socket->waitForReadyRead(-1);
        QByteArray data = socket->readAll();
        QTest::qWait(500);
        socket->write("garbage");

        game_gui->rcv = "";
        game_gui->rcv = string(data.constData(),data.length());
        cout << "rcv " << game_gui->rcv << endl;
        stringstream iss(game_gui->rcv);
        int command;
        vector<int> commands;
        while(iss>>command)
        {
            cout<<"command: "<<command<<endl;
            commands.push_back(command);
        }
        commands.pop_back();
        for(unsigned long i=0;i<commands.size();i++){
            play_card(commands[i]-i);
        }

        game_gui->game_sem->release();
        game_gui->mysem[my_id]->acquire();

        ss.str("");
        ss << game_gui->cards.get_point() << " " << game_gui->players[my_id].getString();
        socket->write(ss.str().c_str());
        QTest::qWait(500);
        socket->write("garbage");
        QTest::qWait(500);

        if(game_gui->players[my_id].get_total_hand().size()==0 || game_gui->cards.get_point()>99){
            game_gui->player_num --;
            game_gui->game_sem->release();
            ss.str("");
            ss << "game over";
            socket->write(ss.str().c_str());
            break;
        }
        else{
            game_gui->game_sem->release();
        }


    }

    while(1){
        game_gui->mysem[my_id]->acquire();
        game_gui->rcv = "";
        game_gui->game_sem->release();
    }


    exec();
}

void MyThread::play_card(int n)
{
    player_gui &pl = game_gui->gui_players[my_id];
    qDebug() << "play " <<  game_gui->gui_players[my_id].card_set[n].number;
    emit game_gui->show_card_sig(my_id,n);
    QTest::qWait(1000);
    pl.card_set[n].pos.tx = 600;
    pl.card_set[n].pos.ty = 250;
    QTest::qWait(1500);
    emit game_gui->remove_card_sig(my_id,n);
    QTest::qWait(500);
    emit game_gui->change_set(my_id,my_id);


}

void MyThread::change_set(int plid)
{
    player_gui &pl = game_gui->gui_players[my_id];
    player_gui &target_pl = game_gui->gui_players[plid];


    QTest::qWait(1000);
    for(unsigned long i=0;i<pl.card_set.size();i++){
        pl.card_set[i].pos.tx = target_pl.card_set[0].pos.ox;
        pl.card_set[i].pos.ty = target_pl.card_set[0].pos.oy;
    }
    for(unsigned long i=0;i<target_pl.card_set.size();i++){
        target_pl.card_set[i].pos.tx = pl.card_set[0].pos.ox;
        target_pl.card_set[i].pos.ty = pl.card_set[0].pos.oy;
    }
    QTest::qWait(1000);
    game_gui->change_set(my_id,plid);

    for(unsigned long i=0;i<pl.card_set.size();i++){
        pl.card_set[i].pos.tx = pl.card_set[i].pos.ox;
        pl.card_set[i].pos.ty = pl.card_set[i].pos.oy;
    }
    for(unsigned long i=0;i<target_pl.card_set.size();i++){
        target_pl.card_set[i].pos.tx = target_pl.card_set[i].pos.ox;
        target_pl.card_set[i].pos.ty = target_pl.card_set[i].pos.oy;
    }


}

void MyThread::show_circle(bool cw)
{
    emit game_gui->show_circle_sig(cw);
    QTest::qWait(2000);
    emit game_gui->close_circle_sig();
}

void MyThread::next_player(int plid)
{
    emit game_gui->next_sig(plid);
    QTest::qWait(2000);
}


void MyThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";


    socket->deleteLater();
    exit(0);
}
