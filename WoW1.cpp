#include <iostream>

enum class CommandType
{
    RED,
    BLUE
};

enum class WarriorType
{
    DRAGON,
    NINJA,
    ICEMAN,
    LION,
    WOLF
};

int INIT_LIFE[5]{0};
constexpr char WARRIORS_NAME[5][7]{"dragon", "ninja", "iceman", "lion", "wolf"};

constexpr WarriorType RED_GENERATE_ORDER[5]{WarriorType::ICEMAN, WarriorType::LION, WarriorType::WOLF, WarriorType::NINJA, WarriorType::DRAGON};

constexpr WarriorType BLUE_GENERATE_ORDER[5]{WarriorType::LION, WarriorType::DRAGON, WarriorType::NINJA, WarriorType::ICEMAN, WarriorType::WOLF};

class Warrior
{
public:
    Warrior() {}

    Warrior(WarriorType type, int id, int life) : mId{id}, mLife{life}, mType{type} {}

    int GetType() { return int(mType); }

    int mId, mLife;

private:
    WarriorType mType;
};

class Command
{
public:
    Command(int lifeNum, CommandType type) : mLifeNum{lifeNum}, mNumOfWarriors{0}, mType{type}, mNextGenerate{0} {}

    Warrior *Generate();

    int mLifeNum, mNumOfWarriors[6]; // 0 ~ 4按顺序存储五种武士数，5存储武士总数
private:
    // std::vector<Warrior> warriors;
    Warrior mWarriors[256]; // 1 ~ 255存储武士。。。0不存
    CommandType mType;
    int mNextGenerate;
};

int main()
{
    int cases{0};
    int lifes{0};
    std::cin >> cases;
    for (int i = 0; i < cases; i++)
    {
        bool redAble{true}, blueAble{true};
        bool redUnable{false}, blueUnable{false};
        std::cin >> lifes;
        Command redCommand(lifes, CommandType::RED),
            blueCommand(lifes, CommandType::BLUE);
        std::cin >>
            INIT_LIFE[int(WarriorType::DRAGON)] >>
            INIT_LIFE[int(WarriorType::NINJA)] >>
            INIT_LIFE[int(WarriorType::ICEMAN)] >>
            INIT_LIFE[int(WarriorType::LION)] >>
            INIT_LIFE[int(WarriorType::WOLF)];
        std::cout << "Case:" << i + 1 << std::endl;
        std::cout.fill('0');
        int time{0};
        Warrior *redWarriorPtr{nullptr}, *blueWarriorPtr{nullptr};
        while (redAble || blueAble)
        {
            redWarriorPtr = redCommand.Generate();
            blueWarriorPtr = blueCommand.Generate();
            redAble = (redWarriorPtr != nullptr) && redAble;
            blueAble = (blueWarriorPtr != nullptr) && blueAble;
            if (redAble)
            {
                std::cout.width(3);
                std::cout << time;
                std::cout << " red " << WARRIORS_NAME[redWarriorPtr->GetType()] << ' '
                          << redWarriorPtr->mId << " born with strength " << redWarriorPtr->mLife << ','
                          << redCommand.mNumOfWarriors[int(redWarriorPtr->GetType())] << ' '
                          << WARRIORS_NAME[redWarriorPtr->GetType()] << " in red headquarter" << std::endl;
            }
            else if (!redUnable)
            {
                std::cout.width(3);
                std::cout << time;
                std::cout << " red headquarter stops making warriors" << std::endl;
                redUnable = true;
            }
            if (blueAble)
            {
                std::cout.width(3);
                std::cout << time;
                std::cout << " blue " << WARRIORS_NAME[blueWarriorPtr->GetType()] << ' '
                          << blueWarriorPtr->mId << " born with strength " << blueWarriorPtr->mLife << ','
                          << blueCommand.mNumOfWarriors[int(blueWarriorPtr->GetType())] << ' '
                          << WARRIORS_NAME[blueWarriorPtr->GetType()] << " in blue headquarter" << std::endl;
            }
            else if (!blueUnable)
            {
                std::cout.width(3);
                std::cout << time;
                std::cout << " blue headquarter stops making warriors" << std::endl;
                blueUnable = true;
            }
            time++;
        }
    }
    return 0;
}

Warrior *Command::Generate()
{
    bool able{false};
    for (int i = 0; i < 5; i++)
    {
        WarriorType currentType{mType == CommandType::RED ? RED_GENERATE_ORDER[mNextGenerate] : BLUE_GENERATE_ORDER[mNextGenerate]};
        able = mLifeNum >= INIT_LIFE[int(currentType)];
        if (able)
        {
            Warrior newWarrior(currentType, ++mNumOfWarriors[5], INIT_LIFE[int(currentType)]);
            mNumOfWarriors[int(currentType)]++;
            mWarriors[mNumOfWarriors[5]] = newWarrior;
            mLifeNum -= INIT_LIFE[int(currentType)];
            mNextGenerate = mNextGenerate == 4 ? 0 : mNextGenerate + 1;
            return &mWarriors[mNumOfWarriors[5]];
        }
        mNextGenerate = mNextGenerate == 4 ? 0 : mNextGenerate + 1;
    }
    return nullptr;
}
