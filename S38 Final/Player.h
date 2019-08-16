#pragma once
#include <vector>
#include <iostream>

using namespace std;

const unsigned int _HANDSIZE = 6; // The number of cards the player should have in their hand

//Prototypes
struct Card;
struct CardHolder;
enum DamageType;
struct Minion;
struct Ability;
class Deck;
//End prototypes

//The base class extended by AIPlayer and UserPlayer. This class is abstract.
class Player {
public:
	bool DoTurn(Player* enemy, Deck* deck);
	void TakeDamage(unsigned int damage, DamageType dType);
	int GetNumMinionsAlive();

	virtual Card* Selection(Deck* deck, Player* enemy) = 0;

	CardHolder* hand[_HANDSIZE] = {};

	vector<Minion*> minions;

	int GetHealth();
	string GetName();
protected:
	string name = "";
	int health = 1000;
private:
	friend struct Card;
	friend struct Ability;

	int dotDamage = 0;
	int dotTurnsRemaining = 0;
};

//The AIPlayer, used (intuitively) by the AI (or, "AI")
class AIPlayer : public Player {
public:
	Card* Selection(Deck* deck, Player* enemy);

	AIPlayer();
};

//The UserPlayer, used (intuitively) by the user.
class UserPlayer : public Player {
public:
	Card* Selection(Deck* deck, Player* enemy);

	UserPlayer();
};