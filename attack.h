#ifndef ATTACK_H
#define ATTACK_H

#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "attackrange.h"

class Attack : public QWidget { // 攻击实体类，包含子弹和斩击
    Q_OBJECT

protected:
    QPointF pos; // 是中心位置不是左上角位置
    double size; // 半径
    AttackRange *range;
    double damage;
    bool isPlayerSide;   // 是否是右方子弹或斩击
    bool isValid = true; // 是否能造成伤害
    QTimer validityTimer;

public:
    explicit Attack(QPoint initPos,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    int validTime,
                    QWidget *parent = nullptr);
    virtual ~Attack();

    QPoint getPos() const;
    AttackRange *getRange() const;
    double getDamage() const;
    double getPlayerSide() const;
    void setPlayerSide();
    double getValidity() const;

    QPainterPath createPath() const; // 返回一个当前子弹或斩击的范围的 QPainterPath
};

class Bullet : public Attack { // 子弹类
    Q_OBJECT

protected:
    QPixmap texture; // 贴图

    QPointF velocity;
    QPointF acceleration = QPointF(0, 0);

    QPointF prevPos; // 上一时刻的位置，用于生成子弹在两个时刻经过的路径

public:
    explicit Bullet(QPoint initPos,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    int validTime,
                    QWidget *parent = nullptr);

    void render(QPainter *painter);

    QPoint getPrevPos();

    void setAcceleration(QPointF a);
    void setVelocity(QPointF v);

    void moveActively(); // 根据速度属性进行移动

    bool isHit(const QRectF &targetRect); // 是否命中一个矩形对象
    bool isHit(const QPainterPath &path); // 是否命中一个特殊形状对象
};

class Slash : public Attack { // 斩击类
public:
    explicit Slash(QPoint pos,
                   double size,
                   double startAngle,
                   double spanAngle,
                   double damage,
                   bool isPlayerSide,
                   int validTime,
                   QWidget *parent = nullptr);

    bool isHit(const QRectF &targetRect);

    void render(QPainter *painter);

protected:
    QPixmap texture;

    double startAngle; // 扇形中线的起始角度，以 x 轴正方向为 0 度
    double spanAngle;  // 扇形的张角
};

#endif // ATTACK_H
