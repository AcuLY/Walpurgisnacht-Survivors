#ifndef LOOT_H
#define LOOT_H

#include <QPoint>
#include <QWidget>

class Loot : public QWidget { // 掉落物抽象类
    Q_OBJECT
public:
    explicit Loot(QPoint pos, QWidget* parent = nullptr);

    void inPickRange();       // 设置进入拾取范围
    bool getIsPicked() const; // 是否已经被拾取

    void moveToPlayer(QPoint&& playerPos); // 向玩家移动

    virtual void render(QPainter* painter) = 0;

private:
    double velocity = -5;
    double acceleration = 1;

    bool isInPickRange = false; // 进入拾取范围
    bool isPicked = false;      // 拾取完毕

    virtual void onLootPicked(); // 拾取后的效果
signals:
    void lootPicked();
};

class Experience : public Loot { // 经验点
    Q_OBJECT
public:
    explicit Experience(int value, QPoint pos, QWidget* parent = nullptr);
    void render(QPainter* painter) override;

private:
    QPixmap texture = QPixmap(":/images/item/exp");

    int value; // 经验值

    void onLootPicked() override;

signals:
    void experiencePicked(int value);
};

class GriefSeedFragment : public Loot { // 悲叹之种碎片
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
