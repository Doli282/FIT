#include <iostream>
#include <string>

using namespace std;

class Character{
public:
    Character(const string & nick, int ad, int maxhp) : name(nick), maxHP(maxhp), AD(ad){}
    string name;
    int HP = 100;
    int maxHP = 100;
    int AD = 10;
    void virtual attack(Character & enemy, int bonus = 1){
        enemy.onHit(AD * bonus);
    }
    void virtual onHit(int dmg){
        HP -= dmg;
        cout << name << " takes " << dmg << " dmg" << endl;
    }
    bool alive(){
        return HP > 0;
    }
    friend std::ostream & operator << (std::ostream & os, const Character & ch);
};

std::ostream & operator << (std::ostream & os, const Character & ch){
    os << "Character: " << ch.name << " HP: " << ch.HP << "/" << ch.maxHP << " AD: " << ch.AD;
    return os;
}

class Warrior : public Character{
public:
    int blockChance = 0;
    int increment = 10;
    
    Warrior(const string & nick, int ad, int maxhp, int inc = 10) : Character(nick, ad, maxhp), increment(inc){}
    void  onHit(int dmg) override{ // onHitWithShield
        if(blockChance >= 100){
            blockChance = 0;
            cout << "attack blocked" << endl;
            return;
        }
        blockChance += increment;
        //cout << "blockchance " << blockChance << endl;
        Character::onHit(dmg);
    }
};

class Mage : public Character{
public:    
    int MP = 0;
    int increment = 20;
    
    Mage(const string & nick, int ad, int maxhp, int inc = 10) : Character(nick, ad, maxhp), increment(inc){
        //this.HP = maxHP;
        //cout << "Mage " << name << " prepared" << endl;
    }
    void  attack(Character & enemy, int bonus = 1) override{
        if(MP >= 100){
            MP = 0;
            Character::attack(enemy, 2);
            cout << "empowered attack" << endl;
            return;
        }
        MP += increment;
        //cout << "MP " << MP << endl;
        Character::attack(enemy);
    }
};

void duel (Character & blue, Character & red){
    cout << "blue: " << blue.name << " vs " "red: " << red.name << endl;
    while(1){
        blue.attack(red);
        if(!red.alive()){
            cout << " *** BLUE WINS *** " << endl;
            cout << "blue: " << blue << endl;
            cout << "red: " << red << endl;
            return;
        }
        red.attack(blue);
        if(!blue.alive()){
            cout << " *** RED WINS *** " << endl;
            cout << "blue: " << blue << endl;
            cout << "red: " << red << endl;
            return;
        }
    }
}

int main (){
    Warrior war("adolf", 8, 100, 20);
    Mage mag("Gandalf", 12, 80, 20);

    duel(war, mag); 
    return 0;
}