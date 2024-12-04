#define CATCH_CONFIG_MAIN
#include "./dungeon/dungeon.hpp"
#include "./creature/enemy/undead/undeadSummoner/undeadSummoner.hpp"
#include "./creature/enemy/alive/aliveSummoner/aliveSummoner.hpp"
#include <random> 
#include <sstream>
#include <catch2/catch_test_macros.hpp>
using namespace dng;
using namespace creature;
//       rm ./CMakeFiles/testing.dir/testing.cpp.gcda


TEST_CASE("Levels"){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10);
    SECTION("Empty constructor"){
        Level a;
        REQUIRE(a.getHeight()==0);
        REQUIRE(a.getWidth()==0);
        REQUIRE(a.getEnemies().size()==0);
        REQUIRE(a.getField().getColumns()==0);
    }
    SECTION("Constructor"){
        std::vector<std::shared_ptr<creature::Enemy>> c={};
        size_t r1 = distrib(gen), r2 = distrib(gen);
        Matrix<Cell> b(r1, r2, Cell());
        Level a(b, c);
        REQUIRE(a.getHeight()==r1);
        REQUIRE(a.getWidth()==r2);
        REQUIRE(a.getEnemies().size()==0);
    }
    SECTION("Setterrs"){
        Coordinates cord{1, 2}, bad{5, 6};
        std::vector<std::shared_ptr<creature::Enemy>> c={std::make_shared<Enemy>(Enemy())};
        Matrix<Cell> b(3, 4, Cell());
        Level a(b, c);
        a.setCellType(CellType::Wall, cord);
        REQUIRE(a.getEnemies().size()==1);
        REQUIRE(a.getField()[2][1].getCellType()==CellType::Wall);
        REQUIRE_THROWS_AS(a.setCellType(CellType::Wall, bad), std::invalid_argument);
        REQUIRE_THROWS_AS(a.changeDoor(bad), std::invalid_argument);
        REQUIRE_THROWS_AS(a.changeDoor(Coordinates{2, 2}), std::invalid_argument);
        a.setCellType(CellType::OpenedDoor, cord);
        a.changeDoor(cord);
        REQUIRE(a.getField()[2][1].getCellType()==CellType::ClosedDoor);
        a.changeDoor(cord);
        REQUIRE(a.getField()[2][1].getCellType()==CellType::OpenedDoor);
    }
    SECTION("Cell"){
        Cell b;
        size_t r1 = distrib(gen);
        REQUIRE(b.getCellType()==CellType::Floor);
        REQUIRE(b.getEssence()==0);
        b.setEssence(r1);
        REQUIRE(b.getEssence()==r1);
        REQUIRE_THROWS_AS(b.setCellType(CellType::Upstairs), std::invalid_argument);
    }
}

TEST_CASE("Dungeon"){
    SECTION("Empty constructor"){
        Dungeon a;
        Character b;
        REQUIRE(a.getCurLvl()==0);
        REQUIRE(a.getEnemiesData()==nullptr);
        REQUIRE(a.getCharacter().getDamage()==b.getDamage());
        REQUIRE(a.getCharacter().getLevel()==b.getLevel());
        REQUIRE(a.getLevels().size()==0);
    }
    SECTION("Constructor"){
        Character b;
        std::vector<Level> levels={};
        std::shared_ptr<creature::Enemy> c={};
        std::shared_ptr<Enemy[]> enemies(new Enemy[1]);
        enemies[0]=Enemy();
        Dungeon a(levels, 0, enemies, 1, b);
        REQUIRE(a.getEnemiesData()!=nullptr);
    }
}

TEST_CASE("Creature"){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(101, 1000);
    Matrix<Cell> b(3, 4, Cell());
    std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
    Coordinates good{1, 1}, bad{111, 111};
    SECTION("Empty constructor"){
        Creature a;
        REQUIRE(a.getCoordinates().x==0);
        REQUIRE(a.getMaxHealth()==100);
        REQUIRE(a.getCurrentHealth()==100);
        REQUIRE(a.getFraction()==Fraction::Human);
        REQUIRE(a.getDamage()==50);
    }
    SECTION("Constructors + setters"){
        size_t r1 = distrib(gen);
        Creature a(r1), b(r1, Fraction::Goblin), c(Fraction::Goblin), nice(good, field, 10, 10, Fraction::Goblin, 10);
        REQUIRE(a.getMaxHealth()==r1);
        REQUIRE(b.getFraction()==Fraction::Goblin);
        REQUIRE(c.getFraction()==Fraction::Goblin);
        (*field)[good.y][good.x].setCellType(CellType::Wall);
        REQUIRE_THROWS_AS(Creature(good, field, 10, 10, Fraction::Goblin, 10), std::invalid_argument);
        (*field)[good.y][good.x].setCellType(CellType::Floor);
        REQUIRE_THROWS_AS(Creature(good, field, 10, 20, Fraction::Goblin, 10), std::invalid_argument);
        a.setCoordinates(good);
        REQUIRE(a.getCoordinates().x==good.x);
        REQUIRE_THROWS_AS(nice.setCoordinates(bad), std::invalid_argument);
        size_t f = c.getCurrentHealth();
        c.setMaxHealth(f);
        REQUIRE(c.getMaxHealth()==f);
        REQUIRE_THROWS_AS(c.setCurrentHealth(c.getMaxHealth()+10), std::invalid_argument);
        REQUIRE_THROWS_AS(c.setMaxHealth(0), std::invalid_argument);
        REQUIRE(b.setDamage(r1).getDamage()==r1);
        REQUIRE(nice.setMaxHealth(r1).setCurrentHealth(r1).getCurrentHealth()==r1);
        REQUIRE(nice.setFraction(Fraction::Dwarf).getFraction()==Fraction::Dwarf);
    }

    SECTION("Combat"){
        Creature a, b;
        a.setFraction(Fraction::Dwarf);
        REQUIRE(a.toAttack(b)==1);
        REQUIRE(b.getCurrentHealth()==a.getMaxHealth()-b.getDamage());
        b.setMaxHealth(1000);
        b.setCurrentHealth(b.getMaxHealth());
        REQUIRE(a.toCurse(b)==1);
        REQUIRE(b.getCurrentHealth()==b.getMaxHealth()-100);
        b.setMaxHealth(10);
        Creature cool(good, field, 1000, 1000, Fraction::Goblin, 10);
        (*field)[good.y][good.x].setCellType(CellType::Lava);
        REQUIRE(cool.getLavaHit()==1);
    }

    SECTION("Movement"){
        Creature cool(good, field, 1000, 1000, Fraction::Goblin, 10);
        REQUIRE(cool.follow()==0);
        Coordinates newc{good.x+1, good.y};
        cool.move(newc);
        REQUIRE(cool.getCoordinates().x==newc.x);
        REQUIRE(cool.getCoordinates().y==newc.y);
    }
}

TEST_CASE("Character"){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(101, 1000);
    Matrix<Cell> b(3, 4, Cell());
    std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
    Coordinates good{1, 1};
    SECTION("Empty constructor"){
        Character a;
        REQUIRE(a.getMaxMana()==100);
        REQUIRE(a.getCurrentMana()==100);
        REQUIRE(a.getEssence()==0);
        REQUIRE(a.getLevel()==0);
        REQUIRE(a.getTable().getMorphismLevel()==0);
    }
    SECTION("Constructor 1"){
        size_t r = distrib(gen);
        Character a(r, r);
        REQUIRE(a.getMaxMana()==r);
        REQUIRE(a.getMaxHealth()==r);
    }
    SECTION("Constructor 2"){
        REQUIRE_THROWS_AS(Character(good, field, 20, 20, 10, 100, 200), std::invalid_argument);
        REQUIRE_THROWS_AS(Character(good, field, 20, 20, 10, 100, 100, 10, 10, 31), std::invalid_argument);
    }
    SECTION("Setters"){
        size_t r = distrib(gen);
        Character a;
        REQUIRE_THROWS_AS(a.setMaxMana(10), std::invalid_argument);
        REQUIRE(a.setMaxMana(r).getMaxMana()==r);
        REQUIRE(a.setCurrentMana(r).getCurrentMana()==r);
        REQUIRE_THROWS_AS(a.setCurrentMana(r+1), std::invalid_argument);
        REQUIRE(a.setEssence(r).getEssence()==r);
        REQUIRE(a.setLevel(20).getLevel()==20);
        REQUIRE_THROWS_AS(a.setLevel(40), std::invalid_argument);
        REQUIRE(a.setSkillsTable(SkillsTable()).getTable().getCurseLevel()==0);
    }
    SECTION("Hero features"){
        size_t r = distrib(gen)%20+20;
        Character a(good, field, 20, 20, 10, 100, 100);
        (*field)[good.y][good.x].setEssence(r);
        REQUIRE(a.collectEssence()==1);
        REQUIRE(a.getEssence()==r);
        REQUIRE(a.collectEssence()==0);
        REQUIRE(a.boostLevel()==0);
        a.setExperience(2000);
    }
    SECTION("Combat"){
        Character a(Coordinates(0, 0), field, 20, 20, 10, 1000, 1000);
        a.setDamage(1000);
        Alive b, c;
        REQUIRE(a.toAttack(b)==1);
        REQUIRE(a.getExperience()!=0);
        a.setEssence(100).upgradeCurse();
        REQUIRE(a.toCurse(c)==1);
        SECTION("Desiccation"){
            a.upgradeDesiccation();
            a.toAttack(c);  
            REQUIRE(a.desiccation(b, 1)==1);
            REQUIRE(a.desiccation(c, 0)==1);
            REQUIRE(a.desiccation(c, 0)==0);
        }
        SECTION("Morphism"){
            Undead d;
            d.setUndeadType(UndeadType::Skeleton);
            a.upgradeMorphism();
            a.learnUndead();
            REQUIRE(a.morphism(d, UndeadType::Zombie)==1);
            REQUIRE(a.morphism(d, UndeadType::Phantom)==0);
            REQUIRE(d.getUndeadType() == UndeadType::Zombie);
        }
        SECTION("Necromancy"){
            std::shared_ptr<Alive> alive = std::make_shared<Alive>(Alive());
            a.toAttack(*alive);
            std::shared_ptr<Enemy> clone = alive;
            std::vector<std::shared_ptr<Enemy>> enemies = {clone};
            a.upgradeNecromancy();
            a.learnUndead();
            REQUIRE(a.necromancy(*alive, UndeadType::Zombie, enemies)==1);
            std::shared_ptr<Undead> undead = std::dynamic_pointer_cast<Undead>(enemies[1]);
            REQUIRE(undead!=nullptr);
            REQUIRE(undead->getUndeadType()==UndeadType::Zombie);
        }
    }
    SECTION("Skills table"){
        size_t r = distrib(gen)%20+100;
        size_t lvl = distrib(gen)%4+1, cost = 10 + distrib(gen)%20, rate = 10 + distrib(gen)%20;
        SkillsTable sk;
        std::vector<size_t> info = {0, 10};
        REQUIRE_THROWS_AS(SkillsTable({UndeadType::Skeleton}, info), std::invalid_argument);
        REQUIRE(sk.learnUndead(r)==1);
        REQUIRE(sk.learnUndead(r)==1);
        REQUIRE(sk.learnUndead(r)==1);
        REQUIRE(sk.learnUndead(r)==0);
        REQUIRE(sk.getSkills()==0);
        REQUIRE_THROWS_AS(sk.setNecromancyLevel(10), std::invalid_argument);
        REQUIRE_THROWS_AS(sk.setMorphismLevel(10), std::invalid_argument);
        REQUIRE_THROWS_AS(sk.setCurseLevel(10), std::invalid_argument);
        REQUIRE_THROWS_AS(sk.setDesiccationLevel(10), std::invalid_argument);
        REQUIRE(sk.setNecromancyLevel(lvl).getNecromancyLevel()==lvl);
        REQUIRE(sk.setCurseLevel(lvl).getCurseLevel()==lvl);
        REQUIRE(sk.setMorphismLevel(lvl).getMorphismLevel()==lvl);
        REQUIRE(sk.setDesiccationLevel(lvl).getDesiccationLevel()==lvl);
        
        REQUIRE(sk.setDesRate(rate).getDesRate()==rate);
        REQUIRE(sk.setCurseDamage(rate).getCurseDamage()==rate);
        REQUIRE(sk.setCurseCost(cost).getCurseCost()==cost);
        REQUIRE(sk.setMorphismCost(cost).getMorphismCost()==cost);
        REQUIRE(sk.setNecroCost(cost).getNecroCost()==cost);
        REQUIRE(sk.getSkills()==4);
    }
}

TEST_CASE("Enemy"){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(101, 1000);
    Matrix<Cell> b(3, 4, Cell());
    std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
    Coordinates next{0, 2};
    SECTION("Constructor"){
        std::shared_ptr<Character> hero = std::make_shared<Character>(Character());            
        std::shared_ptr<Creature> clone = hero;
        REQUIRE_THROWS_AS(Enemy(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, EnemyType::Alive, Condition::Dead,"bob", clone), std::invalid_argument);
        Enemy a(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, EnemyType::Alive, Condition::Calm,"bob", clone);   
        REQUIRE(a.getName()=="bob");
    }
    SECTION("setters"){
        Enemy a;
        a.setCondition(Condition::Calm);
        REQUIRE(a.getCondition()==Condition::Calm);
        REQUIRE(a.getTarget()==nullptr);
        REQUIRE(a.setType(EnemyType::Golem).getType()==EnemyType::Golem);
        REQUIRE(a.setName("Shapkin").getName()=="Shapkin");
        REQUIRE(a.setTarget(nullptr).getTarget()==nullptr);
    }
    SECTION("Follow"){
        std::shared_ptr<Character> hero = std::make_shared<Character>(Character());
        hero->setField(field);      
        std::shared_ptr<Creature> clone = hero;
        clone->setCoordinates(next);
        Enemy a(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, EnemyType::Alive, Condition::Calm,"bob", clone);   
        REQUIRE(a.follow()==1);
    }
    SECTION("Necromancy"){
        Enemy a;
        a.setFraction(Fraction::Goblin);
        a.setDamage(1000);
        std::shared_ptr<Alive> alive = std::make_shared<Alive>(Alive());
        a.toAttack(*alive);
        std::shared_ptr<Enemy> clone = alive;
        std::vector<std::shared_ptr<Enemy>> enemies = {clone};
        REQUIRE(a.necromancy(*alive, UndeadType::Zombie, enemies)==1);
        std::shared_ptr<Undead> undead = std::dynamic_pointer_cast<Undead>(enemies[1]);
        REQUIRE(undead!=nullptr);
        REQUIRE(undead->getUndeadType()==UndeadType::Zombie);
    }
}

TEST_CASE("Undead"){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(101, 1000);
    Matrix<Cell> b(3, 4, Cell());
    std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
    Coordinates next{0, 2};
    SECTION("Combat"){
        Character b;
        b.setDamage(1000);
        Undead c;
        b.toAttack(c);
        REQUIRE(c.getCondition()==Condition::Deleted);
    }
}
TEST_CASE("Alive"){
    SECTION("Constructor"){
        Matrix<Cell> b(3, 4, Cell());
        std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
        std::shared_ptr<Character> hero = std::make_shared<Character>(Character());            
        std::shared_ptr<Creature> clone = hero;
        Alive a(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, Condition::Calm, "bob", clone);   
        REQUIRE(a.getName()=="bob");
    }
}
TEST_CASE("Summoners"){
    Matrix<Cell> b(3, 4, Cell());
    std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
    std::shared_ptr<Character> hero = std::make_shared<Character>(Character());            
    std::shared_ptr<Creature> clone = hero;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(11, 20);
    SECTION("Alive summoner empty constructor + cast"){
        AliveSummoner a;
        std::vector<std::shared_ptr<Enemy>> enemies={};
        REQUIRE(a.cast(enemies)==1);
        REQUIRE(enemies.size()==1);
    }
    SECTION("Alive summoner constructors"){
        std::shared_ptr<Alive> alive = std::make_shared<Alive>(Alive());
        AliveSummoner a(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, Condition::Calm, "bob", clone, 2, 1, alive); 
        REQUIRE(a.getCastCd()==2);
        REQUIRE(a.getSummon()!=nullptr);
        REQUIRE_THROWS_AS(AliveSummoner(2, 3, alive), std::invalid_argument);
        REQUIRE_THROWS_AS(AliveSummoner(3, 2, nullptr), std::invalid_argument);
        REQUIRE_THROWS_AS(AliveSummoner(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, Condition::Calm, "bob", clone, 2, 1, nullptr), std::invalid_argument);
        REQUIRE_THROWS_AS(AliveSummoner(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, Condition::Calm, "bob", clone, 1, 2, alive), std::invalid_argument);
    }
    SECTION("Alive summoner setters"){
        AliveSummoner a;
        size_t r = distrib(gen);
        REQUIRE(a.setCastCd(r).getCastCd()==r);
        REQUIRE(a.setCurrentCd(r-1).getCurrentCd()==r-1);
        REQUIRE_THROWS_AS(a.setCastCd(r-3).getCastCd(), std::invalid_argument);
        REQUIRE_THROWS_AS(a.setCurrentCd(r+5).getCurrentCd(), std::invalid_argument);
        a.decreaseCd();
    }
    SECTION("Undead summoner"){
        UndeadSummoner a;
        std::vector<std::shared_ptr<Enemy>> enemies={};
        REQUIRE(a.cast(enemies)==1);
        REQUIRE(enemies.size()==1);
    }
    SECTION("Undead summoner setters"){
        UndeadSummoner a;
        size_t r = distrib(gen);
        REQUIRE(a.setCastCd(r).getCastCd()==r);
        REQUIRE(a.setCurrentCd(r-1).getCurrentCd()==r-1);
        REQUIRE_THROWS_AS(a.setCastCd(r-3).getCastCd(), std::invalid_argument);
        REQUIRE_THROWS_AS(a.setCurrentCd(r+5).getCurrentCd(), std::invalid_argument);
        a.decreaseCd();
    }
}