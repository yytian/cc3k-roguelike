#include "floor.h"
#include "entity.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>
#include <ctime>
using namespace std;

Floor *Floor::curFloor = NULL; // have to define somewhere? ugly

Floor::Floor(Character *player): player(player), wentDownStairs(false), log("") {
  srand(time(NULL));
  ifstream file("defaultfloor.flr");
  if(!file.is_open()) {
    cerr << "Could not find defaultfloor.flr" << endl;
    exit(2);
  }
  read(file, player);
  file.close();

  int x = 0, y = 0, bannedChamber = -1;

  randomLocation(x,y,bannedChamber); // all chambers allowed
  entities[y][x]->occupy(player);
  player->setX(x);
  player->setY(y);

  randomLocation(x,y,bannedChamber);
  delete entities[y][x];
  entities[y][x] = new Stairs(x,y);

  for(int i = 0; i < 10; i++) {
    randomLocation(x,y);
    entities[y][x]->occupy(Potion::generateRandom(x,y));
  }

  for(int i = 0; i < 10; i++) {
    randomLocation(x,y);
    Gold *gold = Gold::generateRandom(x,y);
    entities[y][x]->occupy(gold);
    if(gold->getValue() == 6) {
      while(true) {
	int deltaX, deltaY;
	Direction dir = randomDirection(deltaX, deltaY);
	if(entities[y+deltaY][x+deltaX]->isEmpty()) {
	  Character *dragon = new Character(x+deltaX, y+deltaY, DRAGON, false);
	  entities[y+deltaY][x+deltaX]->occupy(dragon);
	  gold->setOwner(dragon);
	  dragon->setTraits(new HoardGuard(dragon->curTraits(), gold));
	  break; // leave infinite while loop
	}
      }
    }
  }

  for(int i = 0; i < 20; i++) {
    randomLocation(x,y);
    Character *enemy = Character::randomEnemy();
    enemy->setX(x);
    enemy->setY(y);
    entities[y][x]->occupy(enemy);
  }
}

Floor::Floor(Character *player, istream &in): wentDownStairs(false) {
  srand(time(NULL));
  read(in, player);
}

Floor::~Floor() {
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      if(entities[i][j] != NULL) {
	delete entities[i][j];
      }
    }
  }
}

Entity* Floor::entityAt(int x, int y) {
  return entities[y][x];
}

void Floor::remove(Character *dead) {
  entities[dead->getY()][dead->getX()]->unoccupy(dead);
  if(!dead->isPlayer()) {
    Entity *drop = dead->curTraits()->itemDrop(dead->getX(), dead->getY());
    if(drop != NULL) {
      entities[dead->getY()][dead->getX()]->occupy(drop);
    }
    delete dead;
  }
}

void Floor::tick(Character *player) {
  int x = player->getX(), y = player-> getY();
  entities[y-1][x-1]->alertAdj(player);
  entities[y-1][x]->alertAdj(player);
  entities[y-1][x+1]->alertAdj(player);
  entities[y][x-1]->alertAdj(player);
  entities[y][x+1]->alertAdj(player);
  entities[y+1][x-1]->alertAdj(player);
  entities[y+1][x]->alertAdj(player);
  entities[y+1][x+1]->alertAdj(player);

  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      if(entities[i][j] != NULL) {
	entities[i][j]->act();
      }
    }
  }
  write(cout);
}

bool Floor::wentDown() {
  return wentDownStairs;
}

void Floor::goDown() {
  wentDownStairs = true;
}

bool Floor::gameOver() {
  return !(player->curTraits()->isAlive());
}

void Floor::read(istream &in, Character *player) { // overloading the operator led to annoying problems
  char c;
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      if(in.good()) {  // successful reads
	in.get(c); // puts one char into c; doing it this way to preserve whitespace
	Character *temp = NULL;
	switch(c) {
	case '@':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(player);
	  player->setX(j);
	  player->setY(i);
	  break;
        case '-':
          entities[i][j] = new Wall(j, i,false);
          break; 
	case '|':
	  entities[i][j] = new Wall(j, i,true);
	  break;
	case '.':
	  entities[i][j] = new Ground(j, i);
	  break;
	case '+':
	  entities[i][j] = new Door(j, i);
	  break;
	case '#':
	  entities[i][j] = new Passage(j, i);
	  break;
	case ' ':
	  entities[i][j] = new Nothing();
	  break;
	case '>':
	  entities[i][j] = new Stairs(j, i);
	  break;
	case 'V':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Character(j, i, VAMPIRE, false));
	  break;
	case 'M':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Character(j, i, MERCHANT, false));
	  break;
	case '0':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Potion(j, i, RH));
	  break;
	case '1':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Potion(j, i, BA));
	  break;
	case '2':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Potion(j, i, BD));
	  break;
	case '3':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Potion(j, i, PH));
	  break;
	case '4':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Potion(j, i, WA));
	  break;
	case '5':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Potion(j, i, WD));	
	  break;
	case '6':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Gold(j, i, 1));
	  break;
	case '7':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Gold(j, i, 2));
	  break;
	case '8':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Gold(j, i, 4));
	  break;
	case '9':
	  entities[i][j] = new Ground(j, i);
	  entities[i][j]->occupy(new Gold(j, i, 6));
	  break;
	case '\n': //ignore newlines
	  j -= 1; // we want to repeat an input
	  break;
	default:
	  cerr << "Unrecognized floor entity: " << c << endl;
	  return;
	}
      } else { //failed read
	cerr << "Could not read from file completely." << endl;
	return;
      }
    }
  }
}

void Floor::write(ostream &out) { //ditto
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      if(entities[i][j] != NULL) {
	out << entities[i][j]->getSymbol();
      }
    }
    out << endl;
  }
  return;
}

Direction Floor::randomDirection(int &deltaX, int &deltaY) { // two ways of returning!
  srand(time(NULL));
  int dir = rand() % 8;
  switch(dir) {
  case 0:
    deltaX = -1;
    deltaY = -1;
    return NORTHWEST;
  case 1:
    deltaX = -1;
    deltaY = 0;
    return WEST;
  case 2:
    deltaX = -1;
    deltaY = 1;
    return SOUTHWEST;
  case 3:
    deltaX = 0;
    deltaY = -1;
    return NORTH;
  case 4:
    deltaX = 0;
    deltaY = 1;
    return SOUTH;
  case 5:
    deltaX = 1;
    deltaY = -1;
    return NORTHEAST;
  case 6:
    deltaX = 1;
    deltaY = 0;
    return EAST;
  case 7:
    deltaX = 1;
    deltaY = 1;
    return SOUTHEAST;;
  default:
    return STOP; // should never happen
  }
}

void Floor::randomLocation(int &x, int &y, int &bannedChamber) {
  int temp = bannedChamber; // we'll want to change bannedChamber as each potential candidate comes
  do {
    int chamber = rand() % 5;
    bannedChamber = chamber;
    if(chamber == temp) {
      continue;
    }
    int p; // for future use
    switch(chamber) {
    case 0: // top-left chamber
      x = rand() % 26 + 3; // 3 to 28
      y = rand() % 4 + 3; // 3 to 6
      break;
    case 1: // bottom-left chamber
      x = rand() % 21 + 4; // 4 to 24
      y = rand() % 7 + 15; // 15 to 21
      break;
    case 2: // central chamber
      x = rand() % 12 + 38; // 38 to 49
      y = rand() % 3 + 10; //10 to 12
      break;
    case 3: // top-right chamber
      /* Dividing the chamber into rectangular sections */
      p = rand() % 201; // total volume
      if(p <= 92-1) { //x: 39 to 61, y: 3 to 6, volume 92
	x = rand() % 23 + 39;
	y = rand() % 4 + 3;
      } else if (p <= 92+16-1) { //x: 62 to 69, y: 5 to 6, volume 16
	x = rand() % 8 + 62;
	y = rand() % 2 + 5;
      } else if (p <= 92+16+3-1) { //x: 70 to 72, y: 6, volume 3
	x = rand() % 3 + 70;
	y = 6;
      }
      else { //x: 61 to 75, y: 7 to 12, volume 90
	x = rand() % 15 + 61;
	y = rand() % 6 + 7;
      }
      break;
    case 4:
      p = rand() % 143;
      if(p <= 107-1) { // x: 37 to 75, y: 19 to 21, volume 107
	x = rand() % 39 + 37;
	y = rand() % 3 + 19;
      } else { // x: 64 to 75, y: 16 to 18, volume 36
	x = rand() % 12 + 64;
	y = rand() % 3 + 16;
      }
      break;
    }
  } while(entities[y][x] == NULL || entities[y][x]->getSymbol() != '.');
}

void Floor::randomLocation(int &x, int &y) { // when you don't need to keep the chamber number
  int throwaway = -1;
  randomLocation(x, y, throwaway);
}
