#ifndef __TRAITS_H__
#define __TRAITS_H__

#include "enums.h"
#include "entity.h"

class Entity; // forward declarations
class Character;
class Gold;

class Traits {
  Races race;
  int hp, atk, def;
  bool alive;

public:
  Traits() {}
  Traits(Races race, int hp, int atk, int def);
  virtual ~Traits() {}
  static Traits* racialTraits(Races race);
  static bool merchantHostility;
  Races getRace();
  virtual int maxHP();
  virtual int atkPower();
  virtual int defPower();
  virtual int hitChance();
  virtual bool isAlive();
  virtual Faction faction();
  virtual int goldDrop();
  virtual Entity* itemDrop(int x, int y);
  virtual void die(); // maybe some races can't die!
};

class Extension : public Traits {
protected:
  Traits *prevTraits;
public:
  Extension(Traits* traits): prevTraits(traits) {}
  ~Extension() { delete prevTraits; }
  int maxHP() { return prevTraits->maxHP(); }
  int atkPower() { return prevTraits->atkPower(); }
  int defPower() { return prevTraits->defPower(); }
  int hitChance() { return prevTraits->hitChance(); }
  bool isAlive() { return prevTraits->isAlive(); }
  Faction faction() { return prevTraits->faction(); }
  int goldDrop() { return prevTraits->goldDrop(); }
  Entity* itemDrop(int x, int y) { return prevTraits->itemDrop(x, y); }
  void die() { prevTraits->die(); }
};

class EnemyTraits : public Extension {
public:
  EnemyTraits(Traits*);
  int hitChance(); // halved for enemies
  Faction faction();
};

class Hostility : public Extension { // for when they become enraged
public:
  Hostility(Traits*);
  Faction faction();
};

class ModStat : public Extension {
  ModStat(Traits*);
  Statistics stat;
  int power;

public:
  ModStat(Traits *traits, Statistics stat, int power);
  int maxHP();
  int atkPower();
  int defPower();
};

class HoardGuard : public Extension {
  HoardGuard(Traits*);
  Gold *hoard;

public:
  HoardGuard(Traits *traits, Gold *hoard);
  void die();
};

#endif
