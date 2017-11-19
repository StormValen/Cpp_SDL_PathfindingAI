#pragma once
#include <vector>
#include <time.h>
#include "Scene.h"
#include "Agent.h"
#include "Path.h"
#include "Graph.h"
#include <map>

class ScenePathFinding : public Scene {
private:
	std::vector<Agent*> agents;
	Vector2D coinPosition;
	Vector2D currentTarget;
	int currentTargetIndex;
	Path path;
	int num_cell_x;
	int num_cell_y;
	bool draw_grid;
	std::vector<SDL_Rect> maze_rects;
	void drawMaze();
	void drawCoin();
	SDL_Texture *background_texture;
	SDL_Texture *coin_texture;
	void initMaze();
	bool loadTextures(char* filename_bg, char* filename_coin);
	Vector2D cell2pix(Vector2D cell);
	Vector2D pix2cell(Vector2D pix);
	bool isValidCell(Vector2D cell);

	//Nuestros Pathfinding
	std::vector<Vector2D> BFS(Vector2D _startCell, Vector2D _endCell);
	std::vector<Vector2D> Dijkstra(Vector2D _startCell, Vector2D _endCell);
	std::vector<Vector2D> GBFS(Vector2D _startCell, Vector2D _endCell);


public:
	std::vector< std::vector<int> > terrain;
	ScenePathFinding(int algoritmo);
	~ScenePathFinding();
	void update(float dtime, SDL_Event *event);
	void draw();
	const char* getTitle();

	//Nuestro graph
	int algoritmo;
	Graph myGraph;
	void GraphAllCellsConnections();
};
