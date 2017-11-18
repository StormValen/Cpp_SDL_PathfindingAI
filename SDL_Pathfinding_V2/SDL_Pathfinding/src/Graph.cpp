#include "Graph.h"

//GRAPH

Graph::Graph() {}

Graph::~Graph(){}

void Graph::AddConnection(Connection _connection) {
	conexionList.push_back(_connection);
}


std::vector<Vector2D> Graph::getConnections(Vector2D _fromNode) {
	
	std::vector<Vector2D> fromNodeConnections;
	
	for (int i = 0; i < conexionList.size(); i++) {
		if(conexionList[i].GetFromNode() == _fromNode)
			fromNodeConnections.push_back(conexionList[i].GetToNode());
	}
	return fromNodeConnections;
}

