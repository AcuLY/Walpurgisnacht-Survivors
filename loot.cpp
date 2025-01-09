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

    QPoint prevPos(this->x(), this->y());

    double degree = MathUtils::calculateDegree(QPoint(this->x(), this->y()), playerPos);
    QPointF movement = MathUtils::velocityDecomQPointF(velocity, degree);
    this->move(this->x() + movement.x(), this->y() + movement.y());

    QPoint curPos(this->x(), this->y());

    // 是否穿过了玩家位置，即被拾取
    if ((playerPos.x() - curPos.x()) * (playerPos.x() - prevPos.x()) < 0
        || (playerPos.y() - curPos.x()) * (playerPos.y() - prevPos.y()) < 0) {
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
