//This file is responsible for the Card struct and all of its dependency structs (Minion, Ability, etc.)

#include "Cards.h"
#include "Player.h"

//Destructor for CardHolder - deletes the card it's holding
CardHolder::~CardHolder() {
	delete card;
	card = nullptr;
}

//Casts (plays) the card - what this does depends on it being either a spell or minion card
void Card::Cast(Player* user, Player* enemy) {
	if (type == SpellCard) { //Spell processing starts here
		switch (spell->target) {
		case player_enemy: { //Attacks enemy
			enemy->TakeDamage(spell->damageAmount, spell->dType);
			break;
		}
		case player_user: { //Attacks user (dType here is probably healing)
			user->TakeDamage(spell->damageAmount, spell->dType);
			break;
		}
		case minion_enemy: { //Attaskc minion from enemy with the MOST health
			if (!enemy->GetNumMinionsAlive()) //Checks to make sure there are alive minions
				break;
			Minion* minion = enemy->minions[0];
			for (Minion* m : enemy->minions) {
				if (m->health > minion->health)
					minion = m;
			}
			minion->TakeDamage(spell->damageAmount, spell->dType);
			break;
		}
		case minion_enemy_all: { //Attacks all minions from the enemy
			for (Minion* m : enemy->minions)
				m->TakeDamage(spell->damageAmount, spell->dType);
			break;
		}
		case minion_user: { //Attacks minion from user with LEAST health
			if (!user->GetNumMinionsAlive()) //Checks to make sure there are alive minions
				break;
			Minion* minion = enemy->minions[0];
			for (Minion* m : enemy->minions) {
				if (m->health < minion->health)
					minion = m;
			}
			minion->TakeDamage(spell->damageAmount, spell->dType);
			break;
		}
		case minion_user_all: { //Attaks all minions from the user
			for (Minion* m : user->minions)
				m->TakeDamage(spell->damageAmount, spell->dType);
			break;
		}
		}
	}
	else if (type == MinionCard) { //Minion processing starts here
		user->minions.push_back(minion); //Adds minion to users' minions
	}
	else //Something is wrong here
		throw new exception("Something went wrong... Card::Cast() did not account for Card type. Assuming no card types were added since writing, this should *never ever* be thrown.");
}

//Takes (or rather, processes) damage passed in the form of a number and a DamageType
void Minion::TakeDamage(unsigned int damage, DamageType dType) {
	switch (dType) {
	case DOT: //Please note that DOT stacking in the way it does is NOT a glitch and was intentionally added to make DOT the most powerful damage if done correctly.
		dotDamage += damage;
		dotTurnsRemaining = 3;
		break;
	case DamageType::damage: //Damage subtracts from shield first, and then health.
		if (shield >= damage) {
			shield -= damage;
		}
		else {
			damage -= shield;
			shield = 0;
			if (health >= damage)
				health -= damage;
			else
				health = 0;
		}
		break;
	case DamageType::shield: //Shield adds to the shield variable and in effect increases health
		shield += damage;
		break;
	case healing: //Increases health (does not increase health past maxhealth)
		health += damage;
		if (health > maxHealth)
			health = maxHealth;
		break;
	default:
		throw new exception("Something went wrong... wrong DamageType passed to Minion::TakeDamage()");
		break;
	}
}

//Causes minion to attack with standard attack and any triggered abilities.
void Minion::Do(long id, Player* user, Player* enemy) {
	if (!alive)
		return;
	if (health <= 0) { //Handles dying
		alive = false;
		return;
	}

	int dmgAmount = level * (rand() % dmgRoll);

	enemy->TakeDamage(dmgAmount, damage); //Enemy takes damage

	experience += dmgAmount; //Experience increases by howevermuch damage is dealt
	level += experience / 100; //Causes experience to not pass 100, increasing level instead
	experience %= 100;

	for (Ability* a : abilities) { //Iterates through abilities that a minion has
		if (!a->isConditional || a->conditional == id)
			a->Do(user, enemy);
	}

	if (dotTurnsRemaining > 0) { //Takes DOT damage
		TakeDamage(dotDamage, damage);
		dotTurnsRemaining--;
	}
}

//Executes an ability, based on type of damage and target.
void Ability::Do(Player* user, Player* enemy) {
	switch (target) {
	case player_user: { //Attack user
		user->TakeDamage(damageAmount, dType);
		break;
	}
	case player_enemy: { //Attack enemy
		enemy->TakeDamage(damageAmount, dType);
		break;
	}
	case minion_user: { //Attack user's minion with LEAST health
		if (!user->GetNumMinionsAlive())
			return;

		Minion* t = user->minions[0];
		for (Minion* m : user->minions) {
			if (m->health < t->health) //Targets minion with least health
				t = m;
		}

		t->TakeDamage(damageAmount, dType);
		break;
	}
	case minion_enemy: { //Attack enemy's minion with MOST health
		if (!enemy->GetNumMinionsAlive())
			return;

		Minion* t = enemy->minions[0];
		for (Minion* m : enemy->minions) {
			if (m->health > t->health) //Targets minion with most health
				t = m;
		}

		t->TakeDamage(damageAmount, dType);
		break;
	}
	case minion_user_all: { //Attacks all user's minions
		for (Minion* m : user->minions)
			m->TakeDamage(damageAmount, dType);
		break;
	}
	case minion_enemy_all: { //Attacks all enemy's minions
		for (Minion* m : enemy->minions)
			m->TakeDamage(damageAmount, dType);
		break;
	} //Slightly hideous bracket
	}
}

//Card destructor - deletes spell and/or minion, depending on what's there
Card::~Card() {
	if (spell != nullptr) {
		delete spell;
		spell = nullptr;
	}
	if (minion != nullptr) {
		delete minion;
		minion = nullptr;
	}
}

//Minion destructor - deletes all of the abilities the minion has
Minion::~Minion() {
	for (Ability* a : abilities) {
		if (a != nullptr) {
			delete a;
			a = nullptr;
		}
	}
}