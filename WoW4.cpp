#include <iomanip>
#include <iostream>
#include <vector>
using namespace std;

typedef int Type;
typedef int Flag;

constexpr Type DRAGON{0}, NINJA{1}, ICEMAN{2}, LION{3}, WOLF{4};
constexpr Flag RED{0}, BLUE{1}, NONE{2};

constexpr char WARRIOR_NAME[5][8]{"dragon", "ninja", "iceman", "lion", "wolf"};
constexpr char HEAD_NAME[2][8]{"red", "blue"};

int globalMinute{};
void showTime()
{
    cout << setw(3) << setfill('0') << globalMinute / 60 // 小时
         << ':'
         << setw(2) << setfill('0') << globalMinute % 60 << ' '; // 分钟
}

class City;
class Command;

class BasicWarrior
{
private:
    int mID;
    Command *mCommand;
    Flag mFlag;
    Type mType;
    int mElement, mForce;

public:
    BasicWarrior(int id, Command *head, Type type, int element, int force);
    virtual ~BasicWarrior() {}
    virtual void yell() {} // For Dragon
    void useBomb(BasicWarrior *enemy)
    {
    }
    void useArrow(City *nextCity)
    {
    }
    void attack(BasicWarrior *enemy)
    {
        showTime();
        cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID
             << " attacked "
             << HEAD_NAME[enemy->mFlag] << ' ' << WARRIOR_NAME[enemy->mType] << ' ' << enemy->mID
             << " in city " // << /* TODO: City ID*/
             << " with " << mElement << " elements and force " << mForce;
        enemy->mElement -= mForce;
        // TODO: Sword
    }
    virtual void foughtBack(BasicWarrior *enemy)
    {
    }
    void sendElement(int element);
    virtual void beforeMove() {}
    bool isAlive() const { return mElement >= 0; }
};
class City
{
protected:
    int mID;
    Flag mFlag;
    int mElement;
    BasicWarrior *mWarriorPtr[2], *tmpWarriorPtr[2];
    City *mWestCity, *mEastCity;

public:
    City() : mWarriorPtr{nullptr, nullptr}, tmpWarriorPtr{nullptr, nullptr},
             mWestCity{nullptr}, mEastCity{nullptr} {}
    City(int id, City *westCity, City *eastCity)
        : mID{id}, mFlag{NONE}, mElement{0},
          mWarriorPtr{nullptr, nullptr}, tmpWarriorPtr{nullptr, nullptr},
          mWestCity{westCity}, mEastCity{eastCity} {}
    void letWarriorGetElement()
    {
        if (mWarriorPtr[RED] && mWarriorPtr[BLUE] == nullptr)
        {
            mWarriorPtr[RED]->sendElement(mElement);
            mElement = 0;
        }
        else if (mWarriorPtr[RED] == nullptr && mWarriorPtr[BLUE])
        {
            mWarriorPtr[BLUE]->sendElement(mElement);
            mElement = 0;
        }
    }
};
class Command : public City
{
private:
    vector<BasicWarrior *> warriorList;

public:
    Command() {}
    void generate() {}
    Flag getHead() const { return mFlag; }
    void addElement(int element) { mElement += element; }
};

int main()
{

    return 0;
}

BasicWarrior::BasicWarrior(int id, Command *head, Type type, int element, int force)
    : mID{id},
      mCommand{head}, mFlag{mCommand->getHead()},
      mType{type}, mElement{element}, mForce{force} {}

void BasicWarrior::sendElement(int element) { mCommand->addElement(element); }
