#include "State.h"
#include "Agent.h"

//STATE
State::State(){

}

State::~State() {

}

void State::Enter(Agent* currentAgent) {

}

void State::Update(Agent* currentAgent) {

}

void State::Exit(Agent* currentAgent) {

}


//HOME
Home::Home() {

}

Home::~Home() {

}

void Home::Enter(Agent* currentAgent) {
	

}

void Home::Update(Agent* currentAgent) {
	currentAgent->rested = true; //PARA TEST
	if (currentAgent->rested == true && Vector2D((float)((int)currentAgent->getPosition().x / CELL_SIZE), (float)((int)currentAgent->getPosition().y / CELL_SIZE)) == Vector2D(8,18)) {
		currentAgent->currentState->Exit(currentAgent);
	}
}

void Home::Exit(Agent* currentAgent) {
	currentAgent->currentState = currentAgent->myMine;
	currentAgent->currentState->Enter(currentAgent);
}


//MINE
Mine::Mine() {

}

Mine::~Mine() {

}

void Mine::Enter(Agent* currentAgent) {
	currentAgent->newPathNeeded = true;
	currentAgent->setTarget(Vector2D(2, 2));
	//currentAgent->isPocketFull = false;
	currentAgent->pocketCoins = 0;
	currentAgent->thirsty = false;
	currentAgent->currentState->Update(currentAgent);
}

void Mine::Update(Agent* currentAgent) {
	//while (currentAgent->pocketCoins < 5 && !currentAgent->thirsty) {
		if (Vector2D((float)((int)currentAgent->getPosition().x / CELL_SIZE), (float)((int)currentAgent->getPosition().y / CELL_SIZE)) == currentAgent->getTarget()) {
			currentAgent->newPathNeeded = true;
			currentAgent->setTarget(Vector2D(12, 2));
			currentAgent->pocketCoins += 1;
		}
}

void Mine::Exit(Agent* currentAgent) {

}


//BANK
Bank::Bank() {

}

Bank::~Bank() {

}

void Bank::Enter(Agent* currentAgent) {

}

void Bank::Update(Agent* currentAgent) {

}

void Bank::Exit(Agent* currentAgent) {

}


//SALOON
Saloon::Saloon() {

}

Saloon::~Saloon() {

}

void Saloon::Enter(Agent* currentAgent) {

}

void Saloon::Update(Agent* currentAgent) {

}

void Saloon::Exit(Agent* currentAgent) {

}