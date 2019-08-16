#pragma once
#include <vector>
#include <fstream>
#include <string>

using namespace std;

//Prototypes
class Player;
class Deck; // Used by CardReader
//End prototypes

//Used to define types of cards
enum Class {
	SpellCard, MinionCard, NullType
};

const int DamageTypeNum = 5;

//Everything here is self explanatory besides for DOT, which is damage over time. "Time" is the amount of levels the minion has
enum DamageType {
	damage, healing, DOT, shield
};

const string DamageTypeStrings[] = { "damage", "healing", "DOT", "shield" };

const int TargetTypeNum = 6;

//Used to define types of targets - which does what is fairly self explanatory
enum TargetType {
	player_enemy, player_user, minion_enemy, minion_enemy_all, minion_user, minion_user_all
};

const string TargetTypeStrings[] = { "player_enemy", "player_user", "minion_enemy", "minion_enemy_all", "minion_user", "minion_user_all" };

//Container for an ability (spell or minion ability) - deals damage of type to target
struct Ability {
	TargetType target;
	DamageType dType;
	int damageAmount;
	bool isConditional = false;
	long conditional;

	void Do(Player* user, Player* enemy);
};

//Container for a Minion - mainly contains a vector of abilities.
struct Minion {
	unsigned int maxHealth;
	unsigned int health;
	bool alive = true;
	unsigned int shield = 0;
	unsigned int experience = 0; //When experience reaches 100, level increases by 1
	unsigned int level = 1;
	unsigned int dotTurnsRemaining = 0; //Damage over time
	unsigned int dotDamage = 0; //Damage over time

	string name = "";

	unsigned int dmgRoll;
	vector<Ability*> abilities;

	void Do(long id, Player* user, Player* enemy);
	void TakeDamage(unsigned int damage, DamageType dType);
	~Minion();
};

//Container for a card - mainly contains either a single ability (spell) or a minion
struct Card {
	long id = 0;
	string name = "";
	string description = "";
	Class type = NullType;
	unsigned int level = 0;
	~Card();

	void Cast(Player* user, Player* enemy);

	//Spells
	Ability* spell = nullptr;

	//Minions
	Minion* minion = nullptr;
};

//Defines CardReader, which is used to populate a deck
class CardReader {
public:
	CardReader(const string filename, Deck* deck);
	bool HasCard();
	void NextCard();
	void Finish();
private:
	ifstream stream;
	Deck* deck;
};

//Holds a card and its status. Prevents having to move objects around in memory a bunch, which is nice.
struct CardHolder {
	Card* card;
	bool discard = false;
	bool inHand = false;

	~CardHolder();
};

//Defines Deck, which holds all the CardHolders.
class Deck {
public:
	CardHolder* GetCard();
	~Deck();
private:
	friend class CardReader;

	void Add(Card* card);
	int discarded = 0;
	vector<CardHolder*> deck;
	void Shuffle();
};