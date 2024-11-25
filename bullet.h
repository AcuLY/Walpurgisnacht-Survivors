#ifndef BULLET_H
#define BULLET_H

#include <QWidget>

class Bullet : public QWidget {
    Q_OBJECT

protected:
    double size;
    double damage;
    double velocityX;
    double velocityY;
    double accelerationX = 0;
    double accelerationY = 0;

    bool isPlayerSide;

    bool isValid = true;

    QPointF prevPos;

public:
    explicit Bullet(int x,
                    int y,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    QWidget *parent = nullptr);

    double getSize() const;
    bool getSide() const;
    bool getValidity() const;

    void setAcceleration(double aX, double aY);
    void setVelocity(double vX, double vY);
    void setValidity();

    void moveActively();

    bool isHit(const QRectF &targetRect);

signals:
};

#endif // BULLET_H
