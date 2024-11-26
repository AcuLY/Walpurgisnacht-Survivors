#include "witch.h"

Witch::Witch(QString name,
             int width,
             int height,
             double health,
             double maxVelocity,
             double accelerationFactor,
             double reboundFactor,
             Weapon *weapon,
             QWidget *parent)
    : Character(name,
                width,
                height,
                health,
                maxVelocity,
                accelerationFactor,
                reboundFactor,
                weapon,
                parent) {};

bool Witch::getValidity() {
    return isValid;
}

void Witch::setValidity() {
    isValid = !isValid;
}

Direction Witch::chooseDirection(Character *player) {
    Direction directions[]
        = {West, South, North, East, Center, NorthEast, NorthWest, SouthEast, SouthWest};

    double minDistance = INF;
    Direction optimalDirection;

    for (auto dir : directions) {
        auto [moveX, moveY] = ~dir;
        int newX = moveX * maxVelocity + this->x(), newY = moveY * maxVelocity + this->y();
        QPointF playerPos(player->x(), player->y()), newPos(newX, newY);
        double distance = MathUtils::euclideanDistance(playerPos, newPos);

        if (distance < minDistance) {
            minDistance = distance;
            optimalDirection = dir;
        }
    }

    return optimalDirection;
}

void Witch::moveActively(Character *player) {
    Direction dir = chooseDirection(player);
    auto [moveX, moveY] = ~dir;
    updateAcceleration(moveX, moveY);
    updateVelocity();
    updateQPointF();
}
