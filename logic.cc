#include "floor.h"
#include "entity.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <cstring>
using namespace std;

static const int NUM_FLOORS = 10;
static string fileName;
static int curDepth = 0;
static Character *player = NULL;


int rollDice(int numDice, int numSides) {
  int retVal = 0;
  for(int i = 0; i < numDice; i++) {
    retVal += rand();
  }
  return retVal;
}

void deleteGame() {
  if(Floor::curFloor != NULL) {
    delete Floor::curFloor; // player should be deleted alongside
  }
  Floor::curFloor = NULL;
}

void trim() {
  char c;
  while(cin >> c) {
    if(!isspace(c)) {
      cin.unget();
      return;
    }
  }
}

void loadFloor(istream &in) {
  if(Floor::curFloor != NULL) {
    Floor::curFloor->entityAt(player->getX(), player->getY())->unoccupy(player);
    // so that player doesn't get deleted with the floor
    delete Floor::curFloor;
  }

  if(fileName == "default") {
    Floor::curFloor = new Floor(player);
  } else {
    Floor::curFloor = new Floor(player, in);
  }
}

void initialize(istream &in) {
  Potion::initialize();
  Traits::merchantHostility = false;
  curDepth = 0;

  cout << "Please choose a race:" << endl;
  char c;
  while(cin >> c) {
    switch(c) {
    case 'h':
      player = new Character(0, 0, HUMAN, true);
      loadFloor(in);
      Floor::curFloor->write(cout);
      return;
    case 'e':
      player = new Character(0, 0, ELF, true);
      loadFloor(in);
      Floor::curFloor->write(cout);
      return;
    case 'd':
      player = new Character(0, 0, DWARF, true);
      loadFloor(in);
      Floor::curFloor->write(cout);
      return;
    case 'o':
      player = new Character(0, 0, ORC, true);
      loadFloor(in);
      Floor::curFloor->write(cout);
      return;
    case 'q':
      exit(0);
    default:
      cerr << "Unrecognized race" << endl;
    }
  }
}

Direction readDirection() {
  char c, d;
  trim();
  cin >> c >> d;
  switch(c) {
  case 'n':
    switch(d) {
    case 'o':
      return NORTH;
    case 'e':
      return NORTHEAST;
    case 'w':
      return NORTHWEST;
    default:
      cerr << "Non-directional input" << endl;
      return STOP;
    }
  case 's':
    switch (d) {
    case 'o':
      return SOUTH;
    case 'e':
      return SOUTHEAST;
    case 'w':
      return SOUTHWEST;
    default:
      cerr << "Non-directional input" << endl;
      return STOP;
    }
  case 'w':
    switch(d) {
    case 'e':
      return WEST;
    default:
      cerr << "Non-directional input" << endl;
      return STOP;
    }
  case 'e':
    switch(d) {
    case 'a':
      return EAST;
    default:
      cerr << "Non-directional input" << endl;
      return STOP;
    }
  default:
    cerr << "Non-directional input" << endl;
    return STOP;
  }
}

string raceToString(Races race) { // utility
  switch(race) {
  case HUMAN:
    return "Human";
  case ELF:
    return "Elf";
  case DWARF:
    return "Dwarf";
  case ORC:
    return "Orc";
  default:
    cerr << "This race shouldn't need to be printed." << endl;
    return "";
  }
}

void gameLoop(istream &in) {
  char c;
  while(cin >> c) {
    switch(c) {
    case 'n':
    case 'w':
    case 's':
    case 'e':
      cin.unget();
      player->move(readDirection());
      break;
    case 'u':
      player->use(readDirection());
      break;
    case 'a':
      player->attack(readDirection());
      break;
    case 'r':
      return;
    case 'q':
      deleteGame();
      exit(0);
    default:
      string str;
      getline(cin, str);
      cerr << "Unrecognized command: " << c << str << endl;
    }

    if(Floor::curFloor->wentDown()) {
      if(curDepth == 9) {
	cout << "You've beaten the game with " << player->getScore() << " points! " << endl;
	return;
      } else {
	curDepth++;
	loadFloor(in);
	player->resetExtrinsicTraits();
      }
    }

    Floor::curFloor->tick(player);
    
    cout << "Race: " << raceToString(player->getRace()) << "  Gold: " << player->getGold()
	 << "            Floor " << curDepth + 1 << endl;
    cout << "HP: " << player->getHP() << endl;
    cout << "Atk: " << player->curTraits()->atkPower() << endl;
    cout << "Def: " << player->curTraits()->defPower() << endl;
    cout << "Action: " << Floor::curFloor->log.str() << endl;
    Floor::curFloor->log.str("");
    
    if(!player->curTraits()->isAlive()) {
      cout << "You've died after accumulating " << player->getScore() << " points! " << endl;
      return;
    }
  }
}
  
int main(int argc, char **argv) {
  ifstream in;
  if (argc == 2) {
    fileName = argv[1]; //converting c string to c++ string
    in.open(argv[1], ifstream::in);
    if(!in.is_open()) {
      cerr << "Unable to open " << fileName << endl;
      return 2;
    }
  } else if (argc == 1) {
    fileName = "default";
  } else {
    cerr << "There should be 0 or 1 command line arguments." << endl;
    return 1;
  }

  while(true) {
    initialize(in);  
    gameLoop(in);
    deleteGame();
    if(!(fileName == "default")) { //reopen the file from the start
      in.close();
      in.open(argv[1], ifstream::in);
    }
  }

  in.close();
}
