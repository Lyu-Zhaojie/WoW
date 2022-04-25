#include <iomanip>
#include <ios>
#include <iostream>
#include <vector>
#include <wchar.h>
using namespace std;

typedef int Type;
typedef int Flag;
typedef int weapon;

constexpr Type DRAGON{0}, NINJA{1}, ICEMAN{2}, LION{3}, WOLF{4};
constexpr Flag RED{0}, BLUE{1}, NONE{2};
constexpr Type ORDER[2][5]{{ICEMAN, LION, WOLF, NINJA, DRAGON}, {LION, DRAGON, NINJA, ICEMAN, WOLF}};
constexpr weapon SWORD{0}, BOMB{1}, ARROW{2};

constexpr char WARRIOR_NAME[5][8]{"dragon", "ninja", "iceman", "lion", "wolf"};
constexpr char HEAD_NAME[2][8]{"red", "blue"};

int element{0}, nCity{0}, loyaltyStep{0}, timeLimit{0};
int INIT_ELEMENT[5]{0}, INIT_FORCE[5]{0};
int ARROW_FORCE{};

enum class WarState
{
    NONE,
    WIN,
    DRAW
};

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
protected:
    int mID;
    Command *mCommand;
    Flag mFlag;
    Type mType;
    int mElement, mForce;
    WarState mWarState;
    class WeaponList
    {
    private:
        int mSword, mBomb, mArrow;

    public:
        WeaponList(int weapon1, int weapon2, int warriorForce);
        int getSwordForce() const { return mSword; }
        int getArrowNum() const { return mArrow; }
        int getBombNum() const { return mBomb; }
        bool hasSword() const { return mSword > 0; }
        bool hasBomb() const { return mBomb > 0; }
        bool hasArrow() const { return mArrow > 0; }
        void useSword() { mSword = mSword * 8 / 10; }
        void useArrow() { mArrow -= 1; }
        bool hasWeapon() const { return hasSword() || hasBomb() || hasArrow(); }
    } weaponList;

public:
    BasicWarrior(int id, Command *head, Type type, int element, int force, int weapon1 = -1, int weapon2 = -1);
    virtual ~BasicWarrior() {}

    int getDamage() const { return mForce + weaponList.getSwordForce(); }
    int getBackDamage() const { return mForce / 2 + weaponList.getSwordForce(); }

    virtual void yell() {} // For Dragon

    void useBomb(BasicWarrior *enemy);
    void useArrow(City *nextCity);
    void attack(BasicWarrior *enemy, int cityID);
    void beAttacked(int damage) { mElement -= damage; }
    virtual void foughtBack(BasicWarrior *enemy, int cityID);
    void addElement(int prize) { mElement += prize; }
    void setWarState(WarState state) { mWarState = state; }

    void sendElement(int element);
    virtual void beforeMove() {}
    void move(int cityIndex);
    void reportWeapon();

    bool isAlive() const { return mElement > 0; }
    virtual bool isLoyal() const { return true; }

    Type getType() const { return mType; }
    int getID() const { return mID; }
    int getElement() const { return mElement; }
};
class Dragon : public BasicWarrior
{
private:
    double mMorale;

public:
    Dragon(int id, Command *head, double morale)
        : BasicWarrior(id, head, DRAGON, INIT_ELEMENT[DRAGON], INIT_FORCE[DRAGON], id % 3),
          mMorale{morale}
    {
        cout << "Its morale is " << setiosflags(ios::fixed) << setprecision(2) << mMorale << endl;
    }
    void yell() override;
};
class Ninja : public BasicWarrior
{
public:
    Ninja(int id, Command *head)
        : BasicWarrior(id, head, NINJA, INIT_ELEMENT[NINJA], INIT_FORCE[NINJA], id % 3, (id + 1) % 3) {}
    void foughtBack(BasicWarrior *enemy, int cityID) override {}
};
class Iceman : public BasicWarrior
{
private:
    int moveTimes;

public:
    Iceman(int id, Command *head)
        : BasicWarrior(id, head, ICEMAN, INIT_ELEMENT[ICEMAN], INIT_FORCE[ICEMAN], id % 3),
          moveTimes{0} {}
    void beforeMove() override;
};
class Lion : public BasicWarrior
{
private:
    int mLoyalty;

public:
    Lion(int id, Command *head, int loyalty)
        : BasicWarrior(id, head, LION, INIT_ELEMENT[LION], INIT_FORCE[LION]),
          mLoyalty{loyalty}
    {
        cout << "Its loyalty is " << mLoyalty << endl;
    }
    bool isLoyal() const override { return mLoyalty > 0; }
};
class Wolf : public BasicWarrior
{
public:
    Wolf(int id, Command *head)
        : BasicWarrior(id, head, WOLF, INIT_ELEMENT[WOLF], INIT_FORCE[WOLF]) {}
};
class City
{
protected:
    int mID;
    BasicWarrior *mWarriorPtr[2], *tmpWarriorPtr[2];
    City *mWestCity, *mEastCity;
    int mElement;
    Flag mFlag;
    int mWinTimes[2];

public:
    City() : mWarriorPtr{nullptr, nullptr}, tmpWarriorPtr{nullptr, nullptr},
             mWestCity{nullptr}, mEastCity{nullptr} {}
    City(int id, City *westCity, City *eastCity, int element = 0, Flag flag = NONE)
        : mID{id},
          mWarriorPtr{nullptr, nullptr}, tmpWarriorPtr{nullptr, nullptr},
          mWestCity{westCity}, mEastCity{eastCity}, mElement{element}, mFlag{flag}, mWinTimes{0, 0} {}
    ~City()
    {
        for (Flag i : {RED, BLUE})
        {
            if (mWarriorPtr[i])
                delete mWarriorPtr[i];
            if (tmpWarriorPtr[i])
                delete tmpWarriorPtr[i];
        }
    }
    void letWarriorGetElement();
    virtual void lionEscape();
    void generateElement() { mElement += 10; }
    int getID() const { return mID; }
    void useArrow();
    virtual BasicWarrior *getWarriorPtr(Flag flag) { return mWarriorPtr[flag]; }
    void fight();
    virtual void setPtr();
    void report(Flag flag);
    Flag priority();
    friend void Move(City &westCity, City &eastCity);
};
class Command : public City
{
private:
    // vector<BasicWarrior *> warriorList;
    int generateIndex;
    int nOfWarrior;
    int tmpElement;

public:
    Command(Flag flag, int element, City *westCity, City *eastCity)
        : City(-1, westCity, eastCity, element, flag),
          generateIndex{0}, nOfWarrior{0}, tmpElement{0} {}
    void lionEscape() override;
    void generate();
    Flag getHead() const { return mFlag; }
    void addElement(int element) { tmpElement += element; }
    void freshElement()
    {
        mElement += tmpElement;
        tmpElement = 0;
    }
    void reportElement();
    BasicWarrior *getWarriorPtr(Flag flag) override { return nullptr; }
};

int main()
{
    int n{};
    cin >> n;
    for (int i{}; i < n; i++)
    {
        cin >> element >> nCity >> ARROW_FORCE >> loyaltyStep >> timeLimit;
        cin >> INIT_ELEMENT[0] >> INIT_ELEMENT[1] >> INIT_ELEMENT[2] >> INIT_ELEMENT[3] >> INIT_ELEMENT[4];
        cin >> INIT_FORCE[0] >> INIT_FORCE[1] >> INIT_FORCE[2] >> INIT_FORCE[3] >> INIT_FORCE[4];

        City *cityList{new City[nCity + 1]};
        Command redCommand(RED, element, nullptr, cityList + 1);
        Command blueCommand(BLUE, element, cityList + nCity, nullptr);

        cityList[1] = City(1, &redCommand, cityList + 2);
        cityList[nCity] = City(nCity, cityList + nCity - 1, &blueCommand);
        for (int j{2}; j < nCity; j++)
            cityList[j] = City(j, cityList + j - 1, cityList + j + 1);

        cout << "Case " << i + 1 << ':' << endl;
        for (globalMinute = 0; globalMinute <= timeLimit; globalMinute++)
        {
            switch (globalMinute % 60)
            {
            case 0:
                redCommand.generate();
                blueCommand.generate();
                break;
            case 5:
                redCommand.lionEscape();
                for (int j{1}; j <= nCity; j++)
                    cityList[j].lionEscape();
                break;
            case 10:
                Move(redCommand, cityList[1]);
                for (int j{1}; j < nCity; j++)
                    Move(cityList[j], cityList[j + 1]);
                Move(cityList[nCity], blueCommand);
                redCommand.setPtr();
                for (int j{1}; j <= nCity; j++)
                    cityList[j].setPtr();
                blueCommand.setPtr();
                break;
            case 20:
                for (int j{1}; j <= nCity; j++)
                    cityList[j].generateElement();
                break;
            case 30:
                for (int j{1}; j <= nCity; j++)
                    cityList[j].letWarriorGetElement();
                break;
            case 35:
                redCommand.useArrow();
                for (int j{1}; j <= nCity; j++)
                    cityList[j].useArrow();
                blueCommand.useArrow();
                break;
            case 38:
                // TODO: Bomb
                break;
            case 40:
                for (int j{1}; j <= nCity; j++)
                    cityList[j].fight();
                break;
            case 50:
                redCommand.reportElement();
                blueCommand.reportElement();
                break;
            case 55:
                redCommand.report(RED);
                for (int j{1}; j <= nCity; j++)
                    cityList[j].report(RED);
                blueCommand.report(RED);
                redCommand.report(BLUE);
                for (int j{1}; j <= nCity; j++)
                    cityList[j].report(BLUE);
                blueCommand.report(BLUE);
                break;
            default:
                // cout << globalMinute << endl;
                break;
            }
        }
        delete[] cityList;
    }
    return 0;
}

BasicWarrior::WeaponList::WeaponList(int weapon1, int weapon2, int warriorForce) : mSword{0}, mBomb{0}, mArrow{0}
{
    switch (weapon1)
    {
    case SWORD:
        mSword = warriorForce * 2 / 10;
        break;
    case BOMB:
        mBomb = 1;
        break;
    case ARROW:
        mArrow = 3;
        break;
    default:
        break;
    }
    switch (weapon2)
    {
    case SWORD:
        mSword = warriorForce * 2 / 10;
        break;
    case BOMB:
        mBomb = 1;
        break;
    case ARROW:
        mArrow = 3;
        break;
    default:
        break;
    }
}

BasicWarrior::BasicWarrior(int id, Command *head, Type type, int element, int force, int weapon1, int weapon2)
    : mID{id},
      mCommand{head}, mFlag{mCommand->getHead()},
      mType{type}, mElement{element}, mForce{force}, mWarState{WarState::NONE},
      weaponList(weapon1, weapon2, mForce)
{
    showTime();
    cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " born" << endl;
}

void BasicWarrior::move(int cityIndex)
{
    beforeMove();
    showTime();
    cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " marched to city " << cityIndex << " with " << mElement << " elements and force " << mForce << endl;
}

void BasicWarrior::useBomb(BasicWarrior *enemy)
{
    bool judge{false}; // TODO: judge whether use bomb or not.
    if (judge)
    {
        beAttacked(mElement);
        enemy->beAttacked(enemy->mElement);
        showTime();
        cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " used a bomb and killed " << HEAD_NAME[enemy->mFlag] << ' ' << WARRIOR_NAME[enemy->mType] << ' ' << enemy->mID << endl;
    }
}
void BasicWarrior::useArrow(City *nextCity)
{
    if (weaponList.hasArrow() && nextCity && nextCity->getWarriorPtr(mFlag ^ 1))
    {
        BasicWarrior *enemy{nextCity->getWarriorPtr(mFlag ^ 1)};
        enemy->beAttacked(ARROW_FORCE);
        weaponList.useArrow();
        showTime();
        if (enemy->isAlive())
            cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " shot" << endl;
        else
            cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " shot and killed " << HEAD_NAME[enemy->mFlag] << ' ' << WARRIOR_NAME[enemy->mType] << ' ' << enemy->mID << endl;
    }
}
void BasicWarrior::attack(BasicWarrior *enemy, int cityID)
{
    showTime();
    cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID
         << " attacked "
         << HEAD_NAME[enemy->mFlag] << ' ' << WARRIOR_NAME[enemy->mType] << ' ' << enemy->mID
         << " in city " << cityID
         << " with " << mElement << " elements and force " << mForce << endl;
    enemy->beAttacked(getDamage());
    if (weaponList.hasSword())
        weaponList.useSword();
}
void BasicWarrior::foughtBack(BasicWarrior *enemy, int cityID)
{
    enemy->beAttacked(getBackDamage());
    showTime();
    cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " fought back against " << HEAD_NAME[enemy->mFlag] << ' ' << WARRIOR_NAME[enemy->mType] << ' ' << enemy->mID << " in city " << cityID << endl;
}
void BasicWarrior::sendElement(int element)
{
    mCommand->addElement(element);
    showTime();
    cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " earned " << element << " elements for his headquarter" << endl;
}
void BasicWarrior::reportWeapon()
{
    showTime();
    cout << HEAD_NAME[mFlag] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " has ";
    if (!weaponList.hasWeapon())
        cout << "no weapon" << endl;
    else
    {
        bool withComma{false};
        if (weaponList.hasArrow())
        {
            if (withComma)
                cout << ',';
            cout << "arrow(" << weaponList.getArrowNum() << ')';
            withComma = true;
        }
        if (weaponList.hasBomb())
        {
            if (withComma)
                cout << ',';
            cout << "bomb";
            withComma = true;
        }
        if (weaponList.hasSword())
        {
            if (withComma)
                cout << ',';
            cout << "sword(" << weaponList.getSwordForce() << ')';
            withComma = true;
        }
        cout << endl;
    }
}
void Dragon::yell() {}

void Iceman::beforeMove()
{
    moveTimes++;
    if (moveTimes == 2)
    {
        if (mElement <= 9)
            mElement = 1;
        else
            mElement -= 9;
        mForce += 20;
        moveTimes = 0;
    }
}

void City::lionEscape()
{
    for (Flag i : {RED, BLUE})
    {
        if (mWarriorPtr[i] && mWarriorPtr[i]->getType() == LION && !mWarriorPtr[i]->isLoyal())
        {
            showTime();
            cout << HEAD_NAME[i] << " lion " << mWarriorPtr[i]->getID() << " ran away" << endl;
            delete mWarriorPtr[i];
            mWarriorPtr[i] = nullptr;
        }
    }
}
void City::letWarriorGetElement()
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

void City::useArrow()
{
    if (mWarriorPtr[RED] && mEastCity)
        mWarriorPtr[RED]->useArrow(mEastCity);
    if (mWarriorPtr[BLUE] && mWestCity)
        mWarriorPtr[BLUE]->useArrow(mWestCity);
}
void City::fight()
{
    for (Flag i : {RED, BLUE})
    {
        if (mWarriorPtr[i] && !mWarriorPtr[i]->isAlive())
        {
            delete mWarriorPtr[i];
            mWarriorPtr[i] = nullptr;
        }
    }
    if (mWarriorPtr[RED] && mWarriorPtr[BLUE])
    {
        int element[2]{mWarriorPtr[RED]->getElement(), mWarriorPtr[BLUE]->getElement()};
        Flag pri{priority()};
        mWarriorPtr[pri]->attack(mWarriorPtr[pri ^ 1], mID);
        if (mWarriorPtr[pri ^ 1]->isAlive())
            mWarriorPtr[pri ^ 1]->foughtBack(mWarriorPtr[pri], mID);

        if (mWarriorPtr[RED]->isAlive() && mWarriorPtr[BLUE]->isAlive())
        {
            mWarriorPtr[RED]->setWarState(WarState::DRAW);
            mWarriorPtr[BLUE]->setWarState(WarState::DRAW);
            mWinTimes[RED] = 0;
            mWinTimes[BLUE] = 0;
        }
        else
        {
            for (Flag i : {RED, BLUE})
            {
                if (!mWarriorPtr[i]->isAlive())
                {
                    showTime();
                    cout << HEAD_NAME[i] << ' ' << WARRIOR_NAME[mWarriorPtr[i]->getType()] << ' ' << mWarriorPtr[i]->getID() << " was killed in city " << mID << endl;
                    mWarriorPtr[i ^ 1]->setWarState(WarState::WIN);
                    if (mWarriorPtr[i]->getType() == LION)
                        mWarriorPtr[i ^ 1]->addElement(element[i]);
                    delete mWarriorPtr[i];
                    mWarriorPtr[i] = nullptr;
                    mWarriorPtr[i ^ 1]->sendElement(mElement);
                    mElement = 0;
                    mWinTimes[i] = 0;
                    mWinTimes[i ^ 1]++;
                    if (mWinTimes[i ^ 1] == 2)
                    {
                        mFlag = i ^ 1;
                        showTime();
                        cout << HEAD_NAME[mFlag] << " flag raised in city " << mID << endl;
                    }
                    break;
                }
            }
        }
    }
}
void City::setPtr()
{
    for (Flag i : {RED, BLUE})
    {
        mWarriorPtr[i] = tmpWarriorPtr[i];
        tmpWarriorPtr[i] = nullptr;
        if (mWarriorPtr[i])
            mWarriorPtr[i]->move(mID);
    }
}
void City::report(Flag flag)
{
    if (mWarriorPtr[flag])
        mWarriorPtr[flag]->reportWeapon();
}
Flag City::priority()
{
    if (mFlag != NONE)
        return mFlag;
    else
        return mID & 1 ? RED : BLUE;
}

void Command::lionEscape()
{
    if (mWarriorPtr[mFlag] && mWarriorPtr[mFlag]->getType() == LION && !mWarriorPtr[mFlag]->isLoyal())
    {
        showTime();
        cout << HEAD_NAME[mFlag] << " lion " << mWarriorPtr[mFlag]->getID() << " ran away" << endl;
        delete mWarriorPtr[mFlag];
        mWarriorPtr[mFlag] = nullptr;
    }
}
void Command::generate()
{
    int generateType{ORDER[mFlag][generateIndex]};
    if (mElement >= INIT_ELEMENT[generateType])
    {
        nOfWarrior++;
        mElement -= INIT_ELEMENT[generateType];
        switch (generateType)
        {
        case DRAGON:
            mWarriorPtr[mFlag] = new Dragon(nOfWarrior, this, static_cast<double>(mElement) / INIT_ELEMENT[DRAGON]);
            break;
        case NINJA:
            mWarriorPtr[mFlag] = new Ninja(nOfWarrior, this);
            break;
        case ICEMAN:
            mWarriorPtr[mFlag] = new Iceman(nOfWarrior, this);
            break;
        case LION:
            mWarriorPtr[mFlag] = new Lion(nOfWarrior, this, mElement);
            break;
        case WOLF:
            mWarriorPtr[mFlag] = new Wolf(nOfWarrior, this);
            break;
        }
        generateIndex = generateIndex == 4 ? 0 : generateIndex + 1;
    }
}
void Command::reportElement()
{
    showTime();
    cout << mElement << " elements in " << HEAD_NAME[mFlag] << " headquarter" << endl;
}
void Move(City &westCity, City &eastCity)
{
    eastCity.tmpWarriorPtr[RED] = westCity.mWarriorPtr[RED];
    westCity.mWarriorPtr[RED] = nullptr;
    westCity.tmpWarriorPtr[BLUE] = eastCity.mWarriorPtr[BLUE];
    eastCity.mWarriorPtr[BLUE] = nullptr;
}
