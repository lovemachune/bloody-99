#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sockop.h"

void delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game_sem = new QSemaphore(1);
    num_label = new QLabel(this);
    num_label->setGeometry(30,20,300,100);
    num_label->setFont(QFont("Bradley Hand" , 50 ,  QFont::Bold));
    num_label->setText("bloody 99");
    num_label->show();

    end_label = new QLabel(this);
    end_label->setGeometry(500,250,500,100);
    end_label->setFont(QFont("Bradley Hand" , 50 ,  QFont::Bold));
    end_label->setText("Somebody Win");
    end_label->hide();

    char cards[]="CDHS";

    for (int i=0;i<52;i++) {
        stringstream cardName;
        cardName << ":/img/images/card/" << i%13+1 << cards[i/13] << ".png";
        cout << cardName.str()  << endl;
        QLabel *img = new QLabel(this);
        QTransform trans;
        trans.rotate(0);
        QPixmap pix(cardName.str().c_str());
        pix = pix.transformed(trans);
        img->setPixmap(pix);
        card_hight = pix.height();
        card_width = pix.width();
        img->setGeometry(10*i,10*i,card_width,card_hight);
        img->setScaledContents(true);
        img->hide();
        image_label.push_back(img);
    }
    player_count = 0;
    card_pos init1,init2,init3;
    init1.x = 800;
    init1.y = 5;
    init2.x = 400;
    init2.y = 550;
    init3.x = 1200;
    init3.y = 550;
    init_pos.push_back(init1);
    init_pos.push_back(init2);
    init_pos.push_back(init3);
    init_name.push_back("kaikai");
    init_name.push_back("jikai");
    init_name.push_back("ray");
    pl_rotate.push_back(45);
    pl_rotate.push_back(145);
    pl_rotate.push_back(-90);

    direction_circle = new QLabel(this);
    pix_d1 = new QPixmap(":/img/images/arrow_dierction.png");
    direction_circle->setPixmap(*pix_d1);
    direction_circle->setScaledContents(true);
    direction_circle->setGeometry(560,300,250,250);
    direction_circle->hide();
    ccw = false;
    cw = false;
    cc_count = 0;
    show_next = false;
    n_count = 0;
    next_direct = new QLabel(this);
    pix_direct = new QPixmap(":/img/images/arrow2.png");
    next_direct->setPixmap(*pix_direct);
    next_direct->setScaledContents(true);
    next_direct->setGeometry(560,300,250,250);
    next_direct->hide();

    connect(this,SIGNAL(show_card_sig(int,int)),SLOT(show_card(int,int)));
    connect(this,SIGNAL(remove_card_sig(int,int)),SLOT(remove_card(int,int)));
    connect(this,SIGNAL(cover_card_sig(int,int)),SLOT(cover_card(int,int)));
    connect(this,SIGNAL(assign_card_sig(int,int,int)),SLOT(assign_card(int,int,int)));
    connect(this,SIGNAL(assign_card_sig(int,int,int,int,int)),SLOT(assign_card(int,int,int,int,int)));
    connect(this,SIGNAL(change_set_sig(int,int)),SLOT(change_set(int,int)));
    connect(this,SIGNAL(show_circle_sig(bool)),SLOT(show_circle(bool)));
    connect(this,SIGNAL(close_circle_sig()),SLOT(close_circle()));
    connect(this,SIGNAL(next_sig(int)),SLOT(next(int)));
    connect(this,SIGNAL(end_game_sig()),SLOT(end_game()));


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()) ,this, SLOT(timer_slot()));
    timer->start(30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::addplayer()
{

    player_gui pl1;
    pl1.name = init_name.back();
    init_name.pop_back();
    int initx = init_pos.back().x;
    int inity = init_pos.back().y;
    pl1.init_pos = init_pos.back();
    init_pos.pop_back();

    for(int i=0;i<5;i++){
        card_pos cpos;
        cpos.x = initx - i*100;
        cpos.y = inity;
        cpos.ox = cpos.x;
        cpos.oy = cpos.y;
        cpos.tx = cpos.x;
        cpos.ty = cpos.y;
        pl1.pose_set.push_back(cpos);
    }

    gui_players.push_back(pl1);

    return player_count++;
}

void MainWindow::initial()
{
    cards.shuffel();
//    cards.show();
    qDebug()<< "assign card";
    for (unsigned long i = 0; i < players.size(); i++)
    {
        for (int j = 0; j < cards_of_hand; j++)
        {
            cards.deal(&(players[i]));
            QTest::qWait(200);
            emit assign_card_sig(i,players[i].get_hand(j).suit-48,players[i].get_hand(j).point);
        }
    }
}

void MainWindow::rule(card c, int i, int j, bool deal_flag)
{
    cout<<"in rule "<<c.point<<endl;
    int idx;
    if(c.point==7 || c.point==9){
        j = i+j;
        j = (j+players.size())%players.size();
    }
    player_gui &pl = gui_players[i];
    player_gui &target_pl = gui_players[j];
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
            emit show_circle_sig(turn_flag);
            QTest::qWait(1000);
            emit close_circle_sig();
            break;
        case 5:
            cout<<"in rule "<<j<<endl;
            offset=j;
            break;
        case 7:
            idx = cards.change_one_card(&players[i], &players[j]);
            gui_players[j].card_set[idx].pos.tx = gui_players[i].pose_set.back().ox;
            gui_players[j].card_set[idx].pos.ty = gui_players[i].pose_set.back().oy;
            QTest::qWait(1000);
            emit assign_card_sig(i,players[i].get_total_hand().back().suit-48,players[i].get_total_hand().back().point,pl.pose_set.back().x-10,pl.pose_set.back().y-10);
            emit remove_card_sig(j,idx);
            QTest::qWait(100);
            emit change_set(j,j);
            deal_flag = false;
            break;
        case 9:
            cards.change_all_card(&players[i], &players[j]);

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
            change_set(i,j);

            for(unsigned long i=0;i<pl.card_set.size();i++){
                pl.card_set[i].pos.tx = pl.card_set[i].pos.ox;
                pl.card_set[i].pos.ty = pl.card_set[i].pos.oy;
            }
            for(unsigned long i=0;i<target_pl.card_set.size();i++){
                target_pl.card_set[i].pos.tx = target_pl.card_set[i].pos.ox;
                target_pl.card_set[i].pos.ty = target_pl.card_set[i].pos.oy;
            }
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
    qDebug() << "switch out";

    if(deal_flag){
        cards.deal(&players[turn_count]);
        card cd = players[turn_count].get_total_hand().back();
        emit assign_card_sig(turn_count,cd.suit-48,cd.point);
        QTest::qWait(500);
    }
    qDebug() << "rule out";
    num_label->setText(QString::number(cards.get_point()));
    num_label->show();
    return ;
}

void MainWindow::timer_slot()
{
    for(unsigned long i=0;i<gui_players.size();i++){
        for(unsigned long n=0;n<gui_players[i].card_set.size();n++){
            card_pos *cpos = &gui_players[i].card_set[n].pos;
            QLabel *image = gui_players[i].card_set[n].image;
            QLabel *back = gui_players[i].card_set[n].back;
            cpos->vx = (cpos->tx - cpos->x)/10;
            cpos->vy = (cpos->ty - cpos->y)/10;
            cpos->x += cpos->vx;
            cpos->y += cpos->vy;
            image->setGeometry(cpos->x,cpos->y,card_width,card_hight);
            back->setGeometry(cpos->x,cpos->y,card_width,card_hight);
        }
    }

    if(cw){
        cc_count ++;
        QTransform trans;
        trans.rotate(cc_count*2%360);
        trans.scale(10.0/(10+cc_count),10.0/(10+cc_count));
        direction_circle->setPixmap(pix_d1->transformed(trans));
        direction_circle->show();
    }else if(ccw){
        cc_count ++;
        QTransform trans;
        trans.rotate(cc_count*-2%360);
        trans.scale(10.0/(10+cc_count),-10.0/(10+cc_count));
        direction_circle->setPixmap(pix_d1->transformed(trans));
        direction_circle->show();
    }

    if(show_next){
        n_count ++;
        QTransform trans;
        trans.rotate(pl_rotate[next_plid]);
        next_direct->setPixmap(pix_direct->transformed(trans));
        next_direct->show();

        if(n_count > 60){
            show_next = false;
            n_count = 0;
            next_direct->hide();
        }
    }

}



void MainWindow::assign_card(int plid, int color, int num, int x,int y)
{

    player_gui &pl = gui_players[plid];
    if(pl.pose_set.size()==0){
        return;
    }
    card_gui new_card;
    new_card.back = new QLabel(this);
    new_card.pos = pl.pose_set.back();
    new_card.pos.x = x;
    new_card.pos.y = y;
    pl.pose_set.pop_back();
    new_card.color = color;
    new_card.number = num;
    QPixmap pix(":/img/images/green_back.png");
    new_card.back->setPixmap(pix);
    new_card.back->show();
    new_card.back->setGeometry(new_card.pos.x,new_card.pos.y,pix.width(),pix.height());
    new_card.cover = true;
    new_card.image = image_label[color*13+num-1];
    new_card.image->setGeometry(new_card.pos.x,new_card.pos.y,pix.width(),pix.height());
    new_card.image->hide();
    pl.card_set.push_back(new_card);

}


void MainWindow::assign_card(int plid, int color, int num)
{

    player_gui &pl = gui_players[plid];
    if(pl.pose_set.size()==0){
        return;
    }
    card_gui new_card;
    new_card.back = new QLabel(this);
    new_card.pos = pl.pose_set.back();
    new_card.pos.x = 0;
    new_card.pos.y = 0;
    pl.pose_set.pop_back();
    new_card.color = color;
    new_card.number = num;
    QPixmap pix(":/img/images/green_back.png");
    new_card.back->setPixmap(pix);
    new_card.back->show();
    new_card.back->setGeometry(new_card.pos.x,new_card.pos.y,pix.width(),pix.height());
    new_card.cover = true;
    new_card.image = image_label[color*13+num-1];
    new_card.image->setGeometry(new_card.pos.x,new_card.pos.y,pix.width(),pix.height());
    new_card.image->hide();
    pl.card_set.push_back(new_card);

}

void MainWindow::show_card(int plid, int n)
{
    player_gui pl =gui_players[plid];
    pl.card_set[n].back->hide();
    pl.card_set[n].image->show();
    pl.card_set[n].image->raise();
    pl.card_set[n].cover = false;
}

void MainWindow::cover_card(int plid, int n)
{
    player_gui &pl =gui_players[plid];
    pl.card_set[n].image->hide();
    pl.card_set[n].back->show();
//    for(unsigned long i=pl.card_set.size()-1;i>=0;i--){
//        pl.card_set[i].back->raise();
//    }

    pl.card_set[n].cover = true;
}

void MainWindow::remove_card(int plid, int n)
{
    player_gui &pl =gui_players[plid];
    qDebug() << "removing " << pl.card_set[n].number ;
    pl.card_set[n].back->hide();
    pl.card_set[n].image->hide();
    pl.pose_set.push_back(pl.card_set[n].pos);
    delete pl.card_set[n].back;
    pl.card_set.erase(pl.card_set.begin()+n);
    qDebug() << "removed" ;

}


void MainWindow::change_card(int pl1, int cn1, int pl2, int cn2)
{
    card_gui &c1 = gui_players[pl1].card_set[cn1];
    card_gui &c2 = gui_players[pl2].card_set[cn2];
    QLabel *tmp = c1.image;
    c1.image = c2.image;
    c2.image = tmp;
    c1.image->setGeometry(c1.pos.x,c1.pos.y,card_width,card_hight);
    c2.image->setGeometry(c2.pos.x,c2.pos.y,card_width,card_hight);

    int t_num,t_suit;
    t_num = c1.number;
    c1.number = c2.number;
    c2.number = t_num;
    t_suit = c1.color;
    c1.color = c2.color;
    c2.color = t_suit;

}

void MainWindow::change_set(int plid1, int plid2)
{
    vector<card_gui> tmp_set;
    tmp_set = gui_players[plid1].card_set;
    gui_players[plid1].card_set = gui_players[plid2].card_set;
    gui_players[plid2].card_set = tmp_set;

    construct_card_set(plid1);
    construct_card_set(plid2);
}

void MainWindow::show_circle(bool clockwise)
{
    cw = clockwise;
    ccw = !cw;
    qDebug() << "show circle";
}

void MainWindow::close_circle()
{
    cw = false;
    ccw = false;
    cc_count = 0;
    direction_circle->hide();
    qDebug() << "close circle";
}

void MainWindow::next(int plid)
{
    next_plid = plid;
    show_next = true;
}

void MainWindow::end_game()
{
    QString name = QString().fromStdString(gui_players[turn_count].name) + QString("   win");
    end_label->setText(name);
    end_label->show();

    qDebug() << "end game" ;
}

void MainWindow::construct_card_set(int plid)
{
    player_gui &pl1 = gui_players[plid];
    int initx = pl1.init_pos.x;
    int inity = pl1.init_pos.y;
    pl1.pose_set.clear();

    for(int i=0;i<5;i++){
        card_pos cpos;
        cpos.x = initx - i*100;
        cpos.y = inity;
        cpos.ox = cpos.x;
        cpos.oy = cpos.y;
        cpos.tx = cpos.x;
        cpos.ty = cpos.y;
        pl1.pose_set.push_back(cpos);
    }

    for(unsigned long i=0;i<pl1.card_set.size();i++){
        card_pos tmp = pl1.card_set[i].pos;
        pl1.card_set[i].pos = pl1.pose_set.back();
        pl1.card_set[i].pos.x = tmp.x;
        pl1.card_set[i].pos.y = tmp.y;
        pl1.pose_set.pop_back();
    }

}


