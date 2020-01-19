#include "gamethread.h"
#define num_of_player 2

GameThread::GameThread(MainWindow *mw,QObject *parent) :
    QThread(parent)
{
    game_gui = mw;

}

void GameThread::run()
{
    // thread starts here
    qDebug() << "game Thread started";
    game_gui->players.clear();
    QTest::qWait(100);
    qDebug() <<"player size "<< game_gui->players.size();
    while(1){
        if(game_gui->players.size() == num_of_player) break;
    }
    qDebug() <<"player size "<< game_gui->players.size();
    game_gui->player_num = num_of_player;

    game_gui->initial();
    game_gui->g = true;
//    sem_init(&(game_gui->game_sem), 0, 1);
    game_gui->game_sem = new QSemaphore(1);

    while(1)
    {
        if(game_gui->turn_count==int(game_gui->mysem.size()))
            break;
    }
    cout<<"all player is ok"<<endl;
    game_gui->turn_flag = true;
    game_gui->turn_count = 0;
//    sem_init(&(game_gui->game_sem), 0, 0);
    while(game_gui->game_sem->available()){
        game_gui->game_sem->acquire();
    }
    qDebug() <<"game sem" << game_gui->game_sem->available() ;

    while(1)
    {
        game_gui->mysem[game_gui->turn_count]->release();
//        qDebug() << "waiting mysem " << game_gui->turn_count;
//        qDebug() << game_gui->mysem[game_gui->turn_count]->available() ;
        game_gui->game_sem->acquire();
//        qDebug() << game_gui->game_sem->available() ;
//        qDebug() << "game out" ;
        game_gui->offset = 0;
        stringstream iss(game_gui->rcv);
        cout<<iss.str()<<endl;
        while(iss>>command)
        {
            cout<<"command: "<<command<<endl;
            commands.push_back(command);
        }
        cout<<"commnads size "<<commands.size()<<endl;

        if(commands.size()>0){
            if(commands.size()>=3)
            {
                int sum=0;
                target = commands[commands.size()-1];
                commands.pop_back();
                for(unsigned long i=0 ; i<commands.size() ; i++)
                {
                    index = commands[i];
                    sum+=game_gui->players[game_gui->turn_count].get_hand(index).point;
                }
                for(unsigned long i=0 ; i<commands.size() ; i++)
                {
                    for(unsigned long j=i ; j<commands.size() ; j++)
                    {
                        if(commands[i]>commands[j])
                        {
                            int temp = commands[i];
                            commands[i] = commands[j];
                            commands[j] = temp;
                        }
                    }
                }
                temp_card.suit = '0';
                temp_card.point = sum;
                deal_flag = false;
            }
            else
            {
                index = commands[0];
                target = commands[1];
                commands.pop_back();
                temp_card = game_gui->players[game_gui->turn_count].get_hand(index);
                deal_flag = true;
            }
            for(unsigned long i=0 ; i<commands.size() ; i++)
            {
                index = commands[i]-i;
                game_gui->players[game_gui->turn_count].delCard(index);
            }
            game_gui->rule(temp_card, game_gui->turn_count, target, deal_flag);

            qDebug() << "player" << game_gui->turn_count << "acquire" << game_gui->mysem[game_gui->turn_count]->available();
            game_gui->mysem[game_gui->turn_count]->release();
            commands.clear();

            iss.str("");
            iss << game_gui->cards.get_point() << " " << game_gui->players[game_gui->turn_count].getString();
        }

        if(game_gui->offset!=0)
            game_gui->turn_count = game_gui->turn_count+game_gui->offset;
        else if(game_gui->turn_flag == true)
            game_gui->turn_count ++;
        else
            game_gui->turn_count --;

        if(game_gui->turn_count>=int(game_gui->players.size()))
            game_gui->turn_count = 0;
        if(game_gui->turn_count<0)
            game_gui->turn_count = game_gui->players.size()-1;
        emit game_gui->next_sig(game_gui->turn_count);
        QTest::qWait(1000);
        game_gui->game_sem->acquire();
    }
}
