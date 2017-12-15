#pragma once

class Agent;

class State {
private:
public:
	State();
	~State();
	virtual void Enter(Agent* currentAgent);
	virtual void Update(Agent* currentAgent);
	virtual void Exit(Agent* currentAgent);
};

class Home : public State {
private:
public:
	Home();
	~Home();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};

class Mine : public State {
private:
public:
	Mine();
	~Mine();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};

class Bank : public State {
private:
public:
	Bank();
	~Bank();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};

class Saloon : public State {
private:
public:
	Saloon();
	~Saloon();
	void Enter(Agent* currentAgent);
	void Update(Agent* currentAgent);
	void Exit(Agent* currentAgent);
};