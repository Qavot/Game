#include "./dungeon/dungeon.hpp"
#include "./creature/enemy/alive/aliveSummoner/aliveSummoner.hpp"
#include "./creature/enemy/undead/undeadSummoner/undeadSummoner.hpp"
using namespace dng;
using namespace creature;

int main(){
    Matrix<Cell> b(20, 20, Cell());
    for (int i=0; i<19; i++){
        b[i][1].setCellType(CellType::Wall);
    }
    std::shared_ptr<Matrix<dng::Cell>> field=std::make_shared<Matrix<dng::Cell>>(b);
    Coordinates next{10, 10};
    std::shared_ptr<Character> hero = std::make_shared<Character>(Character());
    hero->setField(field);
    hero->setCoordinates(next);    
    std::shared_ptr<Creature> clone = hero;
    Enemy a(Coordinates(0, 0), field, 20, 20, Fraction::Goblin, 100, EnemyType::Alive, Condition::Calm,"bob", clone);
    std::cout<<a.getCoordinates().x<<"  "<<a.getCoordinates().y<<std::endl;
    std::cout<<clone->getCoordinates().x<<"  "<<clone->getCoordinates().y<<std::endl;
    int d=0;
    while((a.getCoordinates().x!=10 || a.getCoordinates().y!=10) && d++<30){
        std::cout<<a.follow()<<std::endl;
        std::cout<<a.getCoordinates().x<<"  "<<a.getCoordinates().y<<std::endl;
    }
    return 0;
}