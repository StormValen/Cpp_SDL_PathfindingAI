#pragma once
#include"Vector2D.h"

class Agent;

class State {
private:
public:
	State();
	~State();
	virtual void Enter(Agent* currentAgent);
	virtual void Update(Agent* currentAgent);
	virtual void Exit(Agent* currentAgent);

	Vector2D cell2pix(Vector2D cell)
	{
		int offset = 32 / 2;
		return Vector2D(cell.x*32 + offset, cell.y*32 + offset);
	}

	Vector2D pix2cell(Vector2D pix)
	{
		return Vector2D((float)((int)pix.x / 32), (float)((int)pix.y / 32));
	}
};

class Home : public State {
private:
	int timeInsideHome;
	Vector2D positionHome;
	bool startTime;
public:
	Home();
	~Home();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};

class Mine : public State {
private:
	int timeInsideMine;
	Vector2D positionMine;
	Vector2D currentCoint;
	bool startTime;
public:
	Mine();
	~Mine();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};

class Bank : public State {
private:

	Vector2D positionBank;
public:
	Bank();
	~Bank();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};

class Saloon : public State {
private:
	int timeInsideSaloon;
	Vector2D positionSaloon;
	bool startTime;
public:
	Saloon();
	~Saloon();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};