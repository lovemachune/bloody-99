#include "cards.h"

char suits[4] = {'0', '1', '2', '3'};

void swap(card *a, card *b)
{
    card temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

pocker_card::pocker_card()
{
    point = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j <= 13; j++)
        {
            card temp;
            temp.suit = suits[i];
            temp.point = j;
            deck.push_back(temp);
        }
    }
}
void pocker_card::shuffel()
{
    for (int i = 0; i < deck.size(); i++)
    {
        int r = i + (rand() % (52 - i));
        swap(&deck[i], &deck[r]);
    }
}
void pocker_card::show()
{
    for (int i = 0; i < deck.size(); i++)
    {
        printf("%c,%d\n", deck[i].suit, deck[i].point);
    }
}
void pocker_card::deal(player *temp)
{
    (*temp).setCards(deck.back());
    deck.pop_back();
}
void pocker_card::set_point(int num)
{
    if(num == 99)
        point = 99;
    else
        point += num;
    if(point<=0)
        point = 0;
}
int pocker_card::get_point()
{
    return point;
}
int pocker_card::change_one_card(player *source, player *target)
{
    int temp = rand()%(target->size_of_hand());
    source->setCards((target->get_hand(temp)));
    target->delCard(temp);
    return temp;
}
void pocker_card::change_all_card(player *source, player *target)
{
    vector<card> temp;
    temp = target->get_total_hand();
    target->setHand(source->get_total_hand());
    source->setHand(temp);
}



void player::setCards(card temp)
{
    hand.push_back(temp);
}
void player::show()
{
    for (int i = 0; i < hand.size(); i++)
    {
        printf("%c,%d\n", hand[i].suit, hand[i].point);
    }
};
card player::get_hand(int x)
{
    return hand[x];
}
vector<card> player::get_total_hand()
{
    return hand;
}
bool player::gameover()
{
    if(hand.size()>0)
        return false;
    return true;
}
void player::setHand(vector<card> hand)
{
    this->hand = hand;
}
int player::size_of_hand()
{
    return hand.size();
}
void player::delCard(int x)
{
    hand.erase(hand.begin()+x);
}
string player::getString()
{
    string s;
    for(int i=0 ; i<hand.size() ; i++)
    {
        if(i==hand.size()-1)
            s = s+to_string(hand[i].point);
        else
            s = s+to_string(hand[i].point)+" ";
    }
    return s;
}
