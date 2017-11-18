#include "ScenePathFinding.h"
#include "Graph.h"
#include <queue>

using namespace std;

ScenePathFinding::ScenePathFinding(int _algoritmo)
{
	algoritmo = _algoritmo;
	draw_grid = true;

	num_cell_x = SRC_WIDTH / CELL_SIZE;
	num_cell_y = SRC_HEIGHT / CELL_SIZE;
	initMaze();

	loadTextures("../res/maze.png", "../res/coin.png");

	srand((unsigned int)time(NULL));

	Agent *agent = new Agent;
	agent->loadSpriteTexture("../res/soldier.png", 4);
	agents.push_back(agent);


	// set agent position coords to the center of a random cell
	Vector2D rand_cell(-1,-1);
	while (!isValidCell(rand_cell)) 
		rand_cell = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
	agents[0]->setPosition(cell2pix(rand_cell));

	// set the coin in a random cell (but at least 3 cells far from the agent)
	coinPosition = Vector2D(-1,-1);
	while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, rand_cell)<3)) 
		coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
	
	// PathFollowing next Target
	currentTarget = Vector2D(0, 0);
	currentTargetIndex = -1;

	//Apliquem Pathfinding
	GraphAllCellsConnections(); //creo todas las connexiones
	if (algoritmo == 1) {
		path.points = BFS((agents[0]->getPosition()), cell2pix(coinPosition));  //inici i final amb pixels  /PER AIXO ES VEIEN TOTS ELS PUNTS JUNTS}
	}

}

ScenePathFinding::~ScenePathFinding()
{
	if (background_texture)
		SDL_DestroyTexture(background_texture);
	if (coin_texture)
		SDL_DestroyTexture(coin_texture);

	for (int i = 0; i < (int)agents.size(); i++)
	{
		delete agents[i];
	}
}

void ScenePathFinding::update(float dtime, SDL_Event *event)
{

	/* Keyboard & Mouse events */
	switch (event->type) {
	case SDL_KEYDOWN:
		if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)
			draw_grid = !draw_grid;
		break;
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			Vector2D cell = pix2cell(Vector2D((float)(event->button.x), (float)(event->button.y)));
			if (isValidCell(cell))
			{
				if (path.points.size() > 0)
					if (path.points[path.points.size() - 1] == cell2pix(cell))
						break;

				path.points.push_back(cell2pix(cell));
			}
		}
		break;
	default:
		break;
	}

	if ((currentTargetIndex == -1) && (path.points.size()>0))
		currentTargetIndex = 0;

	if (currentTargetIndex >= 0)
	{	
		float dist = Vector2D::Distance(agents[0]->getPosition(), path.points[currentTargetIndex]);
		if (dist < path.ARRIVAL_DISTANCE)
		{
			if (currentTargetIndex == path.points.size() - 1)
			{
				if (dist < 3)
				{
					currentTargetIndex = -1;
					agents[0]->setVelocity(Vector2D(0,0));
					// if we have arrived to the coin, replace it ina random cell!
					if (pix2cell(agents[0]->getPosition()) == coinPosition)
					{
						coinPosition = Vector2D(-1, -1);
						while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, pix2cell(agents[0]->getPosition()))<3))
							coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
							
							agents[0]->setPosition(path.points.back());
							vector<Vector2D> temp;
							if (algoritmo == 1) {
								temp = BFS(agents[0]->getPosition(), cell2pix(coinPosition));
							}
				
							path.points.clear();
							path.points = temp;
					}
				}
				else
				{
					Vector2D steering_force = agents[0]->Behavior()->Arrive(agents[0], currentTarget, path.ARRIVAL_DISTANCE, dtime);
					agents[0]->update(steering_force, dtime, event);
				}
				return;
			}
			currentTargetIndex++;
		}

		currentTarget = path.points[currentTargetIndex];
		Vector2D steering_force = agents[0]->Behavior()->Seek(agents[0], currentTarget, dtime);
		agents[0]->update(steering_force, dtime, event);
	} 
	else
	{
		agents[0]->update(Vector2D(0, 0), dtime, event);
	}
}

void ScenePathFinding::draw()
{
	drawMaze();
	drawCoin();


	if (draw_grid)
	{
		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 255, 255, 255, 127);
		for (int i = 0; i < SRC_WIDTH; i+=CELL_SIZE)
		{
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), i, 0, i, SRC_HEIGHT);
		}
		for (int j = 0; j < SRC_HEIGHT; j = j += CELL_SIZE)
		{
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), 0, j, SRC_WIDTH, j);
		}
	}

	for (int i = 0; i < (int)path.points.size(); i++)
	{
		draw_circle(TheApp::Instance()->getRenderer(), (int)(path.points[i].x), (int)(path.points[i].y), 15, 255, 255, 0, 255);
		if (i > 0)
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), (int)(path.points[i - 1].x), (int)(path.points[i - 1].y), (int)(path.points[i].x), (int)(path.points[i].y));
	}

	draw_circle(TheApp::Instance()->getRenderer(), (int)currentTarget.x, (int)currentTarget.y, 15, 255, 0, 0, 255);

	agents[0]->draw();
}

const char* ScenePathFinding::getTitle()
{
	return "SDL Steering Behaviors :: PathFinding1 Demo";
}

void ScenePathFinding::drawMaze()
{
	if (draw_grid)
	{

		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 0, 0, 255, 255);
		for (unsigned int i = 0; i < maze_rects.size(); i++)
			SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &maze_rects[i]);
	}
	else
	{
		SDL_RenderCopy(TheApp::Instance()->getRenderer(), background_texture, NULL, NULL );
	}
}

void ScenePathFinding::drawCoin()
{
	Vector2D coin_coords = cell2pix(coinPosition);
	int offset = CELL_SIZE / 2;
	SDL_Rect dstrect = {(int)coin_coords.x-offset, (int)coin_coords.y - offset, CELL_SIZE, CELL_SIZE};
	SDL_RenderCopy(TheApp::Instance()->getRenderer(), coin_texture, NULL, &dstrect);
}

void ScenePathFinding::initMaze()
{

	// Initialize a list of Rectagles describing the maze geometry (useful for collision avoidance)
	SDL_Rect rect = { 0, 0, 1280, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 32, 64, 32 };
	maze_rects.push_back(rect);
	rect = { 0, 736, 1280, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 512, 64, 224 }; 
	maze_rects.push_back(rect);
	rect = { 0,32,32,288 };
	maze_rects.push_back(rect);
	rect = { 0,416,32,320 };
	maze_rects.push_back(rect);
	rect = { 1248,32,32,288 };
	maze_rects.push_back(rect);
	rect = { 1248,416,32,320 };
	maze_rects.push_back(rect);
	rect = { 128,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 288,128,96,32 };
	maze_rects.push_back(rect);
	rect = { 480,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 736,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 896,128,96,32 };
	maze_rects.push_back(rect);
	rect = { 1088,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 128,256,64,32 };
	maze_rects.push_back(rect);
	rect = { 288,256,96,32 };
	maze_rects.push_back(rect);
	rect = { 480, 256, 320, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 224, 64, 32 }; 
	maze_rects.push_back(rect);
	rect = { 896,256,96,32 };
	maze_rects.push_back(rect);
	rect = { 1088,256,64,32 };
	maze_rects.push_back(rect);
	rect = { 128,384,32,256 };
	maze_rects.push_back(rect);
	rect = { 160,512,352,32 };
	maze_rects.push_back(rect);
	rect = { 1120,384,32,256 };
	maze_rects.push_back(rect);
	rect = { 768,512,352,32 };
	maze_rects.push_back(rect);
	rect = { 256,640,32,96 };
	maze_rects.push_back(rect);
	rect = { 992,640,32,96 };
	maze_rects.push_back(rect);
	rect = { 384,544,32,96 };
	maze_rects.push_back(rect);
	rect = { 480,704,32,32 };
	maze_rects.push_back(rect);
	rect = { 768,704,32,32 };
	maze_rects.push_back(rect);
	rect = { 864,544,32,96 };
	maze_rects.push_back(rect);
	rect = { 320,288,32,128 };
	maze_rects.push_back(rect);
	rect = { 352,384,224,32 };
	maze_rects.push_back(rect);
	rect = { 704,384,224,32 };
	maze_rects.push_back(rect);
	rect = { 928,288,32,128 };
	maze_rects.push_back(rect);

	// Initialize the terrain matrix (for each cell a zero value indicates it's a wall)
	
	// (1st) initialize all cells to 1 by default
	for (int i = 0; i < num_cell_x; i++)
	{
		vector<int> terrain_col(num_cell_y, 1); 
		terrain.push_back(terrain_col);
	}
	// (2nd) set to zero all cells that belong to a wall
	int offset = CELL_SIZE / 2;
	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			Vector2D cell_center ((float)(i*CELL_SIZE + offset), (float)(j*CELL_SIZE + offset));
			for (unsigned int b = 0; b < maze_rects.size(); b++)
			{
				if (Vector2DUtils::IsInsideRect(cell_center, (float)maze_rects[b].x, (float)maze_rects[b].y, (float)maze_rects[b].w, (float)maze_rects[b].h))
				{
					terrain[i][j] = 0;
				    break;
				}  
			}
			
		}
	}

}

bool ScenePathFinding::loadTextures(char* filename_bg, char* filename_coin)
{
	SDL_Surface *image = IMG_Load(filename_bg);
	if (!image) {
		cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	background_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	if (image)
		SDL_FreeSurface(image);

	image = IMG_Load(filename_coin);
	if (!image) {
		cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	coin_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	if (image)
		SDL_FreeSurface(image);

	return true;
}

Vector2D ScenePathFinding::cell2pix(Vector2D cell)
{
	int offset = CELL_SIZE / 2;
	return Vector2D(cell.x*CELL_SIZE + offset, cell.y*CELL_SIZE + offset);
}

Vector2D ScenePathFinding::pix2cell(Vector2D pix)
{
	return Vector2D((float)((int)pix.x/CELL_SIZE), (float)((int)pix.y / CELL_SIZE));
}

bool ScenePathFinding::isValidCell(Vector2D cell)
{
	if ((cell.x < 0) || (cell.y < 0) || (cell.x >= terrain.size()) || (cell.y >= terrain[0].size()) )
		return false;
	return !(terrain[(unsigned int)cell.x][(unsigned int)cell.y] == 0);
}


//TODAS LAS CONEXIONES DEL GRAF

void ScenePathFinding::GraphAllCellsConnections() {
	//recorrer terrain

	for (int i = 0; i < num_cell_x; i++) {
		for (int j = 0; j < num_cell_y; j++) {
			if (terrain[i][j] == 1) {

				//is valid cell ens permet saber si exiteix la casella que busquem (fet pel profe) - si no OUT OF RANGE!!!
				//el PATH.POINTS ho demana amb pixels, per tant al fer la connexio ho he de pasar a pixels CelltoPixel
				
				//nodo abajo  [col][row]
				if (isValidCell(Vector2D(i + 1, j)) && terrain[i + 1][j] == 1) {
					Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i + 1, j)), 1);
					myGraph.AddConnection(myNewConnection);
			
				}

				//nodo arriba  [col][row]
				if (isValidCell(Vector2D(i - 1, j)) && terrain[i - 1][j] == 1) {
					Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i - 1, j)), 1);
					myGraph.AddConnection(myNewConnection);
				}

				//nodo derecha  [col][row]
				if (isValidCell(Vector2D(i, j + 1)) && terrain[i][j + 1] == 1) {
					Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i, j + 1)), 1);
					myGraph.AddConnection(myNewConnection);
				}

				//nodo izquierda  [col][row]
				if (isValidCell(Vector2D(i, j - 1)) && terrain[i][j - 1] == 1) {
					Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i, j - 1)), 1);
					myGraph.AddConnection(myNewConnection);
				}
			}
		}
	}
}


//ALGORITMOS PATHFINDING

std::vector<Vector2D> ScenePathFinding::BFS(Vector2D _startCell, Vector2D _targetCell)
{
	std::vector<Vector2D> path; //vector que he de tornar, per agafar els punts.
	
	std::queue<Vector2D> frontier;
	frontier.push(_startCell);
	
	std::map<Vector2D, Vector2D> came_from;
	came_from[_startCell] = NULL; //es null perque es el primer, no ve de ningu

	Vector2D current;

	while (!frontier.empty()) //fins que no estigui buida la cua
	{
		current = frontier.front(); //afagem el seguent a analitzar, el guardem en una variable temporal
		frontier.pop(); //ara ja podem borrar-lo de la cua (frontier)

		//agafem al menys quatre connexions del node qu estem avaluant
		
		for each (Vector2D node in  myGraph.getConnections(current))
		{
			//no cal tenir una llista de nodes visitats, podem mirar directament a la llista del came_from
			if (came_from.find(node) == came_from.end()) { //el find no retorna un bool,  mireu això: https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key
				//la connexio que estem mirant no existeix
				
				if (node == _targetCell) { //trobat

					path.push_back(node);
					path.push_back(current); 
					while (current != _startCell) {
						current = came_from[current]; //el current es el fromNode del que estem mirant
						path.push_back(current);
					}
					path.push_back(_startCell);
					std::reverse(path.begin(), path.end()); //girem, ja que el cami trobat esta a l'inversa
					return path;
				}

				//no cal fer-ho si ja s'ha trobat el target, no entra a causa del return
				came_from[node] = current; //actualitzo el mapa came_from, el current es d'on ve
				frontier.push(node); //amplio la frontera

			}
		}
	}
	
}

struct CompareCost
{
	public:
		bool operator()(pair<Vector2D, int> n1, pair<Vector2D, int> n2) {
			return n1.second > n2.second;
		}
};


std::vector<Vector2D> ScenePathFinding::Dijkstra(Vector2D _startCell, Vector2D _targetCell)
{
	std::vector<Vector2D> path; //vector que he de tornar, per agafar els punts.

	std::priority_queue<pair<Vector2D, int>, vector<pair<Vector2D, int>>, CompareCost> frontier;
	pair<Vector2D, int> newFrontierCell(_startCell, 0);
	frontier.push(newFrontierCell);

	std::map<Vector2D, Vector2D> came_from;
	came_from[_startCell] = NULL; //es null perque es el primer, no ve de ningu

	map<Vector2D, float> cost_so_far;
	cost_so_far[_startCell] = 0; //es 0 perque es el primer

	pair<Vector2D, int> current;

	while (!frontier.empty()) //fins que no estigui buida la cua
	{
		current = frontier.top(); //afagem el seguent a analitzar, el guardem en una variable temporal
		frontier.pop(); //ara ja podem borrar-lo de la cua (frontier)

						//agafem al menys quatre connexions del node qu estem avaluant

		for each (Vector2D node in  myGraph.getConnections(current.first))
		{
			//no cal tenir una llista de nodes visitats, podem mirar directament a la llista del came_from
			if (came_from.find(node) == came_from.end()) { //el find no retorna un bool,  mireu això: https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key
														   //la connexio que estem mirant no existeix

				if (node == _targetCell) { //trobat

					path.push_back(node);
					path.push_back(current.first);
					while (current.first != _startCell) {
						current.first = came_from[current.first]; //el current es el fromNode del que estem mirant
						path.push_back(current.first);
					}
					path.push_back(_startCell);
					std::reverse(path.begin(), path.end()); //girem, ja que el cami trobat esta a l'inversa
					return path;
				}

				//no cal fer-ho si ja s'ha trobat el target, no entra a causa del return
				came_from[node] = current.first; //actualitzo el mapa came_from, el current es d'on ve
				cost_so_far[current.first] = current.second + 1;
				pair<Vector2D, int> newFrontierCell(current.first, current.second);
				frontier.push(newFrontierCell); //amplio la frontera

			}
		}
	}

}

