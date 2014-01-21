#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "enums.h"
#include "floor.h"
#include "traits.h"
#include <list>
#include <map>

class Traits; //forward declarations
class Floor;
class Gold;

class Entity {
protected:
  int x, y;
public:
  Entity(); // for when location doesn't matter
  Entity(int x, int y);
  virtual char getSymbol() = 0;
  virtual int getX();
  virtual int getY();
  virtual void setX(int);
  virtual void setY(int);
  virtual void move(int deltaX, int deltaY);
  virtual void act() {}
  virtual bool isEmpty(); // does it have the potential to hold things?
  virtual void occupy(Entity *object) {} // for gold/potions on the ground
  virtual void occupy(Character *mover, Direction fromDir) {} // for characters walking in
  virtual void unoccupy(Entity *occupier) {}
  virtual bool use(Character *user) {return false;} // indicate successful use/take
  virtual bool take(Character *taker) {return false;}
  virtual void defend(Character *attacker) {}
  virtual void alertAdj(Character *adjChar) {} // for dragon hoards, potions
  virtual ~Entity() {}
};

class Nothing: public Entity {
  char getSymbol();
};

class Ground : public Entity {
  Entity *occupier;
public:
  Ground(int x, int y);
  ~Ground();
  char getSymbol();
  bool isEmpty();
  void occupy(Entity *object);
  void occupy(Character*, Direction);
  void unoccupy(Entity*);
  void act();
  void defend(Character*);
  bool use(Character*);
  void alertAdj(Character*);
};

class Wall : public Entity {
  bool orientation;
public:
  Wall(int x, int y, bool orientation); // false is horizontal, true is vertical
  char getSymbol();
};

class Stairs : public Entity {
public:
  Stairs(int x, int y);
  char getSymbol();
  void occupy(Character*, Direction);
};

class Door : public Entity {
  Entity *occupier;
public:
  Door(int x, int y);
  ~Door();
  char getSymbol();
  void occupy(Character*, Direction);
  void unoccupy(Entity*);
  void act();
  void defend(Character*);
};

class Passage : public Entity {
  Entity *occupier;
public:
  Passage(int x, int y);
  ~Passage();
  char getSymbol();
  void occupy(Character*, Direction);
  void unoccupy(Entity*);
  void act();
  void defend(Character*);
};

class Character : public Entity {
public:
  Character(int x, int y, Races race, bool player);
  static Character *randomEnemy();
  ~Character();
  bool isPlayer();
  char getSymbol();
  Races getRace();
  int getHP();
  int getScore();
  int getGold();
  std::list<Entity*> getItems();
  void heal(int power);
  void damage(int power);
  void roam();
  void act();
  void move(Direction);
  void attack(Character *target);
  void attack(Direction);
  void attackSurrounding();
  void defend(Character *attacker);
  void use(Direction);
  void collectGold(Gold*);
  void setHostile();
  Traits* racialTraits();
  Traits* curTraits();
  void setTraits(Traits*);
  void resetExtrinsicTraits();

private:
  Races race;
  Traits* traits;
  int hp;
  int gold;
  bool player;
  std::list<Entity*> items;
};

class Gold : public Entity {
public:
  Gold(int x, int y, int value);
  char getSymbol();
  static Gold* generateRandom(int x, int y);
  int getValue();
  Character *getOwner();
  void setOwner(Character *owner);
  void alertAdj(Character *mover);
private:
  Character *owner;
  int value;
};

class Potion : public Entity {
public:
  static std::map<PotionType, bool> revealed;
  static void initialize();
  Potion(int x, int y, PotionType type);
  char getSymbol();
  static Potion* generateRandom(int x, int y);
  static void revealType(PotionType);
  bool isRevealed();
  void alertAdj(Character*);
  bool use(Character*);

private:
  PotionType type;
};

#endif
