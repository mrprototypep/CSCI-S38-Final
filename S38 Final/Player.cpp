#include "Player.h"
#include "Cards.h"

using namespace std;

//Returns false if the player is dead. True otherwise. Does turn
bool Player::DoTurn(Player* enemy, Deck* deck) {
	if (health <= 0) {
		cout << name << " has died." << endl;
		return false;
	}

	Card* selection = Selection(deck, enemy); //Gets the card to play

	for (Minion* minion : minions)
		minion->Do(selection->id, this, enemy); //Executes all minion turns

	selection->Cast(this, enemy); //Casts card

	if (dotTurnsRemaining > 0) { //Takes DOT damage
		TakeDamage(dotDamage, damage);
		dotTurnsRemaining--;
	}

	return true;
}

//Takes damage of type dType and amount damage. Note that players do not have shield
void Player::TakeDamage(unsigned int damage, DamageType dType) {
	switch (dType) {
	case DOT: //Please note that DOT stacking in the way it does is NOT a glitch and was intentionally added to make DOT the most powerful damage if done correctly.
		dotDamage += damage;
		dotTurnsRemaining = 3;
		break;
	case DamageType::damage: //Subtracts damage from health
		health -= damage;
		break;
	case healing: //Adds damage to health. Doesn't allow health surpass 1000
		health += damage;
		if (health > 1000) //Max health is 1000 for players
			health = 1000;
		break;
	default:
		throw new exception("Something went wrong... wrong DamageType passed to Player::TakeDamage() (shield?)");
		break;
	}
}

//Has the AI select which card from its hand to play. This uses a fairly simplistic and quite frankly flawed method, but by design.
//The AI shouldn't play perfectly, because it makes it too hard to win, especially in a quite frankly luck-based game.
Card* AIPlayer::Selection(Deck* deck, Player* enemy) {
	int index = 0;
	unsigned int maxDamage = 0;
	for (int i = 0; i < _HANDSIZE; i++) { //Iterates through hand and calculates an estimate of how much damage a card will cause
		unsigned int damage = 0;
		if (hand[i]->card->type == MinionCard) {
			damage = hand[i]->card->minion->dmgRoll * hand[i]->card->minion->level;
		}
		else { //Spell card
			Card* c = hand[i]->card;
			switch (c->spell->dType) {
			case DOT:
				damage = c->spell->damageAmount * 3; //DOT happens over 3 turns
				break;
			case healing:
			case shield:
			case DamageType::damage:
				if (c->spell->target == minion_enemy_all)
					damage = c->spell->damageAmount * enemy->GetNumMinionsAlive();
				else if (c->spell->target == minion_user_all)
					damage = c->spell->damageAmount * this->GetNumMinionsAlive();
				else
					damage = c->spell->damageAmount;
				break;
			}
		}

		if (damage > maxDamage) {
			maxDamage = damage;
			index = i;
		}
	}

	if (hand[index]->card->type == SpellCard)
		hand[index]->inHand = false; //Minion cards do not get shuffled back into the deck and, as such, have inHand kept true even when used

	Card* selection = hand[index]->card; //Selects card that causes the most damage
	
	hand[index] = deck->GetCard(); //Replaces selection in deck

	cout << "AI Plays " << selection->name << ':' << endl;
	cout << selection->description << endl << endl;

	return selection;
}

//Asks user to select a card, based off of the information displayed about the enemy and them.
Card* UserPlayer::Selection(Deck* deck, Player* enemy) {
	cout << '['; //Outputs user health fancily
	for (int i = 0; i < 40; i++) {
		if (((double)health / 1000) > i / 40.0)
			cout << "#";
		else
			cout << "-";
	}
	cout << ']';
	cout << " YOUR HEALTH" << endl;

	cout << '['; //Outputs enemy health fancily
	for (int i = 0; i < 40; i++) {
		if (((double)enemy->GetHealth() / 1000) > i / 40.0)
			cout << "#";
		else
			cout << "-";
	}
	cout << ']';
	cout << " ENEMY HEALTH" << endl;

	//Outputs approximate enemy damage from minions
	cout << "Enemy has " << enemy->GetNumMinionsAlive() << " minions, resulting in approximately ";
	int enemyDamage = 0;
	for (Minion* m : enemy->minions) {
		if (m->alive)
			enemyDamage += m->level * m->dmgRoll / 2;
	}
	cout << enemyDamage << " damage next turn." << endl << endl;

	//Outputs minion information (of alive minions)
	if (GetNumMinionsAlive()) {
		cout << "=== Minions:" << endl;
		for (Minion* m : minions) {
			if (!m->alive)
				continue;

			cout << '[';
			for (int i = 0; i < 20; i++) {
				if (((double)m->health / m->maxHealth) > i / 20.0)
					cout << "#";
				else
					cout << "-";
			}
			cout << ']';
			cout << ' ' << m->health << '/' << m->maxHealth << " - " << m->name << " - Max Damage: " << m->level << 'x' << m->dmgRoll << endl;
		}
		cout << endl;
	}

	//Outputs hand information
	cout << "=== Hand:" << endl;
	for (int i = 0; i < _HANDSIZE; i++) {
		Card* c = hand[i]->card;

		cout << i << ": " << ((c->type == MinionCard)?"[MINION]":"[SPELL]") << ' ' << c->name << " [Level " << c->level << ']' << endl;
	}

	//Gets choice. Allows user to say "?" to see descriptions of the cards.
	int choice = -1;
	while (choice < 0 || choice > _HANDSIZE) {
		cout << "Selection: >";
		bool info = false;
		try { //Try/Catch safely gets input.
			string input;
			getline(cin, input);
			if (!input.compare("?"))
				info = true;
			choice = stoi(input);
		}
		catch (invalid_argument) {
			choice = -1;
		}
		catch (out_of_range) {
			choice = -1;
		}

		if (info) {
			for (int i = 0; i < _HANDSIZE; i++) {
				cout << i << ": " << hand[i]->card->name << endl;
				cout << hand[i]->card->description << endl << endl;
			}
		}

		if ((choice < 0 || choice > _HANDSIZE) && !info)
			cout << "Please enter a valid input." << endl;
	}

	cout << endl;

	Card* selection = hand[choice]->card;
	
	if (hand[choice]->card->type == SpellCard) //Minion cards stay "in hand" so they don't get shuffled back into the deck
		hand[choice]->inHand = false;

	hand[choice] = deck->GetCard();

	return selection;
}

//Sets name of AI player ("AI Player")
AIPlayer::AIPlayer() {
	name = "AI Player";
}

//Gets and sets name of player
UserPlayer::UserPlayer() {
	cout << "Please enter your name: ";
	string name;
	getline(cin, name);

	this->name = name;
}

//Returns number of minions alive
int Player::GetNumMinionsAlive() {
	int alive = 0;
	for (Minion* m : minions) {
		if (m->alive)
			alive++;
	}

	return alive;
}

//Gets health
int Player::GetHealth() {
	return health;
}

//Gets name
string Player::GetName() {
	return name;
}