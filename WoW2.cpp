#include <iomanip>
#include <iostream>
#include <vector>

namespace Faction
{
    constexpr int RED{0}, BLUE{1};
}
namespace WarriorType
{
    /**
     * @brief 命名空间WarriorType，定义整形常量以区分武士类型
     * @details `DRAGON`, `NINJA`, `ICEMAN`, `LION`, `WOLF`
     */
    constexpr int DRAGON{0}, NINJA{1}, ICEMAN{2}, LION{3}, WOLF{4};
}
constexpr char WARRIOR_NAME[5][16]{"dragon", "ninja", "iceman", "lion", "wolf"};
constexpr int RED_GENERATE_ORDER[5]{WarriorType::ICEMAN, WarriorType::LION, WarriorType::WOLF, WarriorType::NINJA, WarriorType::DRAGON};
constexpr int BLUE_GENERATE_ORDER[5]{WarriorType::LION, WarriorType::DRAGON, WarriorType::NINJA, WarriorType::ICEMAN, WarriorType::WOLF};
int INIT_ELEMENT[5]{0};
int globalTime{0};

class BaseWarrior
{
public:
    BaseWarrior(int id, int life, int faction)
        : mId{id}, mLife{life}, mFaction{faction} {}
    virtual ~BaseWarrior() = default;

protected:
    int mId, mLife, mFaction;
};
class BaseWeapon
{
public:
    BaseWeapon() {}
    virtual ~BaseWeapon() = default;
};
class Command
{
public:
    Command(int lifeNum, int type)
        : mLifeNum{lifeNum}, mFaction{type}, mNumOfWarriors{0}, mNextGenerateIndex{0}, mIsAble{true} {}
    ~Command()
    {
        for (BaseWarrior *tmpPtr : mWarriors)
            delete tmpPtr;
    }
    void generate();
    bool able() { return mIsAble; }

private:
    int mLifeNum;
    int mFaction;
    int mNumOfWarriors[6]; // 0 ~ 4按顺序存储五种武士数，5存储武士总数
    int mNextGenerateIndex;
    bool mIsAble;
    std::vector<BaseWarrior *> mWarriors;
};

class Sword : public BaseWeapon
{
};
class Bomb : public BaseWeapon
{
};
class Arrow : public BaseWeapon
{
};

class Dragon : public BaseWarrior
{
public:
    Dragon(int id, int life, int faction, int commandLeftLife)
        : BaseWarrior(id, life, faction),
          mMorale{static_cast<double>(commandLeftLife) / INIT_ELEMENT[WarriorType::DRAGON]}
    {
        int weaponId = id % 3;
        switch (weaponId)
        {
        case 0:
            mWeaponPtr = new Sword;
            std::cout << "It has a sword,";
            break;
        case 1:
            mWeaponPtr = new Bomb;
            std::cout << "It has a bomb,";
            break;
        case 2:
            mWeaponPtr = new Arrow;
            std::cout << "It has a arrow,";
            break;
        }
        std::cout << "and it's morale is " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << mMorale << std::endl;
    }
    ~Dragon() { delete mWeaponPtr; }

private:
    BaseWeapon *mWeaponPtr;
    double mMorale;
};
class Ninja : public BaseWarrior
{
public:
    Ninja(int id, int life, int faction)
        : BaseWarrior(id, life, faction)
    {
        int weaponId = id % 3;
        switch (weaponId)
        {
        case 0:
            mWeaponPtr[0] = new Sword;
            mWeaponPtr[1] = new Bomb;
            std::cout << "It has a sword and a bomb" << std::endl;
            break;
        case 1:
            mWeaponPtr[0] = new Bomb;
            mWeaponPtr[1] = new Arrow;
            std::cout << "It has a bomb and a arrow" << std::endl;
            break;
        case 2:
            mWeaponPtr[0] = new Arrow;
            mWeaponPtr[1] = new Sword;
            std::cout << "It has a arrow and a sword" << std::endl;
            break;
        }
    }
    ~Ninja()
    {
        delete mWeaponPtr[0];
        delete mWeaponPtr[1];
    }

private:
    BaseWeapon *mWeaponPtr[2];
};
class Iceman : public BaseWarrior
{
public:
    Iceman(int id, int life, int faction)
        : BaseWarrior(id, life, faction)
    {
        int weaponId = id % 3;
        switch (weaponId)
        {
        case 0:
            mWeaponPtr = new Sword;
            std::cout << "It has a sword" << std::endl;
            break;
        case 1:
            mWeaponPtr = new Bomb;
            std::cout << "It has a bomb" << std::endl;
            break;
        case 2:
            mWeaponPtr = new Arrow;
            std::cout << "It has a arrow" << std::endl;
            break;
        }
    }
    ~Iceman() { delete mWeaponPtr; }

private:
    BaseWeapon *mWeaponPtr;
};
class Lion : public BaseWarrior
{
public:
    Lion(int id, int life, int faction, int loyalty)
        : BaseWarrior(id, life, faction), mLoyalty{loyalty}
    {
        std::cout << "It's loyalty is " << mLoyalty << std::endl;
    }

private:
    int mLoyalty;
};
class Wolf : public BaseWarrior
{
public:
    Wolf(int id, int life, int faction)
        : BaseWarrior(id, life, faction) {}
};
int main()
{
    int cases{0}, lifeNum{0};
    std::cin >> cases;
    for (int i{0}; i < cases; i++)
    {
        globalTime = 0;
        std::cin >> lifeNum >> INIT_ELEMENT[0] >> INIT_ELEMENT[1] >> INIT_ELEMENT[2] >> INIT_ELEMENT[3] >> INIT_ELEMENT[4];
        std::cout << "Case:" << i + 1 << std::endl;
        Command redCommand(lifeNum, Faction::RED), blueCommand(lifeNum, Faction::BLUE);
        while (redCommand.able() || blueCommand.able())
        {
            redCommand.generate();
            blueCommand.generate();
            globalTime++;
        }
    }
    return 0;
}
void Command::generate()
{
    if (mIsAble)
    {
        for (int i{0}; i < 5; i++)
        {
            int currentType{mFaction == Faction::RED ? RED_GENERATE_ORDER[mNextGenerateIndex] : BLUE_GENERATE_ORDER[mNextGenerateIndex]};
            if (mLifeNum >= INIT_ELEMENT[currentType])
            {
                mNumOfWarriors[5]++;
                mNumOfWarriors[currentType]++;
                mLifeNum -= INIT_ELEMENT[currentType];
                BaseWarrior *tmpWarriorPtr;
                std::cout << std::setw(3) << std::setfill('0') << globalTime << ' ';
                std::cout << (mFaction == Faction::RED ? "red " : "blue ");
                std::cout << WARRIOR_NAME[currentType] << ' ' << mNumOfWarriors[5]
                          << " born with strength " << INIT_ELEMENT[currentType] << ','
                          << mNumOfWarriors[currentType] << ' ' << WARRIOR_NAME[currentType] << " in "
                          << (mFaction == Faction::RED ? "red headquarter" : "blue headquarter")
                          << std::endl;
                switch (currentType)
                {
                case WarriorType::DRAGON:
                    tmpWarriorPtr = new Dragon(mNumOfWarriors[5], INIT_ELEMENT[WarriorType::DRAGON], mFaction, mLifeNum);
                    break;
                case WarriorType::NINJA:
                    tmpWarriorPtr = new Ninja(mNumOfWarriors[5], INIT_ELEMENT[WarriorType::NINJA], mFaction);
                    break;
                case WarriorType::ICEMAN:
                    tmpWarriorPtr = new Iceman(mNumOfWarriors[5], INIT_ELEMENT[WarriorType::ICEMAN], mFaction);
                    break;
                case WarriorType::LION:
                    tmpWarriorPtr = new Lion(mNumOfWarriors[5], INIT_ELEMENT[WarriorType::LION], mFaction, mLifeNum);
                    break;
                case WarriorType::WOLF:
                    tmpWarriorPtr = new Wolf(mNumOfWarriors[5], INIT_ELEMENT[WarriorType::WOLF], mFaction);
                    break;
                }
                mWarriors.push_back(tmpWarriorPtr);
                mNextGenerateIndex = mNextGenerateIndex == 4 ? 0 : mNextGenerateIndex + 1;
                return;
            }
            mNextGenerateIndex = mNextGenerateIndex == 4 ? 0 : mNextGenerateIndex + 1;
        }
        mIsAble = false;
        std::cout << std::setw(3) << std::setfill('0') << globalTime << ' ';
        std::cout << (mFaction == Faction::RED ? "red headquarter " : "blue headquarter ");
        std::cout << "stops making warriors" << std::endl;
    }
}
