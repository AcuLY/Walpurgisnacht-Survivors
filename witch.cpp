#include "witch.h"

Witch::Witch(QString name,
             int width,
             int height,
             double health,
             double damage,
             double maxVelocity,
             double accelerationFactor,
             double reboundFactor,
             QWidget *parent)
    : Character(name,
                width,
                height,
                health,
                damage,
                maxVelocity,
                accelerationFactor,
                reboundFactor,
                parent) {};

Direction Witch::chooseDirection(Character *player) {
    Direction directions[]
        = {West, South, North, East, Center, NorthEast, NorthWest, SouthEast, SouthWest};

    double minDistance = 1e9;
    Direction optimalDirection;

    for (auto dir : directions) {
        auto [moveX, moveY] = ~dir;
        int newX = moveX * maxVelocity + this->x();
        int newY = moveY * maxVelocity + this->y();
        double distance = MathUtils::euclideanDistance(player->x(), player->y(), newX, newY);
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
    updatePosition();
}
