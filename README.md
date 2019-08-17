# CSCI-S38-Final
My final for Introduction to C++ at Harvard

Hearthstone II is a not-very-originally named ripoff of Hearthstone, the popular online card game by Blizzard. In the game, both players (The AI and the user) get six cards, all of which are either of type SPELL or MINION.

EFFECT TYPES
There are four effect types in the game:
Damage - Causes damage to the target
Healing - Heals the target
Shield - Adds a shield to the target. This is not reflected in health, but negates damage.
DOT - DOT (or Damage Over Time) deals damage for 3 turns. For example, 75 DOT damage would deal 75 * 3 damage. Note, however, that DOT damage stacks. For example, if two DOT cards were played back to back, both dealing 75 damage, when the second card is played the target would take 150 damage FOR THE FULL THREE TURNS. In other words, playing more DOT cards continues the DOT damage from other cards.

SPELL CARDS
Spell cards have an effect and a target. The target(s) can be one or all minions of either you or your enemy, or you or your enemy. Spell cards can also trigger special abilities that minions have, which will be discussed later.

MINION CARDS
Minion cards summon minions. These minions have a constant damage output of their level times (rand() % N), where N is a constant based on the minion. This is commonly expressed as LxN.
Minions also have abilities, triggered by casting a specific card. For example, the Knight minion has an ability that is trigerred when "Distress Call" is played, which deals DOT damage to the enemy.

SELECTING YOUR CARD
The main loop of this game has you and the AI choosing cards. The AI's card choices are displayed. With your card, some information is shown to you before you choose:
Your health and your enemy's health
The health of all your minions
The names and levels of all the cards in your hand
For your selection, you can either choose the card's number 0-5 or type "?", which shows a description for every card.

When someone wins, the winner is displayed and the game ends.
