#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include <iostream>
#include <sstream>
#include <QTransform>
#include <QTimer>
#include <QtNetwork>
#include <QTest>
#include <semaphore.h>
#include "cards.h"
#include <QSemaphore>

using namespace std;

typedef struct cpos{
    int ox;
    int oy;
    int x;
    int y;
    int vx;
    int vy;
    int tx;
    int ty;
}card_pos;

typedef struct{
    int number;
    int color;
    bool cover;
    card_pos pos;
    QLabel *back;
    QLabel *image;
}card_gui;

typedef struct{
    string name;
    card_pos init_pos;
    vector<card_gui> card_set;
    vector<card_pos> pose_set;
}player_gui;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int addplayer();
    void initial();
    void rule(card c, int i, int j, bool deal_flag);


    vector<player_gui> gui_players;
    vector<card_pos> init_pos;
    vector<string> init_name;

    pocker_card cards;
    vector<player> players;
    vector<QSemaphore *> mysem;
    bool f = false;
    bool g = false;
    QSemaphore *game_sem;
    int turn_count=0, offset=0;
    bool turn_flag=true;
    string rcv="";
    int player_num=0;

//    void play_card(player_gui &pl,int n);

signals:
    void show_card_sig(int plid,int n);
    void cover_card_sig(int plid,int n);
    void remove_card_sig(int plid,int n);
    void assign_card_sig(int plid, int color, int num);
    void assign_card_sig(int plid, int color, int num,int x, int y);
    void change_card_sig(int pl1, int c1, int pl2, int c2);
    void change_set_sig(int plid1, int plid2);
    void show_circle_sig(bool cw);
    void close_circle_sig();
    void next_sig(int plid);
    void end_game_sig();


public slots:
    void timer_slot();
    void show_card(int plid,int n);
    void cover_card(int plid,int n);
    void remove_card(int plid,int n);
    void assign_card(int plid, int color, int num);
    void assign_card(int plid, int color, int num,int x, int y);
    void change_card(int pl1, int c1, int pl2, int c2);
    void change_set(int plid1, int plid2);
    void show_circle(bool clockwise);
    void close_circle();
    void next(int plid);
    void end_game();


private:
    Ui::MainWindow *ui;
    QVector<QLabel*> image_label;
    QTimer *timer;
    QLabel *direction_circle;
    QPixmap *pix_d1;
    QLabel *next_direct;
    QLabel *num_label;
    QLabel *end_label;
    QPixmap *pix_direct;
    QTcpServer *tcpServer;
    vector<int> pl_rotate;
    int card_hight, card_width;
    int player_count;
    bool ccw;
    bool cw;
    bool show_next;
    int cc_count;
    int n_count;
    int next_plid;

    void construct_card_set(int plid);


};
#endif // MAINWINDOW_H
