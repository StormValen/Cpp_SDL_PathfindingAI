#include "Graph.h"

/*Node::Node(Vector2D _position, Node* _came_from) {
	position = _position;
	came_from = _came_from;
}*/

Connection::Connection(Vector2D _fromNode, Vector2D _toNode, float _coste) {
	toNode = _toNode;
	fromNode = _fromNode;
	coste = _coste;
}

float Connection::getCost() { return coste; }

Vector2D Connection::GetFromNode() { return fromNode; }
Vector2D Connection::GetToNode() { return toNode; }

Graph::Graph(std::vector< std::vector<int> > _terrain) {
	//recorrer terrain


	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 40; j++) {
			nodos[i][j] = _terrain[i][j];
			if (nodos[i][j] == 1) {

				//nodo abajo  [col][row]
				if (nodos[i++][j] == 1) {
					Connection myNewConnection(Vector2D(i, j), Vector2D(i++, j), 1.f);
					conexionList.push_back(myNewConnection);
				}

				//nodo arriba  [col][row]
				if (nodos[i--][j] == 1) {
					Connection myNewConnection(Vector2D(i, j), Vector2D(i--, j), 1.f);
					conexionList.push_back(myNewConnection);
				}

				//nodo derecha  [col][row]
				if (nodos[i][j++] == 1) {
					Connection myNewConnection(Vector2D(i, j), Vector2D(i, j++), 1.f);
					conexionList.push_back(myNewConnection);
				}

				//nodo izquierda  [col][row]
				if (nodos[i][j--] == 1) {
					Connection myNewConnection(Vector2D(i, j), Vector2D(i, j--), 1.f);
					conexionList.push_back(myNewConnection);
				}
			}
		}
	}
}

std::list<Connection> Graph::getConnections(Vector2D _fromNode, std::list<Vector2D> _nodosVisitados) {
	std::list<Connection> fromNodeConnections;
	for (std::list<Connection>::iterator it = conexionList.begin(); it != conexionList.end(); ++it) {
		if (it->GetFromNode() == _fromNode) {
			fromNodeConnections.push_back(*it);
		}
	}

	for (std::list<Vector2D>::iterator it = _nodosVisitados.begin(); it != _nodosVisitados.end(); ++it) {
		for (std::list<Connection>::iterator it_2 = fromNodeConnections.begin(); it_2 != fromNodeConnections.end(); ++it_2) {
			if (it->x != it_2->GetToNode().x || it->y != it_2->GetToNode().y) {
				fromNodeConnections.remove(*it_2);
			}
		}
	}

	return fromNodeConnections;
}