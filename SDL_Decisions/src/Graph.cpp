#include "Graph.h"


Connection::Connection(Vector2D _from, Vector2D _to, int _cost)
{
	cost = _cost;
	fromNode = _from;
	toNode = _to;
}

Vector2D Connection::GetFromNode()
{
	return fromNode;
}

Vector2D Connection::GetToNode()
{
	return toNode;
}



//GRAPH

Graph::Graph() {}

Graph::~Graph(){}

void Graph::AddConnection(Connection _connection) {
	conexionList.push_back(_connection);
}


std::vector<Connection> Graph::getConnections(Vector2D _fromNode) {
	
	std::vector<Connection> fromNodeConnections;
	
	for (int i = 0; i < conexionList.size(); i++) {
		if(conexionList[i].GetFromNode() == _fromNode)
			fromNodeConnections.push_back(conexionList[i]);
	}
	return fromNodeConnections;
}

