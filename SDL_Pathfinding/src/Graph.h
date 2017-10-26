#pragma once
#include "Vector2D.h"
#include <list>
struct Node
{
	Vector2D cell;
	Node *next;
	Node *before;

};
class Connection {

public: 
	float getCost();
	Node* GetFromNode();
	Node* GetToNode();
};

class Graph {
public:
	Graph();
	std::list<Connection> conexionList;

	Connection* GetConection(Node* fromNode);
};

