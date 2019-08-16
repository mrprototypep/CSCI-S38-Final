#include <ctime>
#include "Player.h"
#include "Cards.h"

using namespace std;

//PERRY MACMURRAY
int main()
{
	//Title and stuff starts here
	cout << "Welcome to HEARTHSTONE II" << endl;
	cout << "Please note that while I tried my hardest this game is incredibly boring" << endl;
	cout << "I'll leave the game design to Blizzard, who I stole the name Hearthstone and the design of this game from" << endl << endl;
	//End self deprecation

	//Preinitializtion starts here
	srand(time(0));

	//Initialization starts here - create deck
	Deck* deck = new Deck;
	CardReader cr("cards.txt", deck);
	while (cr.HasCard()) //Gets all cards from file
		cr.NextCard();
	cr.Finish();

	//Postinitialization starts here - populate player hands with cards from deck
	Player* player1 = new UserPlayer();
	for (int i = 0; i < _HANDSIZE; i++)
		player1->hand[i] = deck->GetCard(); //Populates inventory
	Player* player2 = new AIPlayer();
	for (int i = 0; i < _HANDSIZE; i++)
		player2->hand[i] = deck->GetCard(); //Populates inventory

	//Main game/loop begins here
	Player* turn = player1;
	Player* enemy = player2;
	while (turn->DoTurn(enemy, deck)) { //Continues to loop between players until someone dies
		Player* temp = turn;
		turn = enemy;
		enemy = temp;
		temp = nullptr;
	}

	cout << enemy->GetName() << " has won!" << endl;

	//Deallocation begins here
	delete player1;
	delete player2;
	delete deck;
}