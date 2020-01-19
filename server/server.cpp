#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include<semaphore.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "sockop.h"
#include "cards.h"

//Clubs : c, Diamond : d, Hearts : h, Spades : s;
using namespace std;

#define num_of_player 1
#define BUFSIZE 1024
pocker_card cards;
vector<player> players;
vector<pthread_t>threads;
vector<sem_t>mysem;
bool f = false, g = false;
char snd[BUFSIZE], rcv[BUFSIZE];
sem_t game_sem;
int turn_count, offset;
bool turn_flag; //clockwise = true, counterclockwise =false;

void *show(void *thread)
{
    pthread_exit(NULL);
}

void initial()
{
    cards.shuffel();
    cards.show();
    for (int i = 0; i < players.size(); i++)
    {
        for (int j = 0; j < cards_of_hand; j++)
        {
            cards.deal(&(players[i]));
        }
    }
}

void rule(card c, int i, int j, bool deal_flag)
{
    bool flag = true;
    cout<<"in rule "<<c.point<<endl;
    switch (c.point)
    {
        case 1:
            if(c.suit=='s')
                cards.set_point(0);
            else
                cards.set_point(1);
            break;
        case 4:
            turn_flag = !turn_flag;
            break;
        case 5:
            cout<<"in rule "<<j<<endl;
            offset=j;
            break;
        case 7:
            cards.change_one_card(&players[i], &players[j]);
            deal_flag = false;
            break;
        case 9:
            cards.change_all_card(&players[i], &players[j]);
            deal_flag = false;
            break;
        case 10:
            if(j>0)
                cards.set_point(10);
            else
                cards.set_point(-10);
            break;
        case 11:
            break;
        case 12:
            if(j>0)
                cards.set_point(20);
            else
                cards.set_point(-20);
            break;
        case 13:
            cards.set_point(99);
            break;
        default:
            cards.set_point(c.point);
            break;
    }
    if(deal_flag)
        cards.deal(&players[turn_count]);
}

void *player_turn(void *address)
{
    sem_t sem;
    sem_init(&sem, 0, 0);
    mysem.push_back(sem);
    int connfd = *(int*)address;
    int mynum = connfd-4;
    char ssnd[1024];
    player myplayer;
    players.push_back(myplayer);
    string s;
    while(1)
    {
        if(!g)
            continue;
        sprintf(ssnd, "Enter any char for ok %d", mynum);
        write(connfd, ssnd, strlen(ssnd));
        read(connfd, rcv, BUFSIZE);
        //printf("%s\n", rcv);
        memset(rcv, 0, BUFSIZE);
        break;
    }
    cout<<"out"<<endl;
    sem_wait(&game_sem);
    turn_count++;
    printf("count = %d, %d\n", turn_count, mynum);
    sem_post(&game_sem);
    while(1)
    {
        sem_wait(&mysem[mynum]);
        cout<<"****in "<<mynum<<endl;
        s = players[mynum].getString();
        sprintf(snd, "%s", s.c_str());
        write(connfd, snd, strlen(snd));
        memset(rcv, 0, BUFSIZE);
        read(connfd, rcv, BUFSIZE);
        sem_post(&game_sem);
        sem_wait(&mysem[mynum]);
        if(players[mynum].get_total_hand().size()==0 || cards.get_point()>99)
        {
            cout<<"you out\n";
            sprintf(snd, "game over");
            write(connfd, snd, strlen(snd));
            players.erase(players.begin()+mynum);
            mysem.erase(mysem.begin()+mynum);
            break;
        }
    }
}

void *total_game(void *data)
{
    vector<int>commands;
    int command;
    card temp_card;
    int index,target;
    bool deal_flag;
    while(1)
    {
        if(players.size()>=num_of_player+1)
            break;
    }
    initial();
    g = true;
    sem_init(&game_sem, 0, 1);
    cout<<mysem.size()<<endl;
    //wait for all player is ok;
    while(1)
    {
        if(turn_count==mysem.size())
            break;
    }
    cout<<"all player is ok"<<endl;
    turn_flag = true;
    turn_count = 0;
    sem_init(&game_sem, 0, 0);
    while(1)
    {
        sem_post(&mysem[turn_count]);
        sem_wait(&game_sem);
        offset = 0;
        stringstream iss(rcv);
        cout<<iss.str()<<endl;
        while(iss>>command)
        {
            cout<<"command: "<<command<<endl;
            commands.push_back(command);
        }
        cout<<"commnads size "<<commands.size()<<endl;
        if(commands.size()>=3)
        {
            int sum=0;
            target = commands[commands.size()-1];
            commands.pop_back();
            for(int i=0 ; i<commands.size() ; i++)
            {
                index = commands[i];
                sum+=players[turn_count].get_hand(index).point;
            }
            for(int i=0 ; i<commands.size() ; i++)
            {
                for(int j=i ; j<commands.size() ; j++)
                {
                    if(commands[i]>commands[j])
                    {
                        int temp = commands[i];
                        commands[i] = commands[j];
                        commands[j] = commands[i];
                    }
                }
            }
            temp_card.suit = 'c';
            temp_card.point = sum;
            deal_flag = false;
        }
        else
        {
            index = commands[0];
            target = commands[1];
            commands.pop_back();
            temp_card = players[turn_count].get_hand(index);
            deal_flag = true;
        }
        for(int i=0 ; i<commands.size() ; i++)
        {
            index = commands[i]-i;
            players[turn_count].delCard(index);
        }
        rule(temp_card, turn_count, target, deal_flag);
        sem_post(&mysem[turn_count]);
        commands.clear();

        cout<<"point :"<<cards.get_point()<<endl;
        cout<<"it is "<<turn_count<<" turn"<<endl;
        cout<<"turn flag "<<turn_flag<<endl;

        sprintf(snd, "%d %s", cards.get_point(),players[turn_count].getString().c_str());
       // write(turn_count+4, snd, strlen(snd));

        if(offset!=0)
            turn_count = turn_count+offset;
        else if(turn_flag == true)
            turn_count++;
        else
            turn_count--;
        
        cout<<"offset "<<offset<<endl;
        cout<<"turn count now is "<<turn_count<<endl;

        if(turn_count>=players.size())
            turn_count = 0;
        if(turn_count<0)
            turn_count = players.size()-1;

    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    players.clear();
    threads.clear();
    mysem.clear();
    turn_count = 0;
    int *new_client, sockfd;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    size_t len=0;
    ssize_t read;
    sockfd = passivesock(argv[1], "tcp", 30);
    pthread_t game;
    int rc = pthread_create(&game, NULL, total_game, (void*)(-1));
    while(1)
    {
        new_client = new int;
        *new_client = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        printf("new connection:%d\n", *new_client);
        pthread_t new_th;
        threads.push_back(new_th);
        rc = pthread_create(&threads.back(), NULL, player_turn, (void *)new_client);
   }
}