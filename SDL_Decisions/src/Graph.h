#pragma once
#include "Vector2D.h"
#include <list>
#include <vector>

//La classe node de momento no es necesaria

class Connection {
private:
	Vector2D fromNode;
	Vector2D toNode;
	
public: 
	int cost;

	Connection(Vector2D _from, Vector2D _to, int _cost);
	Vector2D GetFromNode();
	Vector2D GetToNode();
};

class Graph {
private:

	std::vector<Connection> conexionList;
	
public:
	Graph();
	~Graph();

	void AddConnection(Connection _connection);
	std::vector<Connection> getConnections(Vector2D _fromNode);
};

