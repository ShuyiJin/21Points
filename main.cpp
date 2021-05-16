#include "game.h"

/**********************************
 *		    card.cpp              *
 **********************************/

Card::Card(rank r, suit s, bool ifup):m_rank(r), m_suit(s), m_isfaceup(ifup){}

int Card::getValue() const
{
	int value = 0;
	if(m_isfaceup)
	{
		value = m_rank;

		if(value > 10)
			value = 10;
	}
	return value;
}

void Card::flip()
{
	m_isfaceup = !(m_isfaceup);
}

/**********************************
 *		    hand.cpp              *
 **********************************/

Hand::Hand()
{
	m_cards.reserve(7); // max number of cards allowed
}

Hand::~Hand()
{
	clear();
}

void Hand::add(Card *pCard)
{
	m_cards.push_back(pCard);
}

void Hand::clear()
{
	vector<Card*>::iterator it = m_cards.begin();
	for(; it != m_cards.end(); ++it)
	{	
		delete *it;
		*it = NULL;
	}
	m_cards.clear();
}

int Hand::getTotal() const
{
	if(m_cards.empty())
		return 0;
	if(m_cards[0]->getValue() == 0)
		return 0;

	int total = 0;
	vector<Card *>::const_iterator iter;
	for(iter =  m_cards.begin(); iter != m_cards.end(); ++iter)
	{
		total += (*iter)->getValue(); 
	}

	bool hasAce = false;
	for(iter = m_cards.begin(); iter != m_cards.end(); ++iter)
	{
		if((*iter)->getValue() == Card::ACE)
		{
			hasAce = true;
			break;
		}
	}

	if(hasAce && total <= 11)
		total += 10;

	return total;
}

/**********************************
 *		    deck.cpp              *
 **********************************/

Deck::Deck()
{
	m_cards.reserve(52);
}

void Deck::populate()
{
	clear();
	for(int s = Card::HEART; s <= Card::CLUB; s++) // suit
	{
		for(int r = Card::ACE; r <= Card::KING; r++) // rank
		{
			add(new Card(static_cast<Card::rank>(r), static_cast<Card::suit>(s))); // construct a card
		}
	}
}

void Deck::shuffle()
{
	random_shuffle(m_cards.begin(), m_cards.end()); // c++ algorithm
}

void Deck::deal(Hand &h)
{
	if(m_cards.empty())
	{
		// no card left on deck
		cout << "Deck is empty" << endl;
	}
	else
	{
		h.add(m_cards.back());
		m_cards.pop_back();
	}
}

void Deck::additionalCards(GenericPlayer &genPlayer)
{
	cout << endl;
	while(!(genPlayer.isBusted()) && genPlayer.isHitting())
	{
		deal(genPlayer);
		cout << genPlayer << endl;

		if(genPlayer.isBusted())
			genPlayer.bust();
	}
}

/**********************************
 *	    genericPlayer.cpp         *
 **********************************/

GenericPlayer::GenericPlayer(const string &name):m_name(name){}

bool GenericPlayer::isBusted() const
{
	return getTotal() > 21;
}

void GenericPlayer::bust() const
{
	cout << m_name << " is busted!" << endl;
}

/**********************************
 *		  player.cpp              *
 **********************************/

Player::Player(const string &name): GenericPlayer(name){}

bool Player::isHitting() const
{
	cout << m_name << ", claim for card? (Y/N): ";
	char response;
	cin >> response;
	return response == 'y' || response == 'Y';
}

void Player::win() const
{
	cout << m_name << " won!" << endl;
}

void Player::lose() const
{
	cout << m_name << " lost..." << endl;
}

void Player::tie() const
{
	cout << m_name << " tied..." << endl;
}

/**********************************
 *		  declarer.cpp            *
 **********************************/

Declarer::Declarer(const string &name): GenericPlayer(name){}

bool Declarer::isHitting() const
{
	return getTotal() <= 17;
}

void Declarer::flipFirstCard()
{
	if(!m_cards.empty())
	{
		m_cards[0]->flip();
	}
	else
		cout << "No card to flip" << endl;
}

/**********************************
 *		     game.cpp             *
 **********************************/

Game::Game(const vector<string> &names)
{
	vector<string>::const_iterator name_it;

	for(name_it = names.begin(); name_it != names.end(); ++name_it)
	{
		m_players.push_back(Player(*name_it));
	}

	srand(time(0));
	m_deck.populate();
	m_deck.shuffle();
}

void Game::play()
{
	// draw two cards for all players
	vector<Player>::iterator pit;
	for(int i = 0; i < 2; i++)
	{
		for(pit = m_players.begin(); pit != m_players.end(); pit++)
		{
			m_deck.deal(*pit);
		}
		m_deck.deal(m_declarer);
	}
	m_declarer.flipFirstCard();

	for(pit = m_players.begin(); pit != m_players.end(); ++pit)
	{
		cout << *pit << endl;
	}
	cout << m_declarer << endl;

	for(pit = m_players.begin(); pit != m_players.end(); ++pit)
	{
		m_deck.additionalCards(*pit);
	}

	m_declarer.flipFirstCard();
	cout << endl << m_declarer;

	m_deck.additionalCards(m_declarer);

	if(m_declarer.isBusted())
	{
		cout << endl << "Players won!" << endl;
		return;
	}

	for(pit = m_players.begin(); pit != m_players.end(); ++pit)
	{
		cout << endl;
		if((*pit).isBusted())
			(*pit).lose();
		else if((*pit).getTotal() > m_declarer.getTotal())
			(*pit).win();
		else if((*pit).getTotal() < m_declarer.getTotal())
			(*pit).lose();
		else
			(*pit).tie();
	}
}

/**********************************
 *		     main.cpp             *
 **********************************/

ostream &operator<<(ostream &os, const Card &aCard) // display individual card
{
	const string RANKS[] = {"0", "A", "2", "3", "4", "5", "6", "7",\
							"8", "9", "10", "J", "Q", "K"};
	const string SUITS[] = {"♥", "♠", "♦", "♣"};

	if(aCard.m_isfaceup)
		os << SUITS[aCard.m_suit] << RANKS[aCard.m_rank];
	else
		os << "XX";

	return os;
}

ostream &operator<<(ostream &os, const GenericPlayer &aGenericPlr)
{
	os << aGenericPlr.m_name << ":\t";

	vector<Card*>::const_iterator cit;
	if(!aGenericPlr.m_cards.empty())
	{
		for(cit = aGenericPlr.m_cards.begin(); cit != aGenericPlr.m_cards.end(); ++cit)
		{
			os << *(*cit) << "\t";
		}

		if(aGenericPlr.getTotal() != 0)
		{
			os << "(" << aGenericPlr.getTotal() << ")";
		}
	}
	else
	{
		os << "<empty>";
	}

	return os;
}

void play()
{
	system("clear");
	cout << "Welcome..." << endl;
	int plrCount = 0;
	do
	{
		cout << "Please enter number of players:" << endl;
		cin >> plrCount;
	} while (plrCount < 1 || plrCount > 7);

	/* add players into container*/
	vector<string> plrNames;
	string name;
	for(int i = 0; i < plrCount; i++)
	{
		cout << "Please enter Player" << i+1 << ": ";
		cin >> name;
		plrNames.push_back(name);
	}


	char again;
	while(1)
	{
		/* init game*/
		Game myGame(plrNames);
		myGame.play();

		cout << "Play again? (Y/N) ";
		cin >> again;
		if(again == 'n' || again == 'N')
			break;
		
		sleep(1);
		system("clear");
	}

}

int main()
{
	play();
	return 0;
}
