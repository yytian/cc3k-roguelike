#include "enums.h"
#include "entity.h"
#include "floor.h"
#include "traits.h"
#include <cstdlib>
using namespace std;

Entity::Entity() {}
Entity::Entity(int x, int y): x(x), y(y) {}
int Entity::getX() { return x; }
int Entity::getY() { return y; }
void Entity::setX(int x) {
  this->x = x;
}
void Entity::setY(int y) {
  this->y = y;
}

void Entity::move(int deltaX, int deltaY) {
  x += deltaX;
  y += deltaY;
}

bool Entity::isEmpty() {
  return false;
}

char Nothing::getSymbol() {
  return ' ';
}

Ground::Ground(int x, int y): Entity(x, y), occupier(NULL) {}
Ground::~Ground() { if(occupier != NULL) delete occupier; }

char Ground::getSymbol() {
  if(occupier == NULL) {
    return '.';
  } else {
    return occupier->getSymbol();
  }
}

Wall::Wall(int x, int y, bool orientation): Entity(x, y), orientation(orientation) {}

char Wall::getSymbol() {
  if(orientation) {
    return '|';
  } else {
    return '-';
  }
}

Stairs::Stairs(int x, int y): Entity(x, y) {}

char Stairs::getSymbol() {
  return '>';
}

Door::Door(int x, int y): Entity(x, y), occupier(NULL) {}
Door::~Door() { if(occupier != NULL) delete occupier; }

char Door::getSymbol() {
  if(occupier == NULL) {
    return '+';
  } else {
    return occupier->getSymbol();
  }
}

Passage::Passage(int x, int y): Entity(x, y), occupier(NULL) {}
Passage::~Passage() { if(occupier != NULL) delete occupier; }

char Passage::getSymbol() {
  if(occupier == NULL) {
    return '#';
  } else {
    return occupier->getSymbol();
  }

}

char Gold::getSymbol() {
  return 'G';
}

char Potion::getSymbol() {
  return 'P';
}

bool Ground::isEmpty() {
  return occupier == NULL;
}

void Ground::occupy(Entity *object) {
  if(occupier == NULL) {
    occupier = object;
  }
}

void moveDir(Entity *mover, Direction dir) { //utility function
  switch(dir) {
  case NORTH:
    mover->move(0,-1);
    break;
  case SOUTH:
    mover->move(0,1);
    break;
  case EAST:
    mover->move(1,0);
    break;
  case WEST:
    mover->move(-1,0);
    break;
  case NORTHWEST:
    mover->move(-1,-1);
    break;
  case NORTHEAST:
    mover->move(1,-1);
    break;
  case SOUTHWEST:
    mover->move(-1,1);
    break;
  case SOUTHEAST:
    mover->move(1,1);
    break;
  case STOP:
    break;
  }
}

void Ground::occupy(Character *mover, Direction dir) {
  if(occupier == NULL) {
    Entity *prev = Floor::curFloor->entityAt(mover->getX(), mover->getY()); // find the square the character currently occupies
    prev->unoccupy(mover); //and vacate it
    occupier = mover;
    moveDir(mover, dir);
  } else if(occupier->getSymbol() == 'G') { //collect gold
    Gold *gold = (Gold*) occupier ; //ugly
    if(gold->getOwner() == NULL && mover->isPlayer()) {
      mover->collectGold(gold);
      unoccupy(gold);
      delete gold; // that gold has been collected
      Entity *prev = Floor::curFloor->entityAt(mover->getX(), mover->getY()); // find the square the character currently occupies
      prev->unoccupy(mover); //and vacate it
      occupier = mover;
      moveDir(mover, dir);
    }
  } else {} // can't occupy the same square!
}

void Ground::unoccupy(Entity *occupier) {
  if(this->occupier == occupier) {
    this->occupier = NULL;
  } else {
    cerr << "Attempt to move off unoccupied ground";
  }
}

void Ground::act() {
  if(occupier != NULL) {
    occupier->act();
  }
}

void Ground::defend(Character *attacker) {
  if(occupier != NULL) {
    occupier->defend(attacker);
  }
}

bool Ground::use(Character *user) {
  if(occupier != NULL) {
    bool status = occupier->use(user);
    if(status) { //successful use
      delete occupier;
      occupier = NULL;
    }
    return status;
  }
  return false;
}

void Ground::alertAdj(Character *adjChar) {
  if(occupier != NULL) {
    occupier->alertAdj(adjChar);
  }
}

void Passage::occupy(Character *mover, Direction dir) { // same as for ground, without special case of gold
  if(occupier == NULL) {
    Entity *prev = Floor::curFloor->entityAt(mover->getX(), mover->getY()); // find the square the character currently occupies
      prev->unoccupy(mover); //and vacate it
    occupier = mover;
    moveDir(mover, dir);
  } else {} // can't occupy the same square!
}

void Passage::unoccupy(Entity *occupier) {
  if(this->occupier == occupier) {
    this->occupier = NULL;
  } else {
    cerr << "Attempt to move off unoccupied passage";
  }
}

void Passage::act() {
  if(occupier != NULL) {
    occupier->act();
  }
}

void Passage::defend(Character *attacker) {
  if(occupier != NULL) {
    occupier->defend(attacker);
  }
}

void Door::occupy(Character *mover, Direction dir) { // same as for passage
  if(occupier == NULL) {
    Entity *prev = Floor::curFloor->entityAt(mover->getX(), mover->getY()); // find the square the character currently occupies
      prev->unoccupy(mover); //and vacate it
    occupier = mover;
    moveDir(mover, dir);
  } else {} // can't occupy the same square!
}

void Door::unoccupy(Entity *occupier) {
  if(this->occupier == occupier) {
    this->occupier = NULL;
  } else {
    cerr << "Attempt to move off unoccupied ground";
  }
}

void Door::act() {
  if(occupier != NULL) {
    occupier->act();
  }
}

void Door::defend(Character *attacker) {
  if(occupier != NULL) {
    occupier->defend(attacker);
  }
}

void Stairs::occupy(Character *occupier, Direction dir) {
  if(occupier->isPlayer()) {
    Floor::curFloor->goDown();
  }
}

Gold::Gold(int x, int y, int value): Entity(x, y), owner(NULL), value(value) {}

Gold* Gold::generateRandom(int x, int y) {
  int r = rand() % 8;
  if(r <= 5-1) {
    return new Gold(x, y, 1); //normal
  } else if(r <= 5+2-1) {
    return new Gold(x, y, 2); //small hoard
  } else {
    return new Gold(x, y, 6); //dragon hoard
  }
}

int Gold::getValue() {
  return value;
}

Character *Gold::getOwner() {
  return owner;
}

void Gold::setOwner(Character *owner) {
  this->owner = owner;
}

void Gold::alertAdj(Character *mover) {
  if(owner != NULL) {
    owner->setHostile();
  }
}


map<PotionType, bool> Potion::revealed;

void Potion::initialize() {
  Potion::revealed[RH] = false;
  Potion::revealed[BA] = false;
  Potion::revealed[BD] = false;
  Potion::revealed[PH] = false;
  Potion::revealed[WA] = false;
  Potion::revealed[WD] = false;
}

Potion::Potion(int x, int y, PotionType type): Entity(x, y), type(type) {}

Potion* Potion::generateRandom(int x, int y) {
  int r = rand() % 6;
  switch(r) {
  case 0:
    return new Potion(x, y, RH);
  case 1:
    return new Potion(x, y, BA);
  case 2:
    return new Potion(x, y, BD);
  case 3:
    return new Potion(x, y, PH);
  case 4:
    return new Potion(x, y, WA);
  case 5:
    return new Potion(x, y, WD);
  default:
    return NULL; // g++ complains if this isn't here
  }
}

void Potion::revealType(PotionType toReveal) {
  revealed[toReveal] = true;
}

bool Potion::isRevealed() {
  return revealed[type];
}

string potionName(PotionType type) { // utility
  switch(type) {
  case RH:
    return "restore health";
  case BA:
    return "boost attack";
  case BD:
    return "boost defense";
  case PH:
    return "poison";
  case WA:
    return "wound attack";
  case WD:
    return "wound defense";
  default:
    return "unknown type!!";
  }
}

void Potion::alertAdj(Character *adjChar) {
  if(isRevealed()) {
    Floor::curFloor->log << "PC sees a potion of " << potionName(type) << " ";
  } else {
    Floor::curFloor->log << "PC sees an unknown potion. ";
  }
}

bool Potion::use(Character *user) {
  Floor::curFloor->log << "PC uses a potion of " << potionName(type) << " ! ";
  Potion::revealType(type);
  switch(type) {
  case RH:
    user->heal(10);
    return true;
  case BA:
    user->setTraits(new ModStat(user->curTraits(), ATK, 5));
    return true;
  case BD:
    user->setTraits(new ModStat(user->curTraits(), DEF, 5));
    return true;
  case PH:
    user->damage(10);
    return true;
  case WA:
    user->setTraits(new ModStat(user->curTraits(), ATK, -5));
    return true;
  case WD:
    user->setTraits(new ModStat(user->curTraits(), DEF, -5));
   return true;
  default:
    return false;
  }
}
