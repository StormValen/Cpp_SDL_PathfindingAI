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
	else if (algoritmo == 2) {
		path.points = Dijkstra((agents[0]->getPosition()), cell2pix(coinPosition));
	}
	else if (algoritmo == 3){
		path.points = GBFS((agents[0]->getPosition()), cell2pix(coinPosition));
	}
	else if (algoritmo == 4) {
		path.points = A_estrella((agents[0]->getPosition()), cell2pix(coinPosition));
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
							
							//FET GRACIES AL GRUP DE LA CLASSE A FORMAT PEL TONI FERRARI (ENTI 3A DEVELOPER)
							agents[0]->setPosition(path.points.back()); 
							vector<Vector2D> temp;
							//////////////////////////////////////////

							if (algoritmo == 1) {
								temp = BFS(agents[0]->getPosition(), cell2pix(coinPosition));
							}
							else if (algoritmo == 2) {
								temp = Dijkstra(agents[0]->getPosition(), cell2pix(coinPosition));
							}
							else if (algoritmo == 3) {
								temp = GBFS(agents[0]->getPosition(), cell2pix(coinPosition));
							}
							else if (algoritmo == 4) {
								temp = A_estrella(agents[0]->getPosition(), cell2pix(coinPosition));
							}

							//FET GRACIES AL GRUP DE LA CLASSE A FORMAT PEL TONI FERRARI (ENTI 3A DEVELOPER)
							path.points.clear();
							path.points = temp;
							//////////////////////////////////////////
							
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
					if (algoritmo == 1 || algoritmo == 3) { //BFS and GBFS
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i + 1, j)), 1);
						myGraph.AddConnection(myNewConnection);
					}
					else { //los otros que tienen coste
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i + 1, j)), (int)(rand() % 5));
						myGraph.AddConnection(myNewConnection);
					}
				}

				//nodo arriba  [col][row]
				if (isValidCell(Vector2D(i - 1, j)) && terrain[i - 1][j] == 1) {
					if (algoritmo == 1 || algoritmo == 3) {//BFS and GBFS
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i - 1, j)), 1);
						myGraph.AddConnection(myNewConnection);
					}
					else { //los otros que tienen coste
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i - 1, j)), (int)(rand() % 5));
						myGraph.AddConnection(myNewConnection);
					}
				}

				//nodo derecha  [col][row]
				if (isValidCell(Vector2D(i, j + 1)) && terrain[i][j + 1] == 1) {
					if (algoritmo == 1 || algoritmo == 3) {//BFS and GBFS
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i, j + 1)), 1);
						myGraph.AddConnection(myNewConnection);
					}
					else { //los otros que tienen coste
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i, j + 1)), (int)(rand() % 5));
						myGraph.AddConnection(myNewConnection);
					}
				}

				//nodo izquierda  [col][row]
				if (isValidCell(Vector2D(i, j - 1)) && terrain[i][j - 1] == 1) {
					if (algoritmo == 1 || algoritmo == 3) {//BFS and GBFS
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i, j - 1)), 1);
						myGraph.AddConnection(myNewConnection);
					}
					else { //los otros que tienen coste
						Connection myNewConnection(cell2pix(Vector2D(i, j)), cell2pix(Vector2D(i, j - 1)), (int)(rand() % 5));
						myGraph.AddConnection(myNewConnection);
					}
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
		
		for each (Connection connection in  myGraph.getConnections(current))
		{
			//no cal tenir una llista de nodes visitats, podem mirar directament a la llista del came_from
			if (came_from.find(connection.GetToNode()) == came_from.end()) { //el find no retorna un bool,  mireu aix�: https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key
				//la connexio que estem mirant no existeix
				
				if (connection.GetToNode() == _targetCell) { //trobat

					path.push_back(connection.GetToNode());
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
				came_from[connection.GetToNode()] = current; //actualitzo el mapa came_from, el current es d'on ve
				frontier.push(connection.GetToNode()); //amplio la frontera

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

	map<Vector2D, int> cost_so_far;
	cost_so_far[_startCell] = 0; //es 0 perque es el primer
	std::map<Vector2D, int>::iterator it; // ens permet guardar el que ens torna el find http://www.cplusplus.com/reference/map/map/find/

	pair<Vector2D, int> current;

	while (!frontier.empty()) //fins que no estigui buida la cua
	{
		current = frontier.top(); //afagem el seguent a analitzar, el guardem en una variable temporal
		frontier.pop(); //ara ja podem borrar-lo de la cua (frontier)

						//agafem al menys quatre connexions del node qu estem avaluant

		for each (Connection connection in  myGraph.getConnections(current.first))
		{
			int newCost = current.second + connection.cost; //cojemos el coste de la connexion que hemos puesto al inicio en el graph
			
			it = cost_so_far.find(connection.GetToNode()); //el busquem 

			//si no existeix fem els calculs o be si existeix pero el now cost es mes petit sevalua igual
			if (it == cost_so_far.end() || (newCost < it->second)) { 

				if (connection.GetToNode() == _targetCell) { //trobat

					path.push_back(connection.GetToNode());
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
				came_from[connection.GetToNode()] = current.first; //actualitzo el mapa came_from, el current es d'on ve
				cost_so_far[connection.GetToNode()] = newCost;
				pair<Vector2D, int> newFrontierCell(connection.GetToNode(), newCost);
				frontier.push(newFrontierCell); //amplio la frontera

			}
		}
	}

}

std::vector<Vector2D> ScenePathFinding::GBFS(Vector2D _startCell, Vector2D _targetCell) //EL GBFS SE BASA EN DIJKSTRA , NO EN BFS, SE CAMBIA EL COSTE ACUMULADO POR LA PRIORIDAD CALCULADA EN LA HEURISTICA.
{
	std::vector<Vector2D> path; //vector que he de tornar, per agafar els punts.

	std::priority_queue<pair<Vector2D, int>, vector<pair<Vector2D, int>>, CompareCost> frontier;
	pair<Vector2D, int> newFrontierCell(_startCell, 0);
	frontier.push(newFrontierCell);

	std::map<Vector2D, Vector2D> came_from;
	came_from[_startCell] = NULL; //es null perque es el primer, no ve de ningu

	pair<Vector2D, int> current;

	while (!frontier.empty()) //fins que no estigui buida la cua
	{
		current = frontier.top(); //afagem el seguent a analitzar, el guardem en una variable temporal
		frontier.pop(); //ara ja podem borrar-lo de la cua (frontier)

						//agafem al menys quatre connexions del node qu estem avaluant

		for each (Connection connection in  myGraph.getConnections(current.first))
		{
			//no cal tenir una llista de nodes visitats, podem mirar directament a la llista del came_from
			if (came_from.find(connection.GetToNode()) == came_from.end()) { //el find no retorna un bool,  mireu aix�: https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key
														   //la connexio que estem mirant no existeix

				if (connection.GetToNode() == _targetCell) { //trobat

					path.push_back(connection.GetToNode());
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
				came_from[connection.GetToNode()] = current.first; //actualitzo el mapa came_from, el current es d'on ve

				int priority = abs(current.first.x - _targetCell.x) + abs(current.first.y - _targetCell.y); // calculas la distacia al nodo objetivo segun hueristicas, este suma es lo que nos indica la prioridad en la priority queue

				pair<Vector2D, int> newFrontierCell(connection.GetToNode(), priority);
				frontier.push(newFrontierCell); //amplio la frontera

			}
		}
	}

}

std::vector<Vector2D> ScenePathFinding::A_estrella(Vector2D _startCell, Vector2D _targetCell) //EL GBFS SE BASA EN DIJKSTRA , NO EN BFS, SE CAMBIA EL COSTE ACUMULADO POR LA PRIORIDAD CALCULADA EN LA HEURISTICA.
{
	std::vector<Vector2D> path; //vector que he de tornar, per agafar els punts.

	std::priority_queue<pair<Vector2D, int>, vector<pair<Vector2D, int>>, CompareCost> frontier;
	pair<Vector2D, int> newFrontierCell(_startCell, 0);
	frontier.push(newFrontierCell);

	std::map<Vector2D, Vector2D> came_from;
	came_from[_startCell] = NULL; //es null perque es el primer, no ve de ningu

	map<Vector2D, int> cost_so_far;
	cost_so_far[_startCell] = 0; //es 0 perque es el primer
	std::map<Vector2D, int>::iterator it; // ens permet guardar el que ens torna el find http://www.cplusplus.com/reference/map/map/find/

	pair<Vector2D, int> current;

	while (!frontier.empty()) //fins que no estigui buida la cua
	{
		current = frontier.top(); //afagem el seguent a analitzar, el guardem en una variable temporal
		frontier.pop(); //ara ja podem borrar-lo de la cua (frontier)

						//agafem al menys quatre connexions del node qu estem avaluant

		for each (Connection connection in  myGraph.getConnections(current.first))
		{
			int newCost = current.second + connection.cost; //cojemos el coste de la connexion que hemos puesto al inicio en el graph

			it = cost_so_far.find(connection.GetToNode()); //el busquem 

														   //si no existeix fem els calculs o be si existeix pero el now cost es mes petit sevalua igual
			if (it == cost_so_far.end() || (newCost < it->second)) {

				if (connection.GetToNode() == _targetCell) { //trobat

					path.push_back(connection.GetToNode());
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
				came_from[connection.GetToNode()] = current.first; //actualitzo el mapa came_from, el current es d'on ve

				cost_so_far[connection.GetToNode()] = newCost;

				int priority = abs(current.first.x - _targetCell.x) + abs(current.first.y - _targetCell.y); // calculas la distacia al nodo objetivo segun hueristicas, este suma es lo que nos indica la prioridad en la priority queue

				pair<Vector2D, int> newFrontierCell(connection.GetToNode(), newCost + priority);
				frontier.push(newFrontierCell); //amplio la frontera

			}
		}
	}

}