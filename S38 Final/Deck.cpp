//This file deals with the deck object used to randomly get cards

#include "Cards.h"

//Gets a random card from the deck. Shuffles discard pile back into the deck automatically if needed.
CardHolder* Deck::GetCard() {
	if (deck.size() == 0) //Returns nullptr if no cards in deck
		return nullptr;
	if (deck.size() == discarded) //If no non-discarded cards are in the deck, shuffle.
		Shuffle();

	vector<int> indices; //Gets the indecies of all cards that are actually in the deck (no movement between "piles" exists, so cards that are discarded just are marked as discarded)
	for (int i = 0; i < deck.size(); i++) {
		if (!deck[i]->discard && !deck[i]->inHand)
			indices.push_back(i);
	}
	
	CardHolder* selection = deck[indices[rand() % indices.size()]]; //Randomly picks a non-discarded card, marks it as discarded, and returns it
	discarded++;
	selection->inHand = true;
	selection->discard = true;

	return selection;
}

//Shuffles all discarded cards back into the deck. Note that because minion cards are never actually removed from the hand, they are not shuffled back in. This is by design
void Deck::Shuffle() {
	for (auto c : deck) { //Iterates through the deck and gets rid of all discarded cards (that aren't in hand)
		if (!c->inHand) {
			c->discard = false;
			discarded--;
		}
	}
	//Note that actually shuffling the cards is not needed because GetCard() randomly draws a card anyway.
}

//Adds a card to the deck.
void Deck::Add(Card* card) {
	deck.push_back(new CardHolder{ card });
}

//Deletes all CardHolders in the deck
Deck::~Deck() {
	for (CardHolder* c : deck) {
		delete c;
		c = nullptr;
	}
}