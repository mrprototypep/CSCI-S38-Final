//This file is responsible for the CardReader class.

#include "Cards.h"

//Constructs a CardReader by opening the internal stream and assigning a deck
CardReader::CardReader(const string filename, Deck* deck) {
	stream.open(filename);
	this->deck = deck;
}

//Closes the internal stream
void CardReader::Finish() {
	stream.close();
}

//Returns true if there's more to read, false if not
bool CardReader::HasCard() {
	return !stream.eof();
}

//Builds the next card contained in the file, and adds it to the deck.
//Admittedly this function is kind of messy, but that's mostly at fault of how I had to read from the file.
//If I knew how (or was doing this in a language I'm more familiar with, like C#) I'd try to read cards from a JSON file instead.
void CardReader::NextCard() {
	if (stream.peek() != '@') //If the first character of the new card isn't @, something is wrong with the file.
		throw new invalid_argument("Expected new card symbol and did not find one - check input file!");

	Card* card = new Card(); //Creates new card
	stream.ignore(); //Ignore @
	string text;
	getline(stream, text); //Gets ID
	card->id = stoi(text); //Sets ID

	getline(stream, text); //Gets name
	card->name = text; //Sets name

	bool first = true;
	while (stream.peek() != '#') { //Continues adding to the description until the first character of a line is # - this denotes the card designation
		getline(stream, text); //Gets description
		if (first)
			first = false;
		else
			card->description += '\n';
		card->description += text; //Sets description
	}

	stream.ignore(); //Ignores #
	getline(stream, text); //Gets type (as string)

	if (!text.compare("S")) //Sets type
		card->type = SpellCard;
	else if (!text.compare("M"))
		card->type = MinionCard;
	else
		throw new exception("Something went wrong... invalid type in file, probably");

	getline(stream, text); //Gets level
	card->level = stoi(text); //Sets level

	if (card->type == SpellCard) {
		card->spell = new Ability; //Creates the ability of the spell card

		getline(stream, text); //Gets DamageType
		int i = 0;
		while (i < DamageTypeNum) { //Iterates through DamageTypeStrings until one matches, then assigns its respective DamageType
			if (!text.compare(DamageTypeStrings[i]))
				break;
			else
				i++;
		}
		if (i >= DamageTypeNum)
			throw new out_of_range("Could not find comperable DamageType - check input file!");
		card->spell->dType = (DamageType)i; //Sets DamageType

		getline(stream, text); //Gets damage
		card->spell->damageAmount = stoi(text); //Sets damage

		getline(stream, text); //Gets TargetType
		i = 0; //i is reused here
		while (i < TargetTypeNum) { //Does the same thing as what happened with DamageTypeStrings
			if (!text.compare(TargetTypeStrings[i]))
				break;
			else
				i++;
		}
		if (i >= TargetTypeNum)
			throw new out_of_range("Could not find comperable TargetType - check input file!");
		card->spell->target = (TargetType)i; //Sets TargetType
	}
	else if (card->type == MinionCard) { //Minion processing starts here
		card->minion = new Minion; //Creates new minion object

		card->minion->name = card->name; //Assigns name and level from card, as they're the same
		card->minion->level = card->level;

		getline(stream, text); //Gets health (and max health)
		card->minion->health = stoi(text); //Sets health
		card->minion->maxHealth = stoi(text); //Sets max health

		getline(stream, text); //Gets max damage (random with this as max, then multiplied by level)
		card->minion->dmgRoll = stoi(text) + 1; //Sets max damage - 1 added here because for max value of rand() % n to be x, n has to be x + 1

		while (stream.peek() == '?') { //Continues to try and get abilities until it runs out in the file
			Ability* ability = new Ability; //Creates new ability
			card->minion->abilities.push_back(ability); //Adds ability to card's minion
			ability->isConditional = true; //Sets ability as conditional (on the ID)

			stream.ignore(); //Ignores ?
			getline(stream, text); //Gets Conditional ID (cID)
			ability->conditional = stoi(text); //Sets cID

			getline(stream, text); //Gets DamageType
			int i = 0;
			while (i < DamageTypeNum) { //Iterates
				if (!text.compare(DamageTypeStrings[i]))
					break;
				else
					i++;
			}
			if (i >= DamageTypeNum)
				throw new out_of_range("Could not find comperable DamageType - check input file!");
			ability->dType = (DamageType)i; //Sets DamageType

			getline(stream, text); //Gets damage
			ability->damageAmount = stoi(text); //Sets damage

			getline(stream, text); //Gets target
			i = 0; //i is reused here
			while (i < TargetTypeNum) { //Iterates
				if (!text.compare(TargetTypeStrings[i]))
					break;
				else
					i++;
			}
			if (i >= TargetTypeNum)
				throw new out_of_range("Could not find comperable TargetType - check input file!");
			ability->target = (TargetType)i; //Sets TargetType
		}
	}

	while (!stream.eof() && stream.peek() != '@')
		getline(stream, text); //This in effect ignores the line; I couldn't find a way to use stream.ignore() here easily.

	deck->Add(card); //Adds newly constructed card to deck
}