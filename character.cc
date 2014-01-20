#include "entity.h"
#include "floor.h"
#include <list>
#include <map>
#include <algorithm>
#include <cmath>
#include <cstdlib>
using namespace std;

Character::Character(int x, int y, Races race, bool player): Entity(x, y),
							     race(race),
							     gold(0),
							     player(player) {
  traits = racialTraits();
  hp = traits->maxHP();
}

Character *Character::randomEnemy() {
  int r = rand() % 18;
  if(r <= 4-1) {
    return new Character(0, 0, WEREWOLF, false);
  } else if(r <= 4+3-1) {
    return new Character(0, 0, VAMPIRE, false);
  } else if(r <= 4+3+5-1) {
    return new Character(0, 0, GOBLIN, false);
  } else if(r <= 4+3+5+2-1) {
    return new Character(0, 0, TROLL, false);
  } else if(r <= 4+3+5+2+2-1) {
    return new Character(0, 0, PHOENIX, false);
  } else {
    return new Character(0, 0, MERCHANT, false);
  }
}

Character::~Character() {
  delete traits;
}

bool Character::isPlayer() {
  return player;
}

char Character::getSymbol() {
  if(player) {
    return '@';
  } else {
    switch(race) {
    case VAMPIRE:
      return 'V';
    case WEREWOLF:
      return 'W';
    case GOBLIN:
      return 'N';
    case MERCHANT:
      return 'M';
    case DRAGON:
      return 'D';
    case PHOENIX:
      return 'X';
    case TROLL:
      return 'T';
    default:
      cerr << "This race should not be an enemy!" << endl;
      return '?';
    }
  }
}

Races Character::getRace() {
  return race;
}

int Character::getHP() {
  return hp;
}

list<Entity*> Character::getItems() {
  return items;
}

void Character::heal(int power) {
  hp = min(hp+power, racialTraits()->maxHP());
}

void Character::damage(int power) {
  hp = max(0, hp-power);
  if(hp <= 0) {
    traits->die();
  }
}

void Character::roam() { // roaming for NPCs
  int dir = rand() % 9;
  switch(dir) {
  case 0:
    move(NORTH);
    return;
  case 1:
    move(SOUTH);
    return;
  case 2:
    move(WEST);
    return;
  case 3:
    move(EAST);
    return;
  case 4:
    move(NORTHWEST);
    return;
  case 5:
    move(NORTHEAST);
    return;
  case 6:
    move(SOUTHWEST);
    return;
  case 7:
    move(SOUTHEAST);
    return;
  case 8:
    move(STOP);
    return;
  }
}

void Character::act() {
  if(!player) {
    roam();
    attackSurrounding();
  }
}

void Character::move(Direction dir) {
  switch(dir) {
  case NORTH:
    Floor::curFloor->entityAt(x,y-1)->occupy(this, dir);
    return;
  case WEST:
    Floor::curFloor->entityAt(x-1,y)->occupy(this, dir);
    return;
  case SOUTH:
    Floor::curFloor->entityAt(x,y+1)->occupy(this, dir);
    return;
  case EAST:
    Floor::curFloor->entityAt(x+1,y)->occupy(this, dir);
    return;
  case NORTHWEST:
    Floor::curFloor->entityAt(x-1,y-1)->occupy(this, dir);
    return;
 case NORTHEAST:
   Floor::curFloor->entityAt(x+1,y-1)->occupy(this, dir);
   return;
  case SOUTHWEST:
    Floor::curFloor->entityAt(x-1,y+1)->occupy(this, dir);
    return;
  case SOUTHEAST:
    Floor::curFloor->entityAt(x+1,y+1)->occupy(this, dir);
    return;
  case STOP:
    return;
  }
}

void Character::attack(Character *target) {
  if( abs(target->getX() - this->getX()) <= 1
      && abs(target->getY() - this->getY()) <= 1 )  { // is adjacent
    target->defend(this);
  }
}

void Character::attack(Direction dir) {
  switch(dir) {
  case NORTH:
    Floor::curFloor->entityAt(x,y-1)->defend(this);
    return;
  case WEST:
    Floor::curFloor->entityAt(x-1,y)->defend(this);
    return;
  case SOUTH:
    Floor::curFloor->entityAt(x,y+1)->defend(this);
    return;
  case EAST:
    Floor::curFloor->entityAt(x+1,y)->defend(this);
    return;
  case NORTHWEST:
    Floor::curFloor->entityAt(x-1,y-1)->defend(this);
    return;
  case NORTHEAST:
    Floor::curFloor->entityAt(x-1,y+1)->defend(this);
    return;
  case SOUTHWEST:
    Floor::curFloor->entityAt(x-1,y+1)->defend(this);
    return;
  case SOUTHEAST:
    Floor::curFloor->entityAt(x+1,y+1)->defend(this);
    return;
  case STOP:
    return;
  }
}

void Character::attackSurrounding() { // attack every adjacent space
  Floor::curFloor->entityAt(x,y-1)->defend(this);
  Floor::curFloor->entityAt(x-1,y)->defend(this);
  Floor::curFloor->entityAt(x,y+1)->defend(this);
  Floor::curFloor->entityAt(x+1,y)->defend(this);
  Floor::curFloor->entityAt(x-1,y-1)->defend(this);
  Floor::curFloor->entityAt(x-1,y+1)->defend(this);
  Floor::curFloor->entityAt(x-1,y+1)->defend(this);
  Floor::curFloor->entityAt(x+1,y+1)->defend(this);
}

void Character::defend(Character *attacker) {
  Faction atkFaction = attacker->curTraits()->faction();
  Faction defFaction = curTraits()->faction();

  if(attacker->isPlayer() || atkFaction == MINDLESS ||
     (atkFaction == FRIENDLY && defFaction == HOSTILE) ||
     (atkFaction == HOSTILE && defFaction == FRIENDLY)) {
    
    if(race == MERCHANT && attacker->isPlayer()) {
      Traits::merchantHostility = true;
      setTraits(new Hostility(traits));
    }

    int hit = rand()%100;
    int dmg = 0;
    if(hit < attacker->curTraits()->hitChance()) {
      dmg = ceil((100.0 / (100.0 + curTraits()->defPower())) * attacker->curTraits()->atkPower());
      damage(dmg);
    } else { // a miss
      dmg = -1;
    }

    if(isPlayer()) { // player is being attacked
      if(dmg == -1) { // a miss
	Floor::curFloor->log << attacker->getSymbol() << " whiffs against the PC. ";
      } else {
	Floor::curFloor->log << attacker->getSymbol() << " attacks PC for " << dmg << " damage! ";
      }
    } else if(attacker->isPlayer()) {
      if(dmg == -1) {
	Floor::curFloor->log << "PC whiffs against " << getSymbol() << ". ";
      } else {
	Floor::curFloor->log << "PC attacks " << getSymbol() << " for " << dmg << " damage! ";
      }

      if(!traits->isAlive() && !isPlayer()) {
	Floor::curFloor->log << "PC has killed " << getSymbol() << "! ";
	Floor::curFloor->remove(this);
      }
    }
  } // otherwise the attack never should have happpened
  //peace among allies!
}

void Character::use(Direction dir) {
  switch(dir) {
  case NORTH:
    Floor::curFloor->entityAt(x,y-1)->use(this);
    return;
  case WEST:
    Floor::curFloor->entityAt(x-1,y)->use(this);
    return;
  case SOUTH:
    Floor::curFloor->entityAt(x,y+1)->use(this);
    return;
  case EAST:
    Floor::curFloor->entityAt(x+1,y)->use(this);
    return;
  case NORTHWEST:
    Floor::curFloor->entityAt(x-1,y-1)->use(this);
    return;
  case NORTHEAST:
    Floor::curFloor->entityAt(x+1,y+1)->use(this);
    return;
  case SOUTHWEST:
    Floor::curFloor->entityAt(x-1,y+1)->use(this);
    return;
  case SOUTHEAST:
    Floor::curFloor->entityAt(x+1,y+1)->use(this);
    return;
  case STOP:
    return;
  }
}

void Character::collectGold(Gold *g) {
  gold += g->getValue();
  Floor::curFloor->log << "PC has collected " << g->getValue() << " gold. ";
}

int Character::getGold() {
  return gold;
}

int Character::getScore() {
  if(race == HUMAN) {
    return gold * 1.5;
  } else {
    return gold;
  }
}

void Character::setHostile() {
  traits = new Hostility(traits);
}

Traits* Character::racialTraits() {
  return Traits::racialTraits(race);
}
 
Traits* Character::curTraits() {
  return traits;
}

void Character::setTraits(Traits *traits) {
  this->traits = traits;
}

 void Character::resetExtrinsicTraits() {
  delete traits;
  traits = racialTraits();
}
