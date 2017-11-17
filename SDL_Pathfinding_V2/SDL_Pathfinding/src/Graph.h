#pragma once
#include "Vector2D.h"
#include <list>
#include <vector>


class Node {

public :
	Node() {}
	Node(Vector2D _position);
	Vector2D position;
	int cost_so_far;
};


class Connection {
private:
	Node fromNode;
	Node toNode;
	
public: 
	int cost;

	Connection(Vector2D _from, Vector2D _to, int _cost);
	Node GetFromNode();
	Node GetToNode();
};

class Graph {
private:

	std::vector<Connection> conexionList;
	
public:
	Graph();
	~Graph();

	void AddConnection(Connection _connection);
	std::vector<Vector2D> getConnections(Vector2D _fromNode);
};

