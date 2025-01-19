#include "loot.h"
#include "qpainter.h"
#include "utils.h"

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
    // 如果未进入拾取范围则不移动
    if (!isInPickRange) {
        return;
    }

    QPoint curPos(this->x(), this->y());
    int dis = MathUtils::euclideanDistance(curPos, playerPos);

    // 朝玩家方向加速
    double degree = MathUtils::calculateDegree(QPoint(this->x(), this->y()), playerPos);
    QPointF movement = MathUtils::velocityDecomQPointF(velocity, degree);
    this->move(this->x() + movement.x(), this->y() + movement.y());

    // 当与玩家的距离小于掉落物的速度时必定被拾取
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

void Experience::render(QPainter *painter) {
    painter->drawPixmap(this->pos(), texture);
}

GriefSeedFragment::GriefSeedFragment(QPoint pos, QWidget *parent) : Loot(pos, parent) {
}

void GriefSeedFragment::onLootPicked() {
    emit griefSeedFragmentPicked();
}

void GriefSeedFragment::render(QPainter *painter) {
    painter->drawPixmap(this->pos(), texture);
}
