#include "loot.h"

Loot::Loot(QPoint pos, QWidget *parent) : QWidget{parent} {
    this->setFixedSize(5, 5);
    this->move(pos.x(), pos.y());
}

void Loot::inPickRange() {
    isInPickRange = true;
}

bool Loot::getIsPicked() const {
    return isPicked;
}

void Loot::moveToPlayer(QPoint &&playerPos) {
    if (!isInPickRange) {
        return;
    }

    QPoint curPos(this->x(), this->y());
    int dis = MathUtils::euclideanDistance(curPos, playerPos);

    double degree = MathUtils::calculateDegree(QPoint(this->x(), this->y()), playerPos);
    QPointF movement = MathUtils::velocityDecomQPointF(velocity, degree);
    this->move(this->x() + movement.x(), this->y() + movement.y());

    // 与玩家的距离小于速度时必定被拾取
    if (dis < velocity) {
        isPicked = true;
        onLootPicked();
    }

    velocity += acceleration;
}

void Loot::onLootPicked() {
    emit lootPicked();
}

Experience::Experience(int value, QPoint pos, QWidget *parent) : Loot(pos, parent), value(value) {
}

void Experience::onLootPicked() {
    emit experiencePicked(value);
}

GriefSeedFragment::GriefSeedFragment(QPoint pos, QWidget *parent) : Loot(pos, parent) {
}

void GriefSeedFragment::onLootPicked() {
    emit griefSeedFragmentPicked();
}
