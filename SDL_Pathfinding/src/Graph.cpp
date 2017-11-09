#include "Graph.h"

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