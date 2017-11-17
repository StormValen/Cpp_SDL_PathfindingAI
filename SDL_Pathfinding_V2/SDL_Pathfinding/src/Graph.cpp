#include "Graph.h"


Node::Node(Vector2D _position)
{
	position = _position;
}


//CONNECTIONS

Connection::Connection(Vector2D _from, Vector2D _to, int _cost)
{
	toNode.position = _to;
	fromNode.position = _from;
	cost = _cost;
}

Node Connection::GetFromNode() { return fromNode; }
Node Connection::GetToNode() { return toNode; }


//GRAPH

Graph::Graph() {}

Graph::~Graph(){}

void Graph::AddConnection(Connection _connection) {
	conexionList.push_back(_connection);
}


std::vector<Vector2D> Graph::getConnections(Vector2D _fromNode) {
	
	std::vector<Vector2D> fromNodeConnections;
	
	for (int i = 0; i < conexionList.size(); i++) {
		if(conexionList[i].GetFromNode().position == _fromNode)
			fromNodeConnections.push_back(conexionList[i].GetToNode().position);
	}
	return fromNodeConnections;
}

