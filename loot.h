#ifndef LOOT_H
#define LOOT_H

#include <QPoint>
#include <QWidget>

#include "utils.h"

class Loot : public QWidget {
    Q_OBJECT
public:
    explicit Loot(QPoint pos, QWidget* parent = nullptr);

    void inPickRange();
    bool getIsPicked() const;

    void moveToPlayer(QPoint&& playerPos);

private:
    double velocity;
    double acceleration = 0.5;

    bool isInPickRange = false; // 进入拾取范围
    bool isPicked = false;      // 拾取完毕

    virtual void onLootPicked();
signals:
    void lootPicked();
};

class Experience : public Loot {
    Q_OBJECT
public:
    explicit Experience(int value, QPoint pos, QWidget* parent = nullptr);

private:
    int value;

    void onLootPicked() override;
signals:
    void experiencePicked(int value);
};

#endif // LOOT_H
