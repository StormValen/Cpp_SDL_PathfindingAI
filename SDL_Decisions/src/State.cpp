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
	timeInsideHome = 0;
	positionHome = Vector2D(8, 18);
}

Home::~Home() {

}

void Home::Enter(Agent* currentAgent) {
	timeInsideHome = 0;

	currentAgent->newPathNeeded = true;
	currentAgent->setTarget(positionHome);

	currentAgent->currentState->Update(currentAgent);
}

void Home::Update(Agent* currentAgent) {
	
	if (pix2cell(Vector2D(currentAgent->getPosition().x, currentAgent->getPosition().y)) == positionHome) {
		startTime = true;
	}
	
	if (timeInsideHome >= 200) {
		currentAgent->rested = true;
		startTime = false;
		currentAgent->currentState->Exit(currentAgent);
	}
	if (startTime == true) {
		timeInsideHome++;
	}

	
}

void Home::Exit(Agent* currentAgent) {
	currentAgent->currentState = currentAgent->myMine;
	currentAgent->currentState->Enter(currentAgent);
}


//MINE
Mine::Mine() {
	timeInsideMine = 0;
	positionMine = Vector2D(2, 2);
}

Mine::~Mine() {

}

void Mine::Enter(Agent* currentAgent) {
	
	timeInsideMine = 0;
	currentAgent->newPathNeeded = true;
	currentAgent->setTarget(positionMine);

	currentAgent->currentState->Update(currentAgent);
}

void Mine::Update(Agent* currentAgent) {
	
		if (currentAgent->pocketCoins == 5) {
			currentAgent->isPocketFull = true;
			startTime = false;
			currentAgent->currentState->Exit(currentAgent);
		}
		else if (timeInsideMine > 400) {
			currentAgent->thirsty = true;
			startTime = false;
			currentAgent->currentState->Exit(currentAgent);
		}
		if (pix2cell(Vector2D(currentAgent->getPosition().x, currentAgent->getPosition().y)) == positionMine) {
			startTime = true;
		}

		if (currentAgent->thirsty == false && currentAgent->isPocketFull==false && pix2cell(Vector2D(currentAgent->getPosition().x, currentAgent->getPosition().y)) == currentAgent->getTarget()) {
			currentAgent->pocketCoins++;
			currentAgent->newPathNeeded = true;
			currentAgent->setTarget(Vector2D((float)(rand() % 30 + 1), (float)(rand() % 4 + 1)));
		}
		if (startTime == true) {
			timeInsideMine++;
		}
		
}

void Mine::Exit(Agent* currentAgent) {
	if (currentAgent->thirsty == true) {
		currentAgent->currentState = currentAgent->mySaloon;
		currentAgent->currentState->Enter(currentAgent);
	}
	if (currentAgent->isPocketFull == true) {
		currentAgent->currentState = currentAgent->myBank;
		currentAgent->currentState->Enter(currentAgent);
	}
}


//BANK
Bank::Bank() {
	
	positionBank = Vector2D(22, 18);
}

Bank::~Bank() {

}

void Bank::Enter(Agent* currentAgent) {
	currentAgent->newPathNeeded = true;
	currentAgent->setTarget(positionBank);
	
	currentAgent->bankCoins += currentAgent->pocketCoins;
	currentAgent->pocketCoins = 0;
	currentAgent->isPocketFull = false;

	currentAgent->currentState->Update(currentAgent);
}

void Bank::Update(Agent* currentAgent) {

	if(pix2cell(Vector2D(currentAgent->getPosition().x, currentAgent->getPosition().y)) == currentAgent->getTarget()) {
		if (currentAgent->bankCoins % 10 == 0) {
			currentAgent->wealthy = true;
			currentAgent->currentState->Exit(currentAgent);
		}
		else {
			currentAgent->wealthy = false;
			currentAgent->currentState->Exit(currentAgent);
		}
	}
	
}

void Bank::Exit(Agent* currentAgent) {
	if (currentAgent->wealthy == true) {
		currentAgent->rested = false;
		currentAgent->currentState = currentAgent->myHome;
		currentAgent->currentState->Enter(currentAgent);
	}
	else if (currentAgent->wealthy == false) {
		currentAgent->currentState = currentAgent->myMine;
		currentAgent->currentState->Enter(currentAgent);
	}
}


//SALOON
Saloon::Saloon() {
	timeInsideSaloon = 0;
	positionSaloon = Vector2D(30, 18);
}

Saloon::~Saloon() {

}

void Saloon::Enter(Agent* currentAgent) {
	timeInsideSaloon = 0;
	currentAgent->newPathNeeded = true;
	currentAgent->setTarget(positionSaloon);

	currentAgent->currentState->Update(currentAgent);
}

void Saloon::Update(Agent* currentAgent) {

	if (pix2cell(Vector2D(currentAgent->getPosition().x, currentAgent->getPosition().y)) == currentAgent->getTarget()) {
		startTime = true;
	}

	if (timeInsideSaloon >= 300) {
		currentAgent->thirsty = false;
		startTime = false;
		currentAgent->currentState->Exit(currentAgent);
		
	}
	if (startTime == true) {
		timeInsideSaloon++;
	}
}

void Saloon::Exit(Agent* currentAgent) {
	currentAgent->currentState = currentAgent->myMine;
	currentAgent->currentState->Enter(currentAgent);
}