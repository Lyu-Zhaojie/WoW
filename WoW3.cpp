#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>

using namespace std;

constexpr int DRAGON{0}, NINJA{1}, ICEMAN{2}, LION{3}, WOLF{4};
constexpr int RED{0}, BLUE{1};
constexpr int SWORD{0}, BOMB{2}, USED_ARROW{3}, ARROW{4};
constexpr char WARRIOR_NAME[5][8]{"dragon", "ninja", "iceman", "lion", "wolf"};
constexpr char HEAD_NAME[2][8]{"red", "blue"};
constexpr int ORDER[2][5]{{ICEMAN, LION, WOLF, NINJA, DRAGON}, {LION, DRAGON, NINJA, ICEMAN, WOLF}};

int INIT_ELEMENT[5]{0}, FORCE[5]{0};
int element{0}, nCity{0}, loyaltyStep{0}, timeLimit{0};

int globalMin;

void showTime() { cout << setw(3) << setfill('0') << globalMin / 60 << ':' << setw(2) << setfill('0') << globalMin % 60 << ' '; }

class BasicWarrior
{
protected:
    class WeaponList
    {
    public:
        int mSumWeapon, mWeaponNum[5];
        list<int> mWeapon;
        void refresh() { mWeapon.sort(); }
        WeaponList(int weapon1, int weapon2) : mSumWeapon{0}, mWeaponNum{0}
        {
            if (weapon1 >= 0)
            {
                mSumWeapon++;
                mWeaponNum[weapon1]++;
                mWeapon.push_back(weapon1);
            }
            if (weapon2 >= 0)
            {
                mSumWeapon++;
                mWeaponNum[weapon2]++;
                mWeapon.push_back(weapon2);
            }
        }
        int selectWeapon()
        {
            if (mWeapon.empty())
                return -1;
            int weapon{mWeapon.front()};
            mWeapon.pop_front();
            switch (weapon)
            {
            case SWORD:
                mWeapon.push_back(SWORD);
                return SWORD;
            case BOMB:
                mWeaponNum[BOMB]--;
                mSumWeapon--;
                return BOMB;
            case USED_ARROW:
                mWeaponNum[USED_ARROW]--;
                mSumWeapon--;
                return USED_ARROW;
            case ARROW:
                mWeaponNum[ARROW]--;
                mWeaponNum[USED_ARROW]++;
                mWeapon.push_back(USED_ARROW);
                return ARROW;
            default:
                cout << "error" << endl;
                return -1;
            }
        }
        void add(int weapon, int num)
        {
            mSumWeapon += num;
            mWeaponNum[weapon] += num;
            for (int i{0}; i < num; i++)
                mWeapon.push_back(weapon);
        }
        void del(int weapon, int num)
        {
            mSumWeapon -= num;
            mWeaponNum[weapon] -= num;
            int i{0};
            for (list<int>::iterator it{mWeapon.begin()}; i < num;)
            {
                if (*it == weapon)
                {
                    mWeapon.erase(it++);
                    i++;
                }
                else
                    it++;
            }
        }
    };
    int mID, mElement, mForce;
    int mType, mHead;
    WeaponList mWeaponList;
    struct Damage
    {
        int attack, self;
    };
    Damage getDamage()
    {
        int weapon{mWeaponList.selectWeapon()};
        Damage damage;
        switch (weapon)
        {
        case SWORD:
            damage.attack = mForce * 2 / 10;
            damage.self = 0;
            return damage;
        case BOMB:
            damage.attack = mForce * 4 / 10;
            damage.self = damage.attack / 2;
            return damage;
        case ARROW:
        case USED_ARROW:
            damage.attack = mForce * 3 / 10;
            damage.self = 0;
            return damage;
        default:
            damage.attack = 0;
            damage.self = 0;
            return damage;
        }
    }

public:
    BasicWarrior(int id, int type, int head, int weapon1 = -1, int weapon2 = -1) : mID{id}, mElement{INIT_ELEMENT[type]}, mForce{FORCE[type]}, mType{type}, mHead{head}, mWeaponList(weapon1, weapon2), weaponList{mWeaponList}
    {
        showTime();
        cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " born" << endl;
    }
    virtual ~BasicWarrior() {}
    virtual void yell(int cityIndex) {}
    virtual void beforeMove() {}
    virtual int getLoyalty() { return 1; }
    virtual void steal(BasicWarrior *enemy, int cityIndex) {}
    WeaponList &weaponList;
    void move(int cityIndex)
    {
        beforeMove();
        showTime();
        cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " marched to city " << cityIndex << " with " << mElement << " elements and force " << mForce << endl;
    }
    void moveToCommand()
    {
        beforeMove();
        showTime();
        cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " reached " << HEAD_NAME[mHead ^ 1] << " headquarter with " << mElement << " elements and force " << mForce << endl;
    }
    void report()
    {
        showTime();
        cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[mType] << ' ' << mID << " has " << mWeaponList.mWeaponNum[SWORD] << " sword " << mWeaponList.mWeaponNum[BOMB] << " bomb " << mWeaponList.mWeaponNum[ARROW] + mWeaponList.mWeaponNum[USED_ARROW] << " arrow and " << mElement << " elements" << endl;
    }
    virtual void attack(BasicWarrior *enemy)
    {
        auto [x, y]{getDamage()};
        enemy->attacked(x);
        attacked(y);
    }
    void seize(BasicWarrior *enemy)
    {
        int seizeNum{10 - mWeaponList.mSumWeapon};
        if (seizeNum && enemy->mWeaponList.mWeaponNum[SWORD])
        {
            int seizeSword{std::min(seizeNum, enemy->mWeaponList.mWeaponNum[SWORD])};
            seizeNum -= seizeSword;
            mWeaponList.add(SWORD, seizeSword);
        }
        if (seizeNum && enemy->mWeaponList.mWeaponNum[BOMB])
        {
            int seizeBomb{std::min(seizeNum, enemy->mWeaponList.mWeaponNum[BOMB])};
            seizeNum -= seizeBomb;
            mWeaponList.add(BOMB, seizeBomb);
        }
        if (seizeNum && enemy->mWeaponList.mWeaponNum[ARROW])
        {
            int seizeArrow{std::min(seizeNum, enemy->mWeaponList.mWeaponNum[ARROW])};
            seizeNum -= seizeArrow;
            mWeaponList.add(ARROW, seizeArrow);
        }
        if (seizeNum && enemy->mWeaponList.mWeaponNum[USED_ARROW])
        {
            int seizeUsedArrow{std::min(seizeNum, enemy->mWeaponList.mWeaponNum[USED_ARROW])};
            seizeNum -= seizeUsedArrow;
            mWeaponList.add(USED_ARROW, seizeUsedArrow);
        }
    }
    void attacked(int a) { mElement -= a; }
    bool alive() { return mElement > 0; }
    int getType() { return mType; }
    int getID() { return mID; }
    int getForce() { return mForce; }
    int getElement() { return mElement; }
};
class Dragon : public BasicWarrior
{
public:
    Dragon(int id, int head) : BasicWarrior(id, DRAGON, head, (id % 3) * 2) {}
    void yell(int cityIndex) override
    {
        showTime();
        cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[DRAGON] << ' ' << mID << " yelled in city " << cityIndex << endl;
    }
};
class Ninja : public BasicWarrior
{
public:
    Ninja(int id, int head) : BasicWarrior(id, NINJA, head, (id % 3) * 2, ((id + 1) % 3) * 2) {}
    void attack(BasicWarrior *enemy) override
    {
        Damage damage{getDamage()};
        enemy->attacked(damage.attack);
    }
};
class Iceman : public BasicWarrior
{
public:
    Iceman(int id, int head) : BasicWarrior(id, ICEMAN, head, (id % 3) * 2) {}
    void beforeMove() override { mElement -= mElement / 10; }
};
class Lion : public BasicWarrior
{
public:
    Lion(int id, int head, int loyalty) : BasicWarrior(id, LION, head, (id % 3) * 2), mLoyalty{loyalty} { cout << "Its loyalty is " << mLoyalty << endl; }
    int getLoyalty() override { return mLoyalty; }
    void beforeMove() override { mLoyalty -= loyaltyStep; }

private:
    int mLoyalty;
};
class Wolf : public BasicWarrior
{
public:
    Wolf(int id, int head) : BasicWarrior(id, WOLF, head) {}
    void steal(BasicWarrior *enemy, int cityIndex) override
    {
        if (enemy->getType() != WOLF)
        {
            if (enemy->weaponList.mWeaponNum[SWORD])
            {
                int stealSword{std::min(10 - mWeaponList.mSumWeapon, enemy->weaponList.mWeaponNum[SWORD])};
                enemy->weaponList.del(SWORD, stealSword);
                mWeaponList.add(SWORD, stealSword);
                showTime();
                cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[WOLF] << ' ' << mID << " took " << stealSword << " sword from " << HEAD_NAME[mHead ^ 1] << ' ' << WARRIOR_NAME[enemy->getType()] << ' ' << enemy->getID() << " in city " << cityIndex << endl;
                return;
            }
            if (enemy->weaponList.mWeaponNum[BOMB])
            {
                int stealBomb{std::min(10 - mWeaponList.mSumWeapon, enemy->weaponList.mWeaponNum[BOMB])};
                enemy->weaponList.del(BOMB, stealBomb);
                mWeaponList.add(BOMB, stealBomb);
                showTime();
                cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[WOLF] << ' ' << mID << " took " << stealBomb << " bomb from " << HEAD_NAME[mHead ^ 1] << ' ' << WARRIOR_NAME[enemy->getType()] << ' ' << enemy->getID() << " in city " << cityIndex << endl;
                return;
            }
            if (enemy->weaponList.mWeaponNum[ARROW] + enemy->weaponList.mWeaponNum[USED_ARROW])
            {
                int stealArrow{std::min(10 - mWeaponList.mSumWeapon, enemy->weaponList.mWeaponNum[ARROW] + enemy->weaponList.mWeaponNum[USED_ARROW])};
                int enemyUnusedArrow{enemy->weaponList.mWeaponNum[ARROW]};
                int stealUsedArrow{std::max(0, stealArrow - enemyUnusedArrow)};
                enemy->weaponList.del(ARROW, stealArrow - stealUsedArrow);
                enemy->weaponList.del(USED_ARROW, stealUsedArrow);
                mWeaponList.add(ARROW, stealArrow - stealUsedArrow);
                mWeaponList.add(USED_ARROW, stealUsedArrow);
                showTime();
                cout << HEAD_NAME[mHead] << ' ' << WARRIOR_NAME[WOLF] << ' ' << mID << " took " << stealArrow << " arrow from " << HEAD_NAME[mHead ^ 1] << ' ' << WARRIOR_NAME[enemy->getType()] << ' ' << enemy->getID() << " in city " << cityIndex << endl;
                return;
            }
        }
    }
};
class City
{
public:
    enum class WAR_STATE
    {
        RED_WIN,
        BLUE_WIN,
        BOTH_DIE,
        BOTH_ALIVE,
        NOT_END
    };
    City() : mWarriorPtr{nullptr, nullptr}, tmpWarriorPtr{nullptr, nullptr} {}
    City(int id) : mID{id}, mWarriorPtr{nullptr, nullptr}, tmpWarriorPtr{nullptr, nullptr} {}
    ~City()
    {
        for (int i : {RED, BLUE})
        {
            if (mWarriorPtr[i])
            {
                delete mWarriorPtr[i];
                mWarriorPtr[i] = nullptr;
            }
            if (tmpWarriorPtr[i])
            {
                delete tmpWarriorPtr[i];
                tmpWarriorPtr[i] = nullptr;
            }
        }
    }
    virtual void lionEscape()
    {
        for (int i : {RED, BLUE})
        {
            if (mWarriorPtr[i] && mWarriorPtr[i]->getType() == LION && mWarriorPtr[i]->getLoyalty() <= 0)
            {
                showTime();
                cout << HEAD_NAME[i] << " lion " << mWarriorPtr[i]->getID() << " ran away" << endl;
                delete mWarriorPtr[i];
                mWarriorPtr[i] = nullptr;
            }
        }
    }
    void reportWeapon()
    {
        if (mWarriorPtr[RED])
            mWarriorPtr[RED]->report();
        if (mWarriorPtr[BLUE])
            mWarriorPtr[BLUE]->report();
    }
    friend void Move(City &city1, City &city2);
    virtual void setPtr()
    {
        for (int i : {RED, BLUE})
        {
            mWarriorPtr[i] = tmpWarriorPtr[i];
            tmpWarriorPtr[i] = nullptr;
            if (mWarriorPtr[i])
                mWarriorPtr[i]->move(mID);
        }
    }
    void wolfSteal()
    {
        if (mWarriorPtr[RED] && mWarriorPtr[BLUE])
        {
            mWarriorPtr[RED]->steal(mWarriorPtr[BLUE], mID);
            mWarriorPtr[BLUE]->steal(mWarriorPtr[RED], mID);
        }
    }
    WAR_STATE checkWarState()
    {
        if (mWarriorPtr[RED]->alive() && !mWarriorPtr[BLUE]->alive())
            return WAR_STATE::RED_WIN;
        if (!mWarriorPtr[RED]->alive() && mWarriorPtr[BLUE]->alive())
            return WAR_STATE::BLUE_WIN;
        if (!mWarriorPtr[RED]->alive() && !mWarriorPtr[BLUE]->alive())
            return WAR_STATE::BOTH_DIE;
        if (mWarriorPtr[RED]->weaponList.mSumWeapon == 0 && mWarriorPtr[BLUE]->weaponList.mSumWeapon == 0)
            return WAR_STATE::BOTH_ALIVE;
        if ((mWarriorPtr[RED]->weaponList.mWeaponNum[BOMB] == 0 && mWarriorPtr[RED]->weaponList.mWeaponNum[ARROW] == 0 && mWarriorPtr[RED]->weaponList.mWeaponNum[USED_ARROW] == 0) && (mWarriorPtr[BLUE]->weaponList.mWeaponNum[BOMB] == 0 && mWarriorPtr[BLUE]->weaponList.mWeaponNum[ARROW] == 0 && mWarriorPtr[BLUE]->weaponList.mWeaponNum[USED_ARROW] == 0) && (mWarriorPtr[RED]->getForce() * 2 / 10 == 0 && mWarriorPtr[BLUE]->getForce() * 2 / 10 == 0))
            return WAR_STATE::BOTH_ALIVE;
        return WAR_STATE::NOT_END;
    }
    bool warEnd()
    {
        WAR_STATE state{checkWarState()};
        switch (state)
        {
        case WAR_STATE::RED_WIN:
            showTime();
            cout << "red " << WARRIOR_NAME[mWarriorPtr[RED]->getType()] << ' ' << mWarriorPtr[RED]->getID() << " killed blue " << WARRIOR_NAME[mWarriorPtr[BLUE]->getType()] << ' ' << mWarriorPtr[BLUE]->getID() << " in city " << mID << " remaining " << mWarriorPtr[RED]->getElement() << " elements" << endl;
            mWarriorPtr[RED]->yell(mID);
            mWarriorPtr[RED]->seize(mWarriorPtr[BLUE]);
            delete mWarriorPtr[BLUE];
            mWarriorPtr[BLUE] = nullptr;
            return true;
        case WAR_STATE::BLUE_WIN:
            showTime();
            cout << "blue " << WARRIOR_NAME[mWarriorPtr[BLUE]->getType()] << ' ' << mWarriorPtr[BLUE]->getID() << " killed red " << WARRIOR_NAME[mWarriorPtr[RED]->getType()] << ' ' << mWarriorPtr[RED]->getID() << " in city " << mID << " remaining " << mWarriorPtr[BLUE]->getElement() << " elements" << endl;
            mWarriorPtr[BLUE]->yell(mID);
            mWarriorPtr[BLUE]->seize(mWarriorPtr[RED]);
            delete mWarriorPtr[RED];
            mWarriorPtr[RED] = nullptr;
            return true;
        case WAR_STATE::BOTH_DIE:
            showTime();
            cout << "both red " << WARRIOR_NAME[mWarriorPtr[RED]->getType()] << ' ' << mWarriorPtr[RED]->getID() << " and blue " << WARRIOR_NAME[mWarriorPtr[BLUE]->getType()] << ' ' << mWarriorPtr[BLUE]->getID() << " died in city " << mID << endl;
            delete mWarriorPtr[BLUE];
            mWarriorPtr[BLUE] = nullptr;
            delete mWarriorPtr[RED];
            mWarriorPtr[RED] = nullptr;
            return true;
        case WAR_STATE::BOTH_ALIVE:
            showTime();
            cout << "both red " << WARRIOR_NAME[mWarriorPtr[RED]->getType()] << ' ' << mWarriorPtr[RED]->getID() << " and blue " << WARRIOR_NAME[mWarriorPtr[BLUE]->getType()] << ' ' << mWarriorPtr[BLUE]->getID() << " were alive in city " << mID << endl;
            mWarriorPtr[RED]->yell(mID);
            mWarriorPtr[BLUE]->yell(mID);
            return true;
        case WAR_STATE::NOT_END:
            return false;
        default:
            return true;
        }
    }
    void war()
    {
        if (mWarriorPtr[RED] && mWarriorPtr[BLUE])
        {
            mWarriorPtr[RED]->weaponList.refresh();
            mWarriorPtr[BLUE]->weaponList.refresh();
            if (mID & 1)
            {
                mWarriorPtr[RED]->attack(mWarriorPtr[BLUE]);
                if (warEnd())
                    return;
            }
            for (;;)
            {
                mWarriorPtr[BLUE]->attack(mWarriorPtr[RED]);
                if (warEnd())
                    return;
                mWarriorPtr[RED]->attack(mWarriorPtr[BLUE]);
                if (warEnd())
                    return;
            }
        }
    }

protected:
    int mID;
    BasicWarrior *mWarriorPtr[2];
    BasicWarrior *tmpWarriorPtr[2];
};
class Command : public City
{
public:
    Command(int id, int head, int element) : City(id), isAble{true}, occupied{false}, nOfWarrior{0}, mHead{head}, mElement{element}, mNextGenIndex{0} {}
    bool able() { return isAble; }
    void report()
    {
        showTime();
        cout << mElement << " elements in " << HEAD_NAME[mHead] << " headquarter" << endl;
    }
    void generate();
    void lionEscape() override
    {
        if (mWarriorPtr[mHead] && mWarriorPtr[mHead]->getType() == LION && mWarriorPtr[mHead]->getLoyalty() <= 0)
        {
            showTime();
            cout << HEAD_NAME[mHead] << " lion " << mWarriorPtr[mHead]->getID() << " ran away" << endl;
            delete mWarriorPtr[mHead];
            mWarriorPtr[mHead] = nullptr;
        }
    }
    void setPtr() override
    {
        mWarriorPtr[RED] = tmpWarriorPtr[RED];
        tmpWarriorPtr[RED] = nullptr;
        if (mWarriorPtr[RED] && mHead == BLUE)
        {
            mWarriorPtr[RED]->moveToCommand();
            occupied = true;
        }
        mWarriorPtr[BLUE] = tmpWarriorPtr[BLUE];
        tmpWarriorPtr[BLUE] = nullptr;
        if (mWarriorPtr[BLUE] && mHead == RED)
        {
            mWarriorPtr[BLUE]->moveToCommand();
            occupied = true;
        }
    }
    bool isOccupied() { return occupied; }

private:
    bool isAble, occupied;
    int nOfWarrior, mHead, mElement, mNextGenIndex;
};
int main()
{
    int cases{0};
    cin >> cases;
    for (int i{0}; i < cases; i++)
    {
        cin >> element >> nCity >> loyaltyStep >> timeLimit;
        cin >> INIT_ELEMENT[0] >> INIT_ELEMENT[1] >> INIT_ELEMENT[2] >> INIT_ELEMENT[3] >> INIT_ELEMENT[4];
        cin >> FORCE[0] >> FORCE[1] >> FORCE[2] >> FORCE[3] >> FORCE[4];
        cout << "Case " << i + 1 << ':' << endl;
        Command redCommand(0, RED, element), blueCommand(nCity + 1, BLUE, element);
        City *cites = new City[nCity + 1];
        for (int j{1}; j <= nCity; j++)
            cites[j] = City(j);
        for (globalMin = 0; globalMin <= timeLimit; globalMin += 5)
        {
            switch (globalMin % 60)
            {
            case 0:
                redCommand.generate();
                blueCommand.generate();
                break;
            case 5:
                redCommand.lionEscape();
                for (int j{1}; j <= nCity; j++)
                    cites[j].lionEscape();
                blueCommand.lionEscape();
                break;
            case 10:
                Move(redCommand, cites[1]);
                for (int j{1}; j < nCity; j++)
                    Move(cites[j], cites[j + 1]);
                Move(cites[nCity], blueCommand);
                redCommand.setPtr();
                if (redCommand.isOccupied())
                {
                    showTime();
                    cout << "red headquarter was taken" << endl;
                }
                for (int j{1}; j <= nCity; j++)
                    cites[j].setPtr();
                blueCommand.setPtr();
                if (blueCommand.isOccupied())
                {
                    showTime();
                    cout << "blue headquarter was taken" << endl;
                }
                if (redCommand.isOccupied() || blueCommand.isOccupied())
                    goto endWar;
                break;
            case 35:
                for (int j{1}; j <= nCity; j++)
                    cites[j].wolfSteal();
                break;
            case 40:
                for (int j{1}; j <= nCity; j++)
                    cites[j].war();
                break;
            case 50:
                redCommand.report();
                blueCommand.report();
                break;
            case 55:
                for (int j{1}; j <= nCity; j++)
                    cites[j].reportWeapon();
                break;
            default:
                break;
            }
        }
    endWar:
        delete[] cites;
        cites = nullptr;
    }
}
void Command::generate()
{
    if (isAble)
    {
        bool able{mElement >= INIT_ELEMENT[ORDER[mHead][mNextGenIndex]]};
        if (able)
        {
            nOfWarrior++;
            mElement -= INIT_ELEMENT[ORDER[mHead][mNextGenIndex]];
            switch (ORDER[mHead][mNextGenIndex])
            {
            case DRAGON:
                mWarriorPtr[mHead] = new Dragon(nOfWarrior, mHead);
                break;
            case NINJA:
                mWarriorPtr[mHead] = new Ninja(nOfWarrior, mHead);
                break;
            case ICEMAN:
                mWarriorPtr[mHead] = new Iceman(nOfWarrior, mHead);
                break;
            case LION:
                mWarriorPtr[mHead] = new Lion(nOfWarrior, mHead, mElement);
                break;
            case WOLF:
                mWarriorPtr[mHead] = new Wolf(nOfWarrior, mHead);
                break;
            }
            mNextGenIndex = (mNextGenIndex + 1) % 5;
            return;
        }
        isAble = false;
    }
}
void Move(City &city1, City &city2)
{
    city2.tmpWarriorPtr[RED] = city1.mWarriorPtr[RED];
    city1.mWarriorPtr[RED] = nullptr;
    city1.tmpWarriorPtr[BLUE] = city2.mWarriorPtr[BLUE];
    city2.mWarriorPtr[BLUE] = nullptr;
}
