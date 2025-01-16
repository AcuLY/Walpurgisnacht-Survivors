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

    virtual void render(QPainter* painter) = 0;

private:
    double velocity = -5;
    double acceleration = 1;

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
    void render(QPainter* painter) override;

private:
    QPixmap texture = QPixmap(":/images/item/exp");

    int value;

    void onLootPicked() override;

signals:
    void experiencePicked(int value);
};

class GriefSeedFragment : public Loot {
    Q_OBJECT
public:
    explicit GriefSeedFragment(QPoint pos, QWidget* parent = nullptr);
    void render(QPainter* painter) override;

private:
    QPixmap texture = QPixmap(":/images/item/grief_seed");

    void onLootPicked() override;

signals:
    void griefSeedFragmentPicked();
};

#endif // LOOT_H
