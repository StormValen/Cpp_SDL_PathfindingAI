#pragma once
#include "Vector2D.h"
#include <list>
#include <vector>

/*class Node {
private:
	Node* came_from;
	Vector2D position;
public: 
	Node(Vector2D _position, Node* _came_from);
};*/

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
	std::list<Connection> getConnections(Vector2D _fromNode, std::list<Vector2D> _nodosVisitados);
};

