#ifndef __FLOOR_H__
#define __FLOOR_H__
#include "entity.h"
#include "enums.h"
#include <iostream>
#include <sstream>

class Character; // forward declarations
class Entity;

class Floor {
 private:
  static Direction randomDirection(int &x, int &y);
  void randomLocation(int &x, int &y, int &bannedChamber);
  void randomLocation(int &x, int &y);
  static const int HEIGHT = 25, WIDTH = 79;
  Entity *entities[HEIGHT][WIDTH];
  Character *player;
  bool wentDownStairs;

 public:
  static Floor *curFloor;
  std::stringstream log;
  Floor(Character *player);
  Floor(Character *player, std::istream &in);
  char getSymbol(std::string name);
  Entity* entityAt(int x, int y);
  void remove(Character*);
  void tick(Character* player);
  bool wentDown();
  void goDown();
  bool gameOver();
  void read(std::istream& in, Character *player);
  void write(std::ostream &out);
  ~Floor();
};

#endif
