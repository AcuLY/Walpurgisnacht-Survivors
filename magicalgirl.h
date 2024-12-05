#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QWidget>

#include "character.h"
#include "skill.h"
#include "weapon.h"

class MagicalGirl : public Character {
    Q_OBJECT

protected:
    int mana;

    SkillManager *dodgeSkill;
    SkillManager *attackSkill1;
    SkillManager *ultimateSkill;

    void loadSkill();

public:
    explicit MagicalGirl(QString name,
                         int width,
                         int height,
                         double maxHealth,
                         double maxVelocity,
                         double accelerationFactor,
                         double reboundFactor,
                         int mana,
                         Weapon *weapon,
                         QWidget *parent = nullptr);

    void setMaxHealth(double newMaxHealth);

    virtual AttackRange *useDodgeSkill() = 0;

signals:
};

class Madoka : public MagicalGirl {
public:
    explicit Madoka(QString name,
                    int width,
                    int height,
                    double maxHealth,
                    double maxVelocity,
                    double accelerationFactor,
                    double reboundFactor,
                    int mana,
                    Weapon *weapon,
                    QWidget *parent = nullptr);

    AttackRange *useDodgeSkill() override;
};

#endif // MAGICALGIRL_H
