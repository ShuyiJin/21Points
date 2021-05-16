#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

class Card
{
	public:
		enum rank {ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, \
				JACK, QUEEN, KING};
		enum suit {HEART, SPADE, DIAMOND, CLUB};	
		friend ostream &operator<<(ostream &os, const Card &aCard);
		Card(rank r = ACE, suit s = SPADE, bool ifup = true);
		int getValue() const;
		void flip();
	private:
		rank m_rank;
		suit m_suit;
		bool m_isfaceup;
};

class Hand
{
	protected:
		vector<Card*> m_cards;

	public:
		Hand();
		~Hand();
		void add(Card *pCard); // add a card
		void clear(); // clear all cards
		int getTotal() const; // get total value
};

class GenericPlayer : public Hand
{
		friend ostream &operator<<(ostream &os, const GenericPlayer &aGenericPlr);
	protected:
		string m_name;
	public:
		GenericPlayer(const string &name = "");
		virtual bool isHitting() const = 0;
		bool isBusted() const;
		void bust() const;		
};

class Deck : public Hand
{
	public:
		Deck();
		void populate();
		void shuffle();
		void deal(Hand &h);
		void additionalCards(GenericPlayer &genPlayer);
};

class Player : public GenericPlayer
{
	public:
		Player(const string &name = "");
		virtual bool isHitting() const;
		void win() const;
		void lose() const;
		void tie() const;
};

class Declarer : public GenericPlayer
{
	public:
		Declarer(const string &name = "Declarer");
		virtual bool isHitting() const;
		void flipFirstCard();	
};

class Game
{
	private:
		Declarer m_declarer;
		vector<Player> m_players;
		Deck m_deck;
	public:
		Game(const vector<string> &names);
		void play();
};

ostream &operator<<(ostream &os, const Card &aCard);
ostream &operator<<(ostream &os, const GenericPlayer &aGenericPlr);

#endif /* game.h */
