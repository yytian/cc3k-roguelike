#include "enums.h"
#include "traits.h"
#include "entity.h"
#include <iostream>
#include <algorithm>
using namespace std;

Traits::Traits(Races race, int hp, int atk, int def):
  race(race), hp(hp), atk(atk), def(def), alive(true) {}

bool Traits::merchantHostility = false;

Traits* Traits::racialTraits(Races race) {
  switch(race) {
  case HUMAN:
    return new Traits(HUMAN, 140, 20, 20);
  case ELF:
    return new Traits(ELF, 140, 30, 10);
  case DWARF:
    return new Traits(DWARF, 100, 20, 30);
  case ORC:
    return new Traits(ORC, 180, 30, 25);
  case VAMPIRE:
    return new EnemyTraits(new Traits(VAMPIRE, 180, 30, 25));
  case WEREWOLF:
    return new EnemyTraits(new Traits(WEREWOLF, 120, 30, 5));
  case TROLL:
    return new EnemyTraits(new Traits(TROLL, 120, 25, 15));
  case GOBLIN:
    return new EnemyTraits(new Traits(GOBLIN, 70, 5, 10));
  case MERCHANT:
    if(merchantHostility) {
      return new EnemyTraits(new Traits(MERCHANT, 30, 70, 5));
    } else {
      return new Traits(MERCHANT, 30, 70, 5);
    }
  case DRAGON:
    return new Traits(DRAGON, 150, 20, 20);
  case PHOENIX:
    return new EnemyTraits(new Traits(PHOENIX, 50, 35, 20)); 
  default:
    cerr << "Unrecognized race: " << race;
    return NULL;
  }
}

Races Traits::getRace() {
  return race;
}

int Traits::maxHP() {
  return hp;
}

int Traits::atkPower() {
  return atk;
}

int Traits::defPower() {
  return def;
}

int Traits::hitChance() {
  return 100;
}

bool Traits::isAlive() {
  return alive;
}

Faction Traits::faction() {
  return FRIENDLY;  // default
}

void Traits::die() {
  alive = false;
}

int Traits::goldDrop() {
  if(race == DRAGON || race == MERCHANT) {
    return 0;
  } else {
    return 1;
  }
}

Entity* Traits::itemDrop(int x, int y) {
  if(race == MERCHANT) {
    return new Gold(x, y, 4);
  } else {
    return NULL;
  }
}

EnemyTraits::EnemyTraits(Traits* prevTraits): Extension(prevTraits) {}

int EnemyTraits::hitChance() {
   return 50;
}

Faction EnemyTraits::faction() {
   return HOSTILE;
}

Hostility::Hostility(Traits* prevTraits): Extension(new EnemyTraits(prevTraits)) {}

Faction Hostility::faction() {
  return HOSTILE;
}

ModStat::ModStat(Traits *traits, Statistics stat, int power): Extension(traits), stat(stat), power(power) {}

int ModStat::maxHP() {
  if(stat == HP) {
    return max(1, prevTraits->maxHP() + power);
  }
  else {
    return prevTraits->maxHP();
  }
}

int ModStat::atkPower() {
  if(stat == ATK) {
    return max(0, prevTraits->atkPower() + power);
  }
  else {
    return prevTraits->atkPower();
  }
}

int ModStat::defPower() {
  if(stat == DEF) {
    return max(0, prevTraits->defPower() + power);
  }
  else {
    return prevTraits->defPower();
  }
}

HoardGuard::HoardGuard(Traits *traits, Gold *hoard): Extension(traits), hoard(hoard) {}

void HoardGuard::die() {
  prevTraits->die();
  hoard->setOwner(NULL); // THE HOARD HAS BEEN FREED
}
