#pragma once
#include "Vector2D.h"
#include <list>
#include <vector>

class Connection {
private:
	Vector2D fromNode;
	Vector2D toNode;
	float coste;
public: 
	Connection(Vector2D _fromNode, Vector2D _toNode, float _coste);
	float getCost();
	Vector2D GetFromNode();
	Vector2D GetToNode();
};

class Graph {
private:

	std::list<Connection> conexionList;
	std::vector<std::vector<int>> nodos;
public:
	Graph(std::vector< std::vector<int> > _terrain);
};

