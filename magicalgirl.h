#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QWidget>

#include "character.h"

class MagicalGirl : public Character {
    Q_OBJECT

protected:
    int mana;

public:
    explicit MagicalGirl(QString name,
                         int width,
                         int height,
                         double health,
                         double damage,
                         double maxVelocity,
                         double accelerationFactor,
                         double reboundFactor,
                         int mana,
                         QWidget *parent = nullptr);

signals:
};

#endif // MAGICALGIRL_H
